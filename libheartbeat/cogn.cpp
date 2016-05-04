/****************************************************************************
 *	@file		Cogn.cpp
 *	@brief		Implement of Cogn.hpp
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 ***************************************************************************/

#include <algorithm>
#include <functional>

#include "log.hpp"
#include "datatype.hpp"
#include "algorithm.hpp"
#include "cogn.hpp"

/****************************************************************************
 *	private Impl
 ***************************************************************************/

typedef struct hb::Cogn::Impl
{
public:

	struct Object;

	std::vector<Object> dinosaur;
	std::vector<Object> cactus;
	std::vector<Object> bird;

public:

	static void push_back_info
	(
		std::vector<Info> &         dst, 
		std::vector<Object> const & src, 
		float                       threshold, 
		Object const &              ori
	);

	static float score
	(
		Sense const &  lhs, 
		Object const & rhs, 
		float          delta_time,
		float          threshold
	);

	static void update
	(
		Object &      dst, 
		Sense const * src, 
		float		  delta_time,
		float		  reliable,
		float		  score_decay
	);

	static void merge
	(
		std::vector<Object> &      dst, 
		std::vector<Sense> const & src,
		float                      delta_time, 
		Cogn const &			   wrapper
	);

} Private;

struct Private::Object
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


/****************************************************************************
 *	private method
 ***************************************************************************/

void hb::Cogn::Impl::push_back_info
(
	std::vector<Info> &         dst, 
	std::vector<Object> const & src, 
	float                       threshold, 
	Object const &              ori
)
{
	for (auto & obj : src) {
		if (obj.score > threshold) {
			dst.push_back({	// TODO: coord ?
				obj.x_mid - ori.x_mid,
				obj.y_mid - ori.y_mid,
				obj.wid + ori.wid,
				obj.hgt + ori.hgt,
				obj.spd_x,
				obj.spd_y,
				obj.id
			});
		}
	}
}

float hb::Cogn::Impl::score(Sense const & lhs, Object const & rhs, float delta_time, float threshold)
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

void hb::Cogn::Impl::update(Object & dst, Sense const * src, float delta_time, float reliable, float score_decay)
{
	if (src == nullptr) {
		dst.frame >>= 1;
		dst.x_mid += dst.spd_x * delta_time;
		dst.y_mid += dst.spd_y * delta_time;
		dst.score *= score_decay;
	} else {
		Sense const & sub = *src;
		float k0 = dst.score;
		float k1 = sub.s * 2.0f * reliable;
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

void hb::Cogn::Impl::merge
(
	std::vector<Object>      & dst, 
	std::vector<Sense> const & src, 
	float                      delta_time,
	Cogn const &			   wrapper
)
{
	std::function<float(Sense const &, Object const &)> evaluate = std::bind
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

	// TODO: id?
	static size_t id = 0;
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
			id++
		});
	}

}


/****************************************************************************
 *	public method
 ***************************************************************************/

void hb::Cogn::i_recognize(ObjectType type, std::vector<Sense> const & sense_list, float delta_time)
{
	switch (type)
	{
	case hb::ObjectType::DINOSAUR:
		Private::merge(impl->dinosaur, sense_list, delta_time, *this);
		break;
	case hb::ObjectType::CACTUS:
		Private::merge(impl->cactus, sense_list, delta_time, *this);
		break;
	case hb::ObjectType::BIRD:
		Private::merge(impl->bird, sense_list, delta_time, *this);
		break;
	default:
		break;
	}
}

std::vector<hb::Info> hb::Cogn::then_i_know() const
{
	if (impl->dinosaur.empty()) {
		impl->cactus.clear();
		impl->bird.clear();
		return std::vector<Info>();
	}

	auto & fst = impl->dinosaur.front();
	auto & _0 = impl->bird;
	auto & _1 = impl->cactus;

	std::vector<Info> rv;
	rv.reserve(_0.size() + _1.size());
	Private::push_back_info(rv, _0, 0.0f, fst); // TODO: 0.1f ?
	Private::push_back_info(rv, _1, 0.0f, fst);
	return rv;
}

bool hb::Cogn::i_am_at(int & x0, int & y0) const
{
	if (impl->dinosaur.empty())
		return false;

	auto & fst = impl->dinosaur.front();
	x0 = static_cast<int>(fst.x_mid);
	y0 = static_cast<int>(fst.y_mid);
	return true;
}


/****************************************************************************
 *	public Constructor \ Destructor
 ***************************************************************************/

hb::Cogn::Cogn()
	: impl(std::make_unique<Impl>()) 
{}

hb::Cogn::~Cogn() {}
