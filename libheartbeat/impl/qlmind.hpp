/*********************************************************************
 *	@file		QlMind.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <array>

#include "../core/imind.hpp"
#include "../utility/types.hpp"
#include "datatype.hpp"

namespace hb
{
	class QlMind;
}

/**
 *	\brief	TODO:
 */
class hb::QlMind : virtual public IMind
{

public:

	virtual IMind & operator<<(Input const &) override;
	
	virtual IMind & operator>>(Output &) override;
	
	virtual operator bool() const override;

	virtual void clear() override;

public:

private:

	enum Limit : size_t
	{
		DELTA_X = 8U,
		DELTA_Y = 6U,
		WIDTH   = 4U,
		HEIGHT  = 4U, 
		SPEED_X = 8U,
		SPEED_Y = 4U,
	};

	typedef SparseTable
	<
		std::array<float, 3U>, /* action has 3 types */
		
		Limit::DELTA_X,
		Limit::DELTA_Y,
		Limit::WIDTH,
		Limit::HEIGHT,
		Limit::SPEED_X,
		Limit::SPEED_Y
	> QTable;

	struct State
	{
		RangedValue<size_t> dx  = { 0, Limit::DELTA_X - 1U };
		RangedValue<size_t> dy  = { 0, Limit::DELTA_Y - 1U };
		RangedValue<size_t> wid = { 0, Limit::WIDTH   - 1U };
		RangedValue<size_t> hgt = { 0, Limit::HEIGHT  - 1U };
		RangedValue<size_t> sx  = { 0, Limit::SPEED_X - 1U };
		RangedValue<size_t> sy  = { 0, Limit::SPEED_Y - 1U };
	};

private:

	static void convert(Input::Obj const & src, State & dst);

private:

	bool is_dead;
	size_t lastest_id;
	float total_time;
	size_t total_frames;
	QTable qtable;
	State state_fst;
	//State state_snd;

	size_t last_action;
	State  last_state;
};
