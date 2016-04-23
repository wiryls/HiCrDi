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
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "feel.hpp"
#include "libheartbeat.hpp"

/* function */

cv::Mat src, dst;
hb::Feel feel;
char const wname[] = "test";

static void recalc(bool show = true) 
{
	dst = feel.oh_i_feel(src);

	//cv::rectangle(dst, cv::Rect(5, 5, 100, 100), cv::Scalar::all(255));

	{
		auto r = feel.me();
		cv::rectangle(dst, r, cv::Scalar::all(255));
	}	
	{
		auto rs = feel.cactus();
		for (auto & r : rs)
			cv::rectangle(dst, r, cv::Scalar::all(196));
	}
	{
		auto rs = feel.birds();
		for (auto & r : rs)
			cv::rectangle(dst, r, cv::Scalar::all(128));
	}
	//{
	//	auto r = feel.score();
	//	cv::rectangle(dst, r, cv::Scalar::all(160));
	//}

	if (show)
		cv::imshow("new", dst);
}

LIBHEARTBEAT_API void hb::test(uint8_t const bgra32[], uint8_t rv[], size_t wid, size_t hgt)
{
	/**************** read file ****************/
	if (bgra32 == nullptr) {
		std::string filename = "2.bmp";
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

			cv::createTrackbar("th", wname, &feel.BINARIZATION_THRESHOLD, 255, [](int, void*) {
				recalc();
			});

			cv::createTrackbar("di_th", wname, &feel.MATCH_TH_DINOSAUR, 255, [](int, void*) {
				recalc();
			});

			cv::createTrackbar("ob_th", wname, &feel.MATCH_TH_CACTUS, 255, [](int, void*) {
				recalc();
			});

			cv::createTrackbar("bi_th", wname, &feel.MATCH_TH_BIRD, 255, [](int, void*) {
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
