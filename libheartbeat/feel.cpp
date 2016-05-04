/****************************************************************************
 *	@file		Feel.cpp
 *	@brief		Implement of Feel.hpp
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 ***************************************************************************/

#include <opencv2/opencv.hpp>

#include "log.hpp"
#include "datatype.hpp"
#include "Feel.hpp"

/****************************************************************************
 *	private Impl
 ***************************************************************************/

typedef struct hb::Feel::Impl
{
public:

	cv::Mat view;

	bool is_ready;
	cv::Mat me_tmpl;
	cv::Mat bird_tmpl;
	cv::Mat cactus_tmpl;
	cv::Mat restart_tmpl;

public:

	static const int    MATCH_METHOD;

public:

	static void load_tmpl(bool & is_ok, cv::Mat & dst, char const path[], char const info[]);

	static bool feel_existence(cv::Mat const & src, cv::Mat const & tmpl, int threshold);

	static std::vector<Sense> feel_one(cv::Mat const & src, cv::Mat const & tmpl, int threshold);

	static void merge_neighbor(std::vector<Sense> & rects, int delta_width, int delta_height);

	static std::vector<Sense> feel_multi(cv::Mat const & src, cv::Mat const & tmpl, int threshold);

} Private;

const int    Private::MATCH_METHOD           = cv::TM_CCOEFF_NORMED;

/****************************************************************************
 *	private method
 ***************************************************************************/

void hb::Feel::Impl::load_tmpl
(
	bool &	   is_ok, 
	cv::Mat &  dst, 
	char const path[], 
	char const info[]
)
{
	dst = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	if (dst.empty()) {
		Log::Err("load_tmpl::I cannot see ", info);
		is_ok = false;
	}
}

bool hb::Feel::Impl::feel_existence(cv::Mat const & src, cv::Mat const & tmpl, int threshold)
{
	if (src.empty() || tmpl.empty())
		return false;

	cv::Mat score;
	cv::matchTemplate(src, tmpl, score, MATCH_METHOD);
	if (MATCH_METHOD == CV_TM_SQDIFF || MATCH_METHOD == CV_TM_SQDIFF_NORMED)
		score.convertTo(score, -1, -1.0, 1.0);

	double max;
	cv::minMaxLoc(score, nullptr, &max, nullptr, nullptr);
	return (max * 255.0 > threshold);
}

std::vector<hb::Sense> hb::Feel::Impl::feel_one(cv::Mat const & src, cv::Mat const & tmpl, int threshold)
{
	if (src.empty() || tmpl.empty())
		return std::vector<Sense>();

	cv::Mat score;
	cv::matchTemplate(src, tmpl, score, MATCH_METHOD);

	/* note:
	 * "For SQDIFF and SQDIFF_NORMED, the best matches are lower values. 
	 *  For all the other methods, the higher the better."
	 */
	if (MATCH_METHOD == CV_TM_SQDIFF || MATCH_METHOD == CV_TM_SQDIFF_NORMED)
		score.convertTo(score, -1, -1.0, 1.0);

	/* find the only one matched */
	double max;
	cv::Point match;
	cv::minMaxLoc(score, nullptr, &max, nullptr, &match);

	if (max * 255.0 > threshold)
		return std::vector<Sense>{ {match.x, match.y, tmpl.cols, tmpl.rows, float(max)}};
	else
		return std::vector<Sense>();

	/* References:
	 * [Template Matching]
	 * (http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html)
	 */
}

void hb::Feel::Impl::merge_neighbor(std::vector<Sense>& rects, int delta_width, int delta_height)
{
	size_t n = rects.size();
	for (size_t i = 0U; i < n; i++) {
		auto & lhs = rects[i];
		int x    = lhs.x -  delta_width;
		int y    = lhs.y -  delta_height;
		int wid  = lhs.w + (delta_width  << 1);
		int hgt  = lhs.h + (delta_height << 1);
		for (size_t j = i + 1U; j < n; j++) {
			auto & rhs = rects[j];
			int xmin = x          - rhs.w;
			int ymin = y          - rhs.h;
			int xmax = xmin + wid + rhs.w;
			int ymax = ymin + hgt + rhs.h;
			if (xmin <= rhs.x && rhs.x <= xmax && ymin <= rhs.y && rhs.y <= ymax) {
				int left   = std::min(lhs.x, rhs.x);
				int right  = std::max(lhs.x + lhs.w, rhs.x + rhs.w);
				int top    = std::min(lhs.y, rhs.y);
				int bottom = std::max(lhs.y + lhs.h, rhs.y + rhs.h);
				lhs.x      = left;
				lhs.y      = top;
				lhs.h      = bottom - top;
				lhs.w      = right - left;
				lhs.s      = std::max(lhs.s, rhs.s); // TODO: is this ok?
				if (j != --n)
					std::swap(rects[n], rhs);
				i = size_t(0U) - size_t(1U);	/* restart merging */
			}
		}
	}

	n = rects.size() - n;
	while (n-- > 0U)
		rects.pop_back();
}

