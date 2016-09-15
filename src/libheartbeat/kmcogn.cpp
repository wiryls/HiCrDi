/****************************************************************************
 *	@file		KmCogn.cpp
 *	@brief		Implement of KmCogn.hpp
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 ***************************************************************************/

#include <algorithm>
#include <functional>

#include "utility/log.hpp"
#include "utility/algorithm.hpp"
#include "kmcogn.hpp"

/****************************************************************************
 *	private
 ***************************************************************************/

void hb::KmCogn::make_relative(std::vector<Sense::Obj> & src, Obj const & origin)
{
	for (auto & obj : src) {	// TODO: coord ?
		obj.x -= static_cast<int>(origin.x_mid + origin.wid * 0.5f);
		obj.y -= static_cast<int>(origin.y_mid + origin.hgt * 0.5f);
		obj.w += static_cast<int>(origin.wid);
		obj.h += static_cast<int>(origin.hgt);
	}
}

void hb::KmCogn::make_info
(
	std::vector<Info::Obj> & dst, 
	std::vector<Obj> const & src, 
	float                    threshold
)
{
	for (auto & obj : src) {
		if (obj.score > threshold) {
			dst.push_back({
				obj.x_mid,
				obj.y_mid,
				obj.wid,
				obj.hgt,
				obj.spd_x,
				obj.spd_y,
				obj.id
			});
		}
	}
}

float hb::KmCogn::score(Sense::Obj const & lhs, Obj const & rhs, float delta_time, float threshold)
{
	float nxt_mid_x = rhs.x_mid + rhs.spd_x * delta_time;
	float nxt_mid_y = rhs.y_mid + rhs.spd_y * delta_time;
	float sub_mid_x = static_cast<float>(lhs.x + (lhs.w >> 1));
	float sub_mid_y = static_cast<float>(lhs.y + (lhs.h >> 1));

	float beingness    = 1.0f - 1.0f / (rhs.frame * rhs.frame);
	float shape        = 1.0f / (1.0f + 0.04f * (std::abs(lhs.w - rhs.wid) + std::abs(lhs.h - rhs.hgt)));
	float displacement = 1.0f / (1.0f + 0.0001f * ((nxt_mid_x - sub_mid_x) * (nxt_mid_x - sub_mid_x) + (nxt_mid_y - sub_mid_y) * (nxt_mid_y - sub_mid_y)));

	float score 
		= 0.1f * beingness
		+ 0.25f * shape
		+ 0.65f * displacement
		;

	return score < threshold ? 0.0f : score;
}

void hb::KmCogn::update(Obj & dst, Sense::Obj const * src, float delta_time, float reliable, float score_decay)
{
	if (src == nullptr) {
		dst.frame >>= 1;
		dst.x_mid += dst.spd_x * delta_time;
		dst.y_mid += dst.spd_y * delta_time;
		dst.score *= score_decay;
	} else {
		Sense::Obj const & sub = *src;
		float k0 = dst.score;
		float k1 = sub.s * 20.0f * reliable;
		float k = k0 + k1;
		float nxt_mid_x = dst.x_mid + dst.spd_x * delta_time;
		float nxt_mid_y = dst.y_mid + dst.spd_y * delta_time;
		float sub_mid_x = static_cast<float>(sub.x + (sub.w >> 1));
		float sub_mid_y = static_cast<float>(sub.y + (sub.h >> 1));
		k0 /= k;
		k1 /= k;
		{
			dst.spd_x = k0 * dst.spd_x + k1 * (sub_mid_x - dst.x_mid) / delta_time;
			dst.spd_y = k0 * dst.spd_y + k1 * (sub_mid_y - dst.y_mid) / delta_time;
			dst.hgt   = k0 * dst.hgt   + k1 * sub.h;
			dst.wid   = k0 * dst.wid   + k1 * sub.w;
			dst.x_mid = k0 * nxt_mid_x + k1 * sub_mid_x;
			dst.y_mid = k0 * nxt_mid_y + k1 * sub_mid_y;
			dst.frame++;
			dst.score = reliable;
		}
	}
}

void hb::KmCogn::merge
(
	std::vector<Obj> &              dst, 
	std::vector<Sense::Obj> const & src, 
	float                           delta_time,
	KmCogn &				        wrapper
)
{
	std::function<float(Sense::Obj const &, Obj const &)> evaluate = std::bind
	(
		score, 
		std::placeholders::_1, 
		std::placeholders::_2,
		delta_time, 
		wrapper.SCORE_THRESHOLD
	);

	auto result = match::kuhn_munkres(src, dst, evaluate);

	/* update matched */
	for (auto & mth : result.matched)
		update(dst[mth.snd], &src[mth.fst], delta_time, mth.wgt, wrapper.SCORE_DECAY);

	/* update not matched */
	for (auto idx : result.not_matched_of_snd)
		update(dst[idx], nullptr, delta_time, 0.0f, wrapper.SCORE_DECAY);

	/* remove low scored */
	auto cnt = dst.size();
	for (size_t i = 0U; i < cnt; i++)
		if (dst[i].score < wrapper.MINIMUM_THRESHOLD)
			if (i != --cnt)
				std::swap(dst[i], dst[cnt]);
	cnt = dst.size() - cnt;
	while (cnt-- > 0U)
		dst.pop_back();

	/* add new object */

	for (auto idx : result.not_matched_of_fst) {
		auto & s = src[idx];
		dst.push_back
		({
			float(s.x + (s.w >> 1)), 
			float(s.y + (s.h >> 1)), 
			float(s.w), 
			float(s.h),
			0.0f, 
			0.0f, 
			1, 
			wrapper.INITIAL_THRESHOLD,
			wrapper.id++
		});
	}
}


/****************************************************************************
 *	public
 ***************************************************************************/

hb::ICogn & hb::KmCogn::operator<<(Input const & in)
{
	merge(dinosaurs, in.dinosaurs  , in.delta_time, *this);
	if (dinosaurs.empty())
		return *this;

	auto & fst = dinosaurs.front();

	auto relative_cactus = in.cactus;
	auto relative_birds  = in.birds;
	make_relative(relative_cactus, fst);
	make_relative(relative_birds , fst);
	merge(   cactus, relative_cactus, in.delta_time, *this);
	merge(    birds, relative_birds , in.delta_time, *this);

	is_dead    = in.is_dead;
	delta_time = in.delta_time;

	return *this;
}

hb::ICogn & hb::KmCogn::operator>>(Output & out)
{
	if (dinosaurs.empty()) {
		cactus.clear();
		birds.clear();
		return *this;
	}

	auto & _0 = birds;
	auto & _1 = cactus;
	auto & rv = out.objs;
	
	rv.clear();
	rv.reserve(_0.size() + _1.size());
	
	make_info(rv, _0, 0.0f); // TODO: 0.0f ?
	make_info(rv, _1, 0.0f);

	auto & fst = dinosaurs.front();
	out.x0 = fst.x_mid;
	out.y0 = fst.y_mid;

	out.delta_time = delta_time;
	out.is_dead = is_dead;

	return *this;
}

hb::KmCogn::operator bool() const
{
	return true;
}

void hb::KmCogn::clear()
{
	id = 0;
	is_dead = false;
	delta_time = 0.f;
	dinosaurs.clear();
	cactus.clear();
	birds.clear();
}
