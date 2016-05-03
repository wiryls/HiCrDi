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
#include "cogn.hpp"

/****************************************************************************
 *	private Impl
 ***************************************************************************/

typedef struct hb::Cogn::Impl
{
public:

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
		Sense const * src = nullptr, 
		float		  delta_time = 0.0f,
		float		  reliable = 0.0f
	);

	static std::vector<std::pair<size_t, size_t>> kuhn_munkres
	(
		std::vector<float> const & wgt, 
		size_t                     n, 
		size_t                     m
	);

	static void merge
	(
		std::vector<Object> &      dst, 
		std::vector<Sense> const & src,
		float                      delta_time, 
		float					   threshold
	);

} Private;


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

void hb::Cogn::Impl::update(Object & dst, Sense const * src, float delta_time, float reliable)
{
	if (src == nullptr) {
		dst.frame >>= 1;
		dst.x_mid += dst.spd_x * delta_time;
		dst.y_mid += dst.spd_y * delta_time;
		dst.score *= 0.6f;
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


/**
 *	/brief	kuhn_munkres
 *			wgt, sample:
 *			-- O0 O1 O2 On
 *			T0
 *			T1
 *			T2
 *			T3
 *			Tm
 */
std::vector<std::pair<size_t, size_t>> hb::Cogn::Impl::kuhn_munkres(std::vector<float> const & wgt, size_t n, size_t m)
{
	std::vector<std::pair<size_t, size_t>> rv;
	if (wgt.size() < n * m)
		return rv;

	std::vector<float> w_nm(wgt);
	bool need_swap = n > m;
	if (need_swap) {
		n ^= m ^= n ^= m;
		for (size_t i = 0U; i < n; i++)
			for (size_t j = 0U; j < m; j++)
				w_nm[i * m + j] = wgt[i + j * n];
	}

	static float const MAX = 1.0f;
	static size_t const NOT_MATCH = ~size_t(0);
	std::vector<size_t> dst(m, NOT_MATCH);
	{	/* kuhn munkres */

		std::vector<float>  l_x(n);
		std::vector<float>  l_y(m);
		for (size_t i = 0U; i < n; i++)
			l_x[i] = *std::max_element(w_nm.begin() + i * m, w_nm.begin() + (i + 1) * m);

		for (size_t k = 0U; k < n; k++) {

			std::vector<float> slack(m, MAX);

			while (true) {
				std::vector<bool> vst_x(n);
				std::vector<bool> vst_y(m);

				std::function<bool(size_t i)> helper = [&](size_t i)
				{
					vst_x[i] = true;
					for (size_t j = 0U; j < m; j++) {
						if (!vst_y[j]) {
							auto delta = l_x[i] + l_y[j] - w_nm[i * m + j];
							if (std::abs(delta) < 1e-5f) {
								vst_y[j] = true;
								if (dst[j] == NOT_MATCH || helper(dst[j])) {
									dst[j] = i;
									return true;
								}
							} else if (slack[j] > delta) {
								slack[j] = delta;
							}
						}
					}
					return false;
				};

				if (helper(k))
					break;

				auto min_weight = MAX;
				for (size_t j = 0; j < m; j++)
					if (!vst_y[j])
						min_weight = std::min(min_weight, slack[j]);

				for (size_t i = 0; i < n; i++)
					if (vst_x[i])
						l_x[i] -= min_weight;

				for (size_t j = 0; j < m; j++)
					if (vst_y[j])
						l_y[j] += min_weight;
					else
						slack[j] -= min_weight;
			}
		}
	}

	{	/* take out invalid objects */
		rv.reserve(m);
		for (auto j = 0U; j < m; j++) {
			auto & i = dst[j];
			if (i != NOT_MATCH && w_nm[i * m + j] > 0.0f) {
				if (need_swap)
					rv.push_back(std::make_pair(i, j));
				else
					rv.push_back(std::make_pair(j, i));
			}
		}
	}

	return rv;
}

void hb::Cogn::Impl::merge
(
	std::vector<Object>      & dst, 
	std::vector<Sense> const & src, 
	float                      delta_time,
	float                      threshold
)
{
	size_t n = src.size();
	size_t m = dst.size();

	std::vector<float> weight(n * m);
	for (size_t i = 0U; i < n; i++)
		for (size_t j = 0U; j < m; j++)
			weight[i * m + j] = score(src[i], dst[j], delta_time, threshold);

	auto match = kuhn_munkres(weight, n, m);

	std::vector<bool> src_vst(n); // i
	std::vector<bool> dst_vst(m); // j

	/* merge src[i] & dst[j] */
	for (auto & pair : match) {
		auto i = pair.second;
		auto j = pair.first;
		update(dst[j], &src[i], delta_time, weight[i * m + j]);
		src_vst[i] = true;
		dst_vst[j] = true;
	}

	/* update not matched */
	{
		size_t cnt = m;
		for (size_t j = 0; j < cnt; j++)
			if (!dst_vst[j]) {
				update(dst[j], nullptr, delta_time);
				if (dst[j].score < 0.3f * threshold) {
					if (j != --cnt) {
						std::swap(dst    [j], dst    [cnt]);
						std::swap(dst_vst[j], dst_vst[cnt]);
						j--;
					}
				}
			}

		cnt = m - cnt;
		while (cnt-- > 0U)
			dst.pop_back();
	}

	// TODO:
	static size_t id = 0;

	/* add new object */
	for (size_t i = 0; i < n; i++)
		if (!src_vst[i]) {
			auto & s = src[i];
			dst.push_back
			({
				float(s.x + (s.w >> 1)), 
				float(s.y + (s.h >> 1)), 
				float(s.w), 
				float(s.h),
				0.0f, 
				0.0f, 
				1, 
				0.8f * threshold,
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
		Private::merge(impl->dinosaur, sense_list, delta_time, 0.5f);
		break;
	case hb::ObjectType::CACTUS:
		Private::merge(impl->cactus, sense_list, delta_time, 0.5f);
		break;
	case hb::ObjectType::BIRD:
		Private::merge(impl->bird, sense_list, delta_time, 0.5f);
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

	// cm::Log::Msg("my:", impl->dinosaur.size());

	auto & fst = impl->dinosaur.front();
	auto & _0 = impl->bird;
	auto & _1 = impl->cactus;

	std::vector<Info> rv;
	rv.reserve(_0.size() + _1.size());
	Private::push_back_info(rv, _0, 0.1f, fst); // TODO: weight
	Private::push_back_info(rv, _1, 0.1f, fst);
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
