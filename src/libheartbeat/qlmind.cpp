/****************************************************************************
 *	@file		QlMind.cpp
 *	@brief		Implement of QlMind.hpp
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 ***************************************************************************/

#include <algorithm>

#include "utility\randnum.hpp"
#include "qlmind.hpp"


/****************************************************************************
 *	private
 ***************************************************************************/

void hb::QlMind::convert(Input::Obj const & src, State & dst)
{
	auto fdx = [](float x, float w, float h) 
	{
		if (x > w * 0.5f)
			return static_cast<size_t>(h);
		else if (x < 0.0f)
			return static_cast<size_t>(0U);
		else
			return static_cast<size_t>(h * (1.0f - 4.0f * (x / w - 0.5f) * (x / w - 0.5f)));
	};

	auto fdy = [](float x, float w, float h) 
	{
		if (x > w)
			return static_cast<size_t>(h * 2.f);
		else if (x < -w)
			return static_cast<size_t>(0.f);
		else
			return static_cast<size_t>(h + h * std::powf(x / w, 0.6f));
	};

	auto fs = [](float x, float l, float r, float h) 
	{
		if (x < l)
			return static_cast<size_t>(0);
		else if (x > r)
			return static_cast<size_t>(h);
		else
			return static_cast<size_t>((x - l) / (r - l) * h);
	};

	dst.dx  = fdx(src.delta_x, 1000.f, Limit::DELTA_X);
	dst.dy  = fdy(src.delta_y,  200.f, Limit::DELTA_Y / 2.f);
	//dst.wid = static_cast<size_t>((src.relative_width  - 20.f) / 30.f);
	//dst.hgt = static_cast<size_t>((src.relative_height - 20.f) / 30.f);
	dst.sx  = fs(-src.speed_x,  60.0f,  900.0f, Limit::SPEED_X);
	dst.sy  = fs(-src.speed_y, -350.0f, 350.0f, Limit::SPEED_Y);
}

/****************************************************************************
 *	public
 ***************************************************************************/

hb::IMind & hb::QlMind::operator<<(Input const & in)
{
	{	/* clear old states */
		std::swap(state_fst, decltype(state_fst)());
		//std::swap(state_snd, decltype(state_snd)());
	}
	
	auto jump_over = false;
	auto just_dead = false;
	{	/* gather useful info */
		decltype(in.objs) infos;
		for (auto obj : in.objs)
			if (obj.delta_x > 0)
				infos.push_back(obj);
			else if (lastest_id < obj.id) {
				lastest_id = obj.id;
				jump_over = true;
			}

		/* find top 2 info */
		auto cnt = infos.size();
		if (cnt > 1U) {
			auto cmp = [] (Input::Obj const & lhs, Input::Obj const & rhs) {
				auto lhs_dis_sqr = lhs.delta_x * lhs.delta_x + lhs.delta_y * lhs.delta_y;
				auto rhs_dis_sqr = rhs.delta_x * rhs.delta_x + rhs.delta_y * rhs.delta_y;
				return lhs_dis_sqr < rhs_dis_sqr;
			};
			std::nth_element(infos.begin(), infos.begin() + 1U, infos.end(), cmp);
		}

		/* convert 2 state */
		auto iter = infos.begin();
		if (cnt > 0U) 
			convert(*iter++, state_fst);
		//if (cnt > 1U)
		//	convert(*iter++, state_snd);
	}

	{	/* update */
		if (is_dead && !in.is_dead) {
			just_dead = true;
			lastest_id = 0U;
		}

		is_dead = in.is_dead;
		total_time += in.delta_time;
		total_frames++;
	}

	{	/* learn */
		static const float learning_rate = 0.1f;
		static const float discount_rate = 0.8f;
		float reinforcement = 0.01f;
		if (jump_over)
			reinforcement += 30.0f;
		if (just_dead)
			reinforcement -= 100.0f;

		auto & last_vals = qtable
		(
			last_state.dx,
			last_state.dy,
			//last_state.wid,
			//last_state.hgt,
			last_state.sx,
			last_state.sy
		);

		auto & this_vals = qtable
		(
			last_state.dx,
			last_state.dy,
			//last_state.wid,
			//last_state.hgt,
			last_state.sx,
			last_state.sy
		);

		auto max_val = *std::max_element(this_vals.begin(), this_vals.end());
		
		auto & old_val = last_vals[last_action];
		old_val = (1.f - learning_rate) * old_val 
				+        learning_rate  * (reinforcement + discount_rate * max_val);
	}

	return *this;
}

hb::IMind & hb::QlMind::operator>>(Output & out)
{
	static const float exploitation_rate = 1.f;

	last_state  = state_fst;

	out.states.clear();
	out.states.push_back(last_state.dx);
	out.states.push_back(last_state.dy);
	//out.states.push_back(last_state.wid);
	//out.states.push_back(last_state.hgt);
	out.states.push_back(last_state.sx);
	out.states.push_back(last_state.sy);
	out.cnt = lastest_id;

	auto & qvals = qtable
	(
		last_state.dx,
		last_state.dy,
		//last_state.wid,
		//last_state.hgt,
		last_state.sx,
		last_state.sy
	);

    float sum = 0.f;
    std::array<float, 3U> value;
  
	for (size_t i = 0; i < value.size(); i++) {
		value[i] = std::expf(exploitation_rate * qvals[i]);
		sum += value[i];
	}

	if (sum != 0.f) {
		for (auto & v : value)
			v /= sum;
	} else {
		 last_action = std::distance(qvals.begin(), std::max_element(qvals.begin(), qvals.end()));
		 if (last_action == Action::JUMP)
			out.choice = Action::JUMP;
		 else if (last_action == Action::DOWN)
			 out.choice = Action::DOWN;
		 else
			 out.choice = Action::IDEL;
		 return *this;
	}

    size_t action = 0;
    float cum_prob = 0.0;
    float random_num = cm::rand(0.0f, 0.1f);
    while (random_num > cum_prob && action < value.size())
    {
		cum_prob += value[action];
		action++;
    }
    last_action = action - 1U;

	if (last_action == Action::JUMP)
		out.choice = Action::JUMP;
	else if (last_action == Action::DOWN)
		out.choice = Action::DOWN;
	else
		out.choice = Action::IDEL;
	return *this;
}

hb::QlMind::operator bool() const
{
	return true;
}

void hb::QlMind::clear()
{
	is_dead = false;
	lastest_id = 0U;
	total_time = 0.f;
	total_frames = 0U;
	qtable.clear();
	
	std::swap(state_fst , decltype(state_fst)());
	
	last_action = 0U;
	std::swap(last_state, decltype(last_state)());
}
