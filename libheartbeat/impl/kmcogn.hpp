/*********************************************************************
 *	@file		KmCogn.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	24/04/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <vector>

#include "../core/icogn.hpp"
#include "../utility/types.hpp"
#include "datatype.hpp"

namespace hb 
{
	class KmCogn;
}

/**
 *	\brief	TODO:
 */
class hb::KmCogn : virtual public ICogn
{

public:

	virtual ICogn & operator<<(Input const &) override;

	virtual ICogn & operator>>(Output &) override;

	virtual operator bool() const override;

	virtual void clear() override;

public:

	RangedValue<float> INITIAL_THRESHOLD = { 0.f, 1.f, 0.4f  };
	RangedValue<float> MINIMUM_THRESHOLD = { 0.f, 1.f, 0.15f };
	RangedValue<float> SCORE_THRESHOLD   = { 0.f, 1.f, 0.5f  };
	RangedValue<float> SCORE_DECAY		 = { 0.f, 1.f, 0.6f  };
	
private:

	struct Obj
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

private:

	static void make_relative
	(
		std::vector<Sense::Obj> & src,
		Obj const &               ori
	);

	static void make_info
	(
		std::vector<Info::Obj> & dst, 
		std::vector<Obj> const & src, 
		float                    threshold
	);

	static float score
	(
		Sense::Obj const & lhs,
		Obj const &        rhs, 
		float              delta_time,
		float              threshold
	);

	static void update
	(
		Obj &              dst, 
		Sense::Obj const * src, 
		float		       delta_time,
		float		       reliable,
		float		       score_decay
	);

	static void merge
	(
		std::vector<Obj> &              dst, 
		std::vector<Sense::Obj> const & src,
		float                           delta_time, 
		KmCogn &				        wrapper
	);

private:

	size_t id;
	bool is_dead;
	float delta_time;
	std::vector<Obj> dinosaurs;
	std::vector<Obj> cactus;
	std::vector<Obj> birds;
};

