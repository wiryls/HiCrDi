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
#include <sstream>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "log.hpp"
#include "timer.hpp"
#include "feel.hpp"
#include "cogn.hpp"
#include "libheartbeat.hpp"

/* function */

cm::Timer timer;
cv::Mat src, dst;
hb::Feel feel;
hb::Cogn cogn;
char const wname[] = "test";

static void recalc(bool show = true) 
{
	float delta_time = (timer.getElapsedTime() / 1000.0f);
	if (delta_time > 1.0f)
		delta_time = 0.001f;
	timer.start();

	dst = feel.oh_i_feel(src);
	char buffer[11];
	auto dinosaur_senses = feel.where_are(hb::ObjectType::DINOSAUR);
	auto cactus_senses = feel.where_are(hb::ObjectType::CACTUS);
	auto birds_senses = feel.where_are(hb::ObjectType::BIRD);
	{
		for (auto & s : dinosaur_senses) {
			cv::Rect r = hb::toRect(s);
			std::sprintf(buffer, "%.2f", s.s);
			cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
			cv::rectangle(dst, r, cv::Scalar::all(128));
		}
	}
	{
		for (auto & s : cactus_senses) {
			cv::Rect r = hb::toRect(s);
			std::sprintf(buffer, "%.2f", s.s);
			cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
			cv::rectangle(dst, r, cv::Scalar::all(255));
		}

		for (auto & s : birds_senses) {
			cv::Rect r = hb::toRect(s);
			std::sprintf(buffer, "%.2f", s.s);
			cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
			cv::rectangle(dst, r, cv::Scalar::all(196));
		}
		if (feel.is_game_over())
			cv::bitwise_not(dst, dst);
	}
	{
		cogn.i_recognize(hb::ObjectType::DINOSAUR, dinosaur_senses, delta_time);
		cogn.i_recognize(hb::ObjectType::CACTUS  , cactus_senses, delta_time);
		cogn.i_recognize(hb::ObjectType::BIRD    , birds_senses, delta_time);

		int x, y;
		cogn.i_am_at(x, y);
		auto infos = cogn.then_i_know();
		for (auto & info : infos) {
			cv::Rect r = hb::toRect(info, x, y);
			std::sprintf(buffer, "%u", info.id);
			cv::putText(dst, buffer, r.tl(), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar::all(255));
			std::sprintf(buffer, "%.2f", info.speed_x);
			cv::putText(dst, buffer, r.tl() + cv::Point(0, 16), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar::all(225));
			cv::rectangle(dst, r, cv::Scalar::all(196), 3);
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
