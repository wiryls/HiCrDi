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
	struct Info;
}

namespace hb {
	extern cv::Rect toRect(Sense  const & src);
	extern cv::Rect toRect(Info   const & src, int x0, int y0);
}

namespace hb {
	template<typename T> class RangedValue;
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

/****************************************************************************
 *	
 ***************************************************************************/

template<typename T> class hb::RangedValue
{
public:
	RangedValue(T const & min, T const & max, T const & val = T());
	T & operator = (T const & rhs);
	operator T &();
	operator T const &() const;

private:
	T val;
	T val_min;
	T val_max;
};


template<typename T> inline hb::RangedValue<T>::
RangedValue(T const & min, T const & max, T const & val)
	: val_min(min)
	, val_max(max)
	, val(val)
{
	if (min > max)
		throw std::invalid_argument("`min` > `max`");
	*this = val;
}

template<typename T> inline T & hb::RangedValue<T>::
operator=(T const & rhs)
{
	if (rhs < val_min)
		val = val_min;
	else if (val_max < rhs)
		val = val_max;
	else
		val = rhs;
	return *this;
}

template<typename T> inline hb::RangedValue<T>::
operator T&()
{
	return val;
}

template<typename T> inline hb::RangedValue<T>::
operator T const&() const
{
	return val;
}