std::vector<hb::Sense> hb::Feel::Impl::feel_multi(cv::Mat const & src, cv::Mat const & tmpl, int threshold)
{
	if (src.empty() || tmpl.empty())
		return std::vector<Sense>();

	cv::Mat score;
	cv::matchTemplate(src, tmpl, score, MATCH_METHOD);

	if (MATCH_METHOD == CV_TM_SQDIFF || MATCH_METHOD == CV_TM_SQDIFF_NORMED)
		score.convertTo(score, CV_8UC1, -255.0, 255.0);
	else
		score.convertTo(score, CV_8UC1, 255.0);

	cv::Mat mask;
	cv::threshold(score, mask, threshold, 255, CV_THRESH_BINARY);

	std::vector<Sense> rv;
	for (auto iter = mask.datastart; iter != mask.dataend; iter++) {
		if (*iter == 255) {
			int i = (iter - mask.datastart);
			int x = i % mask.cols;
			int y = i / mask.cols;

			/* fill the hole for search next */
			cv::Rect r;
			cv::floodFill(mask, cv::Point(x, y), cv::Scalar::all(0), &r);

			/* get the max score */
			double max;
			cv::Point match;
			cv::Mat roi = score(r);
			cv::minMaxLoc(roi, nullptr, &max, nullptr, &match);
			rv.push_back({x + match.x, y + match.y, tmpl.cols, tmpl.rows, float(max) / 255.f});
		}
	}

	return rv;
}

/****************************************************************************
 *	public method
 ***************************************************************************/

cv::Mat const & hb::Feel::oh_i_feel(cv::Mat const & src)
{
	auto & dst = impl->view;

	/* [1]grayscale    --- convert to single-channel */
	cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);

	/* [2]contrast     --- highlight foreground */
	dst.convertTo(dst, -1, CONTRAST_ALPHA, 0.0);

	/* [3]equalization --- increase the color discrimination */
	cv::equalizeHist(dst, dst);

	/* [4]binarization --- remove background */
	bool is_inv = (float)cv::countNonZero(dst) > dst.total() * 0.666f;
	cv::threshold(dst, dst, BINARIZATION_THRESHOLD, 255.0, (is_inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY));

	if ((float)cv::countNonZero(dst) > dst.total() * 0.5f)
		cv::bitwise_not(dst, dst);

	/* [5]open         --- remove clouds, roads and some noise */
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::erode (dst, dst, kernel);
	cv::dilate(dst, dst, kernel);

	return dst;

	/* References:
	 * 1. http://docs.opencv.org/2.4/doc/tutorials/core/basic_linear_transform/basic_linear_transform.html
	 */
}

bool hb::Feel::is_game_over() const
{
	auto & i = *impl;
	if (!i.is_ready)
		return false;

	return Private::feel_existence(i.view, i.restart_tmpl, MATCH_TH_RESTART);
}

std::vector<hb::Sense> hb::Feel::where_are(ObjectType type) const
{
	auto & i = *impl;
	if (!i.is_ready)
		return std::vector<Sense>();

	std::vector<Sense> rv;
	
	switch (type)
	{
	case hb::ObjectType::DINOSAUR:
		rv = Private::feel_one(i.view, i.me_tmpl, MATCH_TH_DINOSAUR);
		break;
	case hb::ObjectType::CACTUS:
		rv = Private::feel_multi(i.view, i.cactus_tmpl, MATCH_TH_CACTUS);
		Private::merge_neighbor(rv, 16, 0);
		break;
	case hb::ObjectType::BIRD:
		rv = Private::feel_multi(i.view, i.bird_tmpl, MATCH_TH_BIRD);
		break;
	default:
		break;
	}

	return rv;
}

/****************************************************************************
 *	public Constructor \ Destructor
 ***************************************************************************/

hb::Feel::Feel()
	: impl(std::make_unique<Impl>())
{
	auto & i = *impl;

	i.is_ready = true;
	Private::load_tmpl(i.is_ready, i.     me_tmpl, "di.bmp", "me"    );
	Private::load_tmpl(i.is_ready, i.   bird_tmpl, "bi.bmp", "birds" );
	Private::load_tmpl(i.is_ready, i. cactus_tmpl, "ob.bmp", "cactus");
	Private::load_tmpl(i.is_ready, i.restart_tmpl, "go.bmp", "restart");
}

hb::Feel::~Feel() {}

