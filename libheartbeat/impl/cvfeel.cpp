/****************************************************************************
 *	@file		cvfeel.cpp
 *	@brief		Implement of cvfeel.hpp
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 ***************************************************************************/

#include <opencv2/opencv.hpp>

#include "../utility/log.hpp"
#include "cvfeel.hpp"

/****************************************************************************
 *	private Impl
 ***************************************************************************/

const int hb::CvFeel::MATCH_METHOD = cv::TM_CCOEFF_NORMED;

/****************************************************************************
 *	private
 ***************************************************************************/

void hb::CvFeel::load_tmpl(bool & is_ok, cv::Mat & dst, char const path[], char const err_msg[]) const
{
	dst = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	if (dst.empty()) {
		Log::Err("load_tmpl::I cannot see ", err_msg);
		is_ok = false;
	}
}

void hb::CvFeel::see(cv::Mat const & src)
{
	auto & dst = this->view;

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

	/* References:
	 * 1. http://docs.opencv.org/2.4/doc/tutorials/core/basic_linear_transform/basic_linear_transform.html
	 */
}

bool hb::CvFeel::feel_existence(cv::Mat const & tmpl, int threshold) const
{
	cv::Mat score;
	cv::matchTemplate(view, tmpl, score, MATCH_METHOD);
	if (MATCH_METHOD == CV_TM_SQDIFF || MATCH_METHOD == CV_TM_SQDIFF_NORMED)
		score.convertTo(score, -1, -1.0, 1.0);

	double max;
	cv::minMaxLoc(score, nullptr, &max, nullptr, nullptr);
	return (max * 255.0 > threshold);
}

std::vector<hb::IFeel::Output::Obj> hb::CvFeel::feel_one(cv::Mat const & tmpl, int threshold) const
{
	cv::Mat score;
	cv::matchTemplate(view, tmpl, score, MATCH_METHOD);

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
		return std::vector<Sense::Obj>{ {match.x, match.y, tmpl.cols, tmpl.rows, float(max)}};
	else
		return std::vector<Sense::Obj>();

	/* References:
	 * [Template Matching]
	 * (http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html)
	 */
}

std::vector<hb::IFeel::Output::Obj> hb::CvFeel::feel_several(cv::Mat const & tmpl, int threshold) const
{
	cv::Mat score;
	cv::matchTemplate(view, tmpl, score, MATCH_METHOD);

	if (MATCH_METHOD == CV_TM_SQDIFF || MATCH_METHOD == CV_TM_SQDIFF_NORMED)
		score.convertTo(score, CV_8UC1, -255.0, 255.0);
	else
		score.convertTo(score, CV_8UC1, 255.0);

	cv::Mat mask;
	cv::threshold(score, mask, threshold, 255, CV_THRESH_BINARY);

	std::vector<Sense::Obj> rv;
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

void hb::CvFeel::merge_adjacent(std::vector<Output::Obj>& rects, int delta_width, int delta_height) const
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

/****************************************************************************
 *	public
 ***************************************************************************/

hb::CvFeel::CvFeel()
	: is_ready(true)
	, is_valid(false)
{
	load_tmpl(is_ready,  dinosaur_tmpl,  "di.bmp", "me"     );
	load_tmpl(is_ready, dinosaur2_tmpl, "di2.bmp", "me2"    );
	load_tmpl(is_ready,      bird_tmpl,  "bi.bmp", "birds"  );
	load_tmpl(is_ready,    cactus_tmpl,  "ob.bmp", "cactus" );
	load_tmpl(is_ready,   restart_tmpl,  "go.bmp", "restart");
}

hb::IFeel & hb::CvFeel::operator<<(Input const & in)
{
	if (!is_ready || in.img.empty())
		return *this;

	see(in.img);
	is_valid = !view.empty();
	delta_time = in.delta_time;

	return *this;
}

hb::IFeel & hb::CvFeel::operator>>(Output & out)
{
	if (!is_ready || !is_valid)
		return *this;
	
	out.delta_time   = delta_time;
	out.debug_view   = view;

	out.is_dead = feel_existence(  restart_tmpl, MATCH_TH_RESTART);
	out.dinosaurs    = feel_one      ( dinosaur_tmpl, MATCH_TH_DINOSAUR);
	out.birds        = feel_several  (     bird_tmpl, MATCH_TH_BIRD);
	out.cactus       = feel_several  (   cactus_tmpl, MATCH_TH_CACTUS);

	merge_adjacent(out.cactus, 16, 0);
	if (out.dinosaurs.empty())
		out.dinosaurs = feel_one     (dinosaur2_tmpl, MATCH_TH_DINOSAUR);
	
	is_valid = false;
	return *this;
}

hb::CvFeel::operator bool() const
{
	return is_ready && is_valid;
}

void hb::CvFeel::clear()
{
	bool is_valid = true;
	float delta_time = 0.f;
}
