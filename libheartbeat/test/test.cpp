/*********************************************************************
 *	@file		test.cpp
 *	@brief		I'll do some experiments.
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 *********************************************************************/ 

/* headers */
#define _CRT_SECURE_NO_WARNINGS
#include <sstream>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "../utility/log.hpp"
#include "../utility/timer.hpp"
#include "../impl/cvfeel.hpp"
#include "../impl/kmcogn.hpp"
#include "../impl/qlmind.hpp"
#include "../libheartbeat.hpp"

/* function */

cm::Timer timer;
cv::Mat src, dst;
hb::CvFeel feel;
hb::KmCogn cogn;
hb::QlMind mind;
char const wname[] = "test";

static void recalc(bool show = true) 
{
	float delta_time = (timer.getElapsedTime() / 1000.0f);
	if (delta_time > 1.0f)
		delta_time = 0.001f;
	timer.start();

	hb::View view{src, delta_time};

	hb::Sense sense;
	hb::Info info;
	hb::Action action;

	feel << view  >> sense;
	cogn << sense >> info;
	mind << info  >> action;

	char buffer[11];
	{
		dst = sense.debug_view.clone();

		for (auto & s : sense.dinosaurs) {
			cv::Rect r = hb::toRect(s);
			std::sprintf(buffer, "%.2f", s.s);
			cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
			cv::rectangle(dst, r, cv::Scalar::all(128));
		}
	}
	{
		for (auto & s : sense.cactus) {
			cv::Rect r = hb::toRect(s);
			std::sprintf(buffer, "%.2f", s.s);
			cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
			cv::rectangle(dst, r, cv::Scalar::all(255));
		}

		for (auto & s : sense.birds) {
			cv::Rect r = hb::toRect(s);
			std::sprintf(buffer, "%.2f", s.s);
			cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
			cv::rectangle(dst, r, cv::Scalar::all(196));
		}

		if (sense.is_dead)
			cv::bitwise_not(dst, dst);
	}

	{
		for (auto & i : info.objs) {
			cv::Rect r = hb::toRect(i, info.x0, info.y0);
			std::sprintf(buffer, "%u", i.id);
			cv::putText(dst, buffer, r.tl(), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar::all(255));
			std::sprintf(buffer, "%.2f", i.speed_x);
			cv::putText(dst, buffer, r.tl() + cv::Point(0, 16), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar::all(225));
			cv::rectangle(dst, r, cv::Scalar::all(196), 3);

			if (i.id == 10) {
				Log::Msg(
					"dx: ", int(i.delta_x), "\t", 
					"dy: ", int(i.delta_y), "\t", 
					"rw: ", int(i.relative_width), "\t",
					"rh: ", int(i.relative_height), "\t",
					"sx: ", int(i.speed_x), "\t",
					"sy: ", int(i.speed_y), "\t"
					);
			}
		}
	}

	if (show)
		cv::imshow("new", dst);
}

LIBHEARTBEAT_API void hb::test(uint8_t const bgra32[], uint8_t rv[], size_t wid, size_t hgt)
{
	cm::Log::Switch(cm::Log::Type::ERR, cm::Log::State::FILE);
	cm::Log::Switch(cm::Log::Type::DBG, cm::Log::State::FILE);
	cm::Log::Switch(cm::Log::Type::MSG, cm::Log::State::FILE);

	/**************** read file ****************/
	if (bgra32 == nullptr) {
		std::string filename = "s05.png";
		src = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
		if(src.empty()) {
			std::cout << "cannot read image file: " << filename << std::endl;
			return;
		}
	} else {
		src = cv::Mat(hgt, wid, CV_8UC4, (void *)bgra32);
		cv::cvtColor(src, src, cv::COLOR_BGRA2BGR);
	}

	/**************** calc ****************/

	{	/**************** display ****************/
		if (bgra32 == nullptr) {

			cv::namedWindow(wname);

			cv::createTrackbar("alpha", wname, nullptr, 300, [](int v, void*) {
				feel.CONTRAST_ALPHA = float(v) / 100;
				recalc();
			});
			cv::createTrackbar("th", wname, &(int)feel.BINARIZATION_THRESHOLD, 255, [](int, void*) {
				recalc();
			});
			cv::createTrackbar("di_th", wname, &(int)feel.MATCH_TH_DINOSAUR, 255, [](int, void*) {
				recalc();
			});
			cv::createTrackbar("ob_th", wname, &(int)feel.MATCH_TH_CACTUS, 255, [](int, void*) {
				recalc();
			});
			cv::createTrackbar("bi_th", wname, &(int)feel.MATCH_TH_BIRD, 255, [](int, void*) {
				recalc();
			});

			recalc(false);

			cv::imshow("new", dst);
			cv::waitKey(0);

		} else {
			recalc(false);
			int i = 0;
			for (auto iter = dst.datastart; iter != dst.dataend; iter++) {
				rv[i++] = *iter;
				rv[i++] = *iter;
				rv[i++] = *iter;
				rv[i++] = 0xFFU;
			}
		}
	}

	return;
}
