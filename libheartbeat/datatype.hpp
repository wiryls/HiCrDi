/*********************************************************************
 *	@file		datatype.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	25/04/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

namespace cv {
	template<typename _Tp> class Rect_;
	typedef Rect_<int> Rect2i;
	typedef Rect2i Rect;
}

namespace hb {
	enum struct ObjectType;
	struct Sense;
	struct Object;
	struct Info;
}

namespace hb {
	extern cv::Rect toRect(Sense  const & src);
	extern cv::Rect toRect(Object const & src);
	extern cv::Rect toRect(Info   const & src, int x0, int y0);
}

/****************************************************************************
 *	
 ***************************************************************************/

/**
 *	\brief	TODO:
 */
enum struct hb::ObjectType
{
	DINOSAUR,
	CACTUS,
	BIRD,
};


/**
 *	\brief	TODO:
 */
struct hb::Sense
{
	int x;
	int y;
	int w;
	int h;
	float s;
};


/**
 *	\brief	TODO:
 */
struct hb::Object
{
	float  x_mid;
	float  y_mid;
	float  wid;
	float  hgt;
	float  spd_x;
	float  spd_y;
	size_t frame;
	float  score;
	size_t id;
};


/**
 *	\brief	TODO:
 */
struct hb::Info
{
	float delta_x;
	float delta_y;
	float relative_width;
	float relative_height;
	float speed_x;
	float speed_y;
	size_t id;
};
