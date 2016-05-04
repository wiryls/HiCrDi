/*********************************************************************
 *	@file		Cogn.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	24/04/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <memory>
#include <vector>

#include "datatype.hpp"

namespace hb {
	enum struct ObjectType;
	struct Sense;
	struct Info;
	class Cogn;
}

/**
 *	\brief	TODO:
 */
class hb::Cogn
{

public:

	void i_recognize(ObjectType type, std::vector<Sense> const & sense_list, float delta_time);

	std::vector<Info> then_i_know() const;

	bool i_am_at(int & x0, int & y0) const;

public:

	/**
	 *	\brief constructor
	 */
	Cogn();

	/**
	 *	\brief destructor
	 */
	~Cogn();

public:

	RangedValue<float> INITIAL_THRESHOLD = RangedValue<float>(0.f, 1.f, 0.4f);
	RangedValue<float> MINIMUM_THRESHOLD = RangedValue<float>(0.f, 1.f, 0.15f);
	RangedValue<float> SCORE_THRESHOLD   = RangedValue<float>(0.f, 1.f, 0.5f);
	RangedValue<float> SCORE_DECAY		 = RangedValue<float>(0.f, 1.f, 0.6f);

private:

	struct Impl;
	std::unique_ptr<Impl> impl;
};

