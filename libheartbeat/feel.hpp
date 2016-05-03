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

	static double CONTRAST_ALPHA;
	static int    BINARIZATION_THRESHOLD;
	static int    MATCH_TH_DINOSAUR;
	static int    MATCH_TH_CACTUS;
	static int    MATCH_TH_BIRD;
	static int    MATCH_TH_RESTART;

private:

	struct Impl;
	std::unique_ptr<Impl> impl;
};
