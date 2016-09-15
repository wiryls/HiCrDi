/*********************************************************************
 *	@file		datatype.cpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	25/04/16	MYLS		Creation.
 *
 *********************************************************************/

#include <opencv2/core/types.hpp>
#include "datatype.hpp"

/****************************************************************************
 *	implement
 ***************************************************************************/

cv::Rect hb::toRect(Sense::Obj const & src)
{
	return cv::Rect(src.x, src.y, src.w, src.h);
}

cv::Rect hb::toRect(Info::Obj const & src, float x0, float y0)
{
	return cv::Rect
	(
		static_cast<int>(x0 + src.delta_x - src.relative_width * 0.5f),
		static_cast<int>(y0 + src.delta_y - src.relative_height * 0.5f),
		static_cast<int>(src.relative_width),
		static_cast<int>(src.relative_height)
	);
}
