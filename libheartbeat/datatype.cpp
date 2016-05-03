/*********************************************************************
 *	@file		datatype.cpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	25/04/16	MYLS		Creation.
 *
 *********************************************************************/

#include <opencv2/opencv.hpp>
#include "datatype.hpp"

/****************************************************************************
 *	implement
 ***************************************************************************/

cv::Rect hb::toRect(Sense const & src)
{
	return cv::Rect(src.x, src.y, src.w, src.h);
}

cv::Rect hb::toRect(Object const & src)
{
	return cv::Rect
	(
		static_cast<int>(src.x_mid - (src.wid * 0.5f)),
		static_cast<int>(src.y_mid - (src.hgt * 0.5f)),
		static_cast<int>(src.wid),
		static_cast<int>(src.hgt)
	);
}

cv::Rect hb::toRect(Info const & src, int x0, int y0)
{
	return cv::Rect
	(
		x0 + static_cast<int>(src.delta_x - (src.relative_width  * 0.5f)),
		y0 + static_cast<int>(src.delta_y - (src.relative_height * 0.5f)),
		static_cast<int>(src.relative_width),
		static_cast<int>(src.relative_height)
	);
}
