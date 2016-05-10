/*********************************************************************
 *	@file		CvFeel.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <vector>

#include "../utility/types.hpp"
#include "../core/ifeel.hpp"
#include "datatype.hpp"

namespace hb 
{
	class CvFeel;
}

class hb::CvFeel : virtual public IFeel
{

public:

	virtual IFeel & operator << (Input const &) override;

	virtual IFeel & operator >> (Output &) override;

	virtual operator bool() const override;

	virtual void clear() override;

public:

	CvFeel();
	 
public:

	RangedValue<double> CONTRAST_ALPHA			= {0.0, 3.0, 1.96};
	RangedValue<int>	BINARIZATION_THRESHOLD	= {  0, 255, 249 };
	RangedValue<int>	MATCH_TH_DINOSAUR		= {  0, 255, 205 };
	RangedValue<int>	MATCH_TH_CACTUS			= {  0, 255, 144 };
	RangedValue<int>	MATCH_TH_BIRD			= {  0, 255, 196 };
	RangedValue<int>	MATCH_TH_RESTART		= {  0, 255, 196 };

private:

	void load_tmpl(bool & is_ok, cv::Mat & dst, char const path[], char const err_msg[]) const;

	void see(cv::Mat const & src);
	
	bool feel_existence(cv::Mat const & tmpl, int threshold) const;
	
	std::vector<Output::Obj> feel_one(cv::Mat const & tmpl, int threshold) const;

	std::vector<Output::Obj> feel_several(cv::Mat const & tmpl, int threshold) const;

	void merge_adjacent(std::vector<Output::Obj> & rects, int delta_width, int delta_height) const;

private:

	static const int MATCH_METHOD;

private:

	bool is_ready;
	bool is_valid;
	float delta_time;
	cv::Mat view;
	cv::Mat dinosaur_tmpl;
	cv::Mat dinosaur2_tmpl;
	cv::Mat bird_tmpl;
	cv::Mat cactus_tmpl;
	cv::Mat restart_tmpl;

};
