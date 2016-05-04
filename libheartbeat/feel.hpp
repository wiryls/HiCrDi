/*********************************************************************
 *	@file		Feel.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <memory>
#include <vector>

#include "datatype.hpp"

namespace cv {
	class Mat;
	template<typename _Tp> class Rect_;
	typedef Rect_<int> Rect2i;
	typedef Rect2i Rect;
}

namespace hb {
	enum struct ObjectType;
	struct Sense;
	class Feel;
}

/**
 *	\brief	TODO:
 */
class hb::Feel {

public:

	cv::Mat const & oh_i_feel(cv::Mat const & view);

	bool is_game_over()  const;

	std::vector<Sense> where_are(ObjectType type) const;

public:

	/**
	 *	\brief constructor
	 */
	Feel();

	/**
	 *	\brief destructor
	 */
	~Feel();
	 
public:

	RangedValue<double> CONTRAST_ALPHA			= RangedValue<double>(0.0, 3.0, 1.96);
	RangedValue<int>	BINARIZATION_THRESHOLD	= RangedValue<int>   (  0, 255, 249);
	RangedValue<int>	MATCH_TH_DINOSAUR		= RangedValue<int>   (  0, 255, 205);
	RangedValue<int>	MATCH_TH_CACTUS			= RangedValue<int>   (  0, 255, 144);
	RangedValue<int>	MATCH_TH_BIRD			= RangedValue<int>   (  0, 255, 196);
	RangedValue<int>	MATCH_TH_RESTART		= RangedValue<int>   (  0, 255, 196);

private:

	struct Impl;
	std::unique_ptr<Impl> impl;
};
