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

public:

	static const int    MATCH_METHOD;

public:

	static double& CONTRAST_ALPHA;
	static int   & BINARIZATION_THRESHOLD;
	static int   & MATCH_TH_DINOSAUR;
	static int   & MATCH_TH_CACTUS;
	static int   & MATCH_TH_BIRD;

public:

	static void load_tmpl(bool & is_ok, cv::Mat & dst, char const path[], char const info[]);

	static cv::Rect feel_one(cv::Mat const & src, cv::Mat const & tmpl, int threshold);

	static void merge_neighbor(std::vector<cv::Rect> & rects, int delta_width, int delta_height);

	static std::vector<cv::Rect> feel_multi(cv::Mat const & src, cv::Mat const & tmpl, int threshold);

} Private;

const int    Private::MATCH_METHOD           = cv::TM_CCOEFF_NORMED;

// TODO:
double hb::Feel::CONTRAST_ALPHA = 1.96;
int    hb::Feel::BINARIZATION_THRESHOLD = 249;
int    hb::Feel::MATCH_TH_DINOSAUR = 128;
int    hb::Feel::MATCH_TH_CACTUS = 128;
int    hb::Feel::MATCH_TH_BIRD = 196;

// TODO:
double & Private::CONTRAST_ALPHA           = hb::Feel::CONTRAST_ALPHA;
int    & Private::BINARIZATION_THRESHOLD   = hb::Feel::BINARIZATION_THRESHOLD;
int    & Private::MATCH_TH_DINOSAUR        = hb::Feel::MATCH_TH_DINOSAUR;
int    & Private::MATCH_TH_CACTUS           = hb::Feel::MATCH_TH_CACTUS;
int    & Private::MATCH_TH_BIRD            = hb::Feel::MATCH_TH_BIRD;


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
		// TODO: Logging...
		std::cout << "I cannot see " << info << "!\n";
		is_ok = false;
	}
}

cv::Rect hb::Feel::Impl::feel_one(cv::Mat const & src, cv::Mat const & tmpl, int threshold)
{
	if (tmpl.empty())
		return cv::Rect();

	cv::Mat result;
	cv::matchTemplate(src, tmpl, result, MATCH_METHOD);

	/* note:
	 * "For SQDIFF and SQDIFF_NORMED, the best matches are lower values. 
	 *  For all the other methods, the higher the better."
	 */
	if (MATCH_METHOD == CV_TM_SQDIFF || MATCH_METHOD == CV_TM_SQDIFF_NORMED)
		result.convertTo(result, CV_8UC1, -255.0, 255.0);
	else
		result.convertTo(result, CV_8UC1, 255.0);

	double max;
	cv::Point match;
	cv::minMaxLoc(result, nullptr, &max, nullptr, &match);

	if (max > threshold)
		return cv::Rect(match.x, match.y, tmpl.cols, tmpl.rows);
	else
		return cv::Rect();

	/* References:
	 * [Template Matching]
	 * (http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html)
	 */
}

void hb::Feel::Impl::merge_neighbor(std::vector<cv::Rect>& rects, int delta_width, int delta_height)
{
	size_t n = rects.size();
	for (size_t i = 0U; i < n; i++) {
		auto & lhs = rects[i];
		int x    = lhs.x      -  delta_width;
		int y    = lhs.y      -  delta_height;
		int wid  = lhs.width  + (delta_width  << 1);
		int hgt  = lhs.height + (delta_height << 1);
		for (size_t j = i + 1U; j < n; j++) {
			auto & rhs = rects[j];
			int xmin = x          - rhs.width;
			int ymin = y          - rhs.height;
			int xmax = xmin + wid + rhs.width;
			int ymax = ymin + hgt + rhs.height;
			if (xmin <= rhs.x && rhs.x <= xmax && ymin <= rhs.y && rhs.y <= ymax) {
				int left   = std::min(lhs.x, rhs.x);
				int right  = std::max(lhs.x + lhs.width, rhs.x + rhs.width);
				int top    = std::min(lhs.y, rhs.y);
				int bottom = std::max(lhs.y + lhs.height, rhs.y + rhs.height);
				lhs.x      = left;
				lhs.y      = top;
				lhs.height = bottom - top;
				lhs.width  = right - left;
				std::swap(rhs, rects[--n]);
				i = size_t(0U) - size_t(1U);	/* restart merging */
			}
		}
	}

	n = rects.size() - n;
	while (n--)
		rects.pop_back();
}

std::vector<cv::Rect> hb::Feel::Impl::feel_multi(cv::Mat const & src, cv::Mat const & tmpl, int threshold)
{
	if (tmpl.empty())
		return std::vector<cv::Rect>();

	cv::Mat result;
	cv::matchTemplate(src, tmpl, result, MATCH_METHOD);

	if (MATCH_METHOD == CV_TM_SQDIFF || MATCH_METHOD == CV_TM_SQDIFF_NORMED)
		result.convertTo(result, CV_8UC1, -255.0, 255.0);
	else
		result.convertTo(result, CV_8UC1, 255.0);


	cv::threshold(result, result, threshold, 255, CV_THRESH_BINARY);

	std::vector<cv::Rect> rv;
	for (auto iter = result.datastart; iter != result.dataend; iter++) {
		if (*iter == 255) {
			int i = (iter - result.datastart);
			int x = i % result.cols;
			int y = i / result.cols;
			cv::floodFill(result, cv::Point(x, y), cv::Scalar::all(0));
			rv.emplace_back(x, y, tmpl.cols, tmpl.rows);
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
	dst.convertTo(dst, -1, Private::CONTRAST_ALPHA, 0.0);

	/* [3]equalization --- increase the color discrimination */
	cv::equalizeHist(dst, dst);

	/* [4]binarization --- remove background */
	bool is_inv = (float)cv::countNonZero(dst) > dst.total() * 0.666f;
	cv::threshold(dst, dst, Private::BINARIZATION_THRESHOLD, 255.0, (is_inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY));

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

cv::Rect hb::Feel::me() const
{
	auto & i = *impl;
	if (!i.is_ready)
		return cv::Rect();

	return Private::feel_one(i.view, i.me_tmpl, Private::MATCH_TH_DINOSAUR);
}

std::vector<cv::Rect> hb::Feel::cactus() const
{
	auto & i = *impl;
	if (!i.is_ready)
		return std::vector<cv::Rect>();

	auto rv = Private::feel_multi(i.view, i.cactus_tmpl, Private::MATCH_TH_CACTUS);
	Private::merge_neighbor(rv, 16, 0);
	return rv;
}

std::vector<cv::Rect> hb::Feel::birds() const
{
	auto & i = *impl;
	if (!i.is_ready)
		return std::vector<cv::Rect>();

	return Private::feel_multi(i.view, i.bird_tmpl, Private::MATCH_TH_BIRD);
}

/****************************************************************************
 *	public Constructor \ Destructor
 ***************************************************************************/

hb::Feel::Feel()
	: impl(std::make_unique<Impl>()) 
{
	auto & i = *impl;

	i.is_ready = true;
	Private::load_tmpl(i.is_ready, i.    me_tmpl, "di.bmp", "me"    );
	Private::load_tmpl(i.is_ready, i.  bird_tmpl, "bi.bmp", "birds" );
	Private::load_tmpl(i.is_ready, i.cactus_tmpl, "ob.bmp", "cactus");
}

hb::Feel::~Feel() {}
