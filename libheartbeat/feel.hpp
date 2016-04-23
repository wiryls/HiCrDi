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

namespace cv {
	class Mat;

	template<typename _Tp> class Rect_;
	typedef Rect_<int> Rect2i;
	typedef Rect2i Rect;
}

namespace hb {
	class Feel;
}

/**
 *	\brief	TODO:
 */
class hb::Feel {

public:

	cv::Mat const &       oh_i_feel(cv::Mat const & view);

	cv::Rect              me()     const;

	std::vector<cv::Rect> cactus() const;

	std::vector<cv::Rect> birds()  const;
	
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

private:

	struct Impl;
	std::unique_ptr<Impl> impl;
};
