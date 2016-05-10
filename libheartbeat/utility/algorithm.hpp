/*********************************************************************
 *	@file		datatype.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	05/04/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <vector>
#include <cfloat>
#include <functional>
#include <algorithm>


namespace hb { 	namespace match 
{
	struct result_t;

	struct matched_t;

	template<class T, class U> result_t kuhn_munkres
	(
		std::vector<T> const & fst_array,
		std::vector<U> const & snd_array,
		std::function<float(T const &, U const &)> const & evaluate
	);
}}

/****************************************************************************
 *	kuhn_munkres
 ***************************************************************************/

struct hb::match::matched_t
{
	size_t fst;
	size_t snd;
	float  wgt;
};

struct hb::match::result_t
{
	std::vector<size_t> not_matched_of_fst;
	std::vector<size_t> not_matched_of_snd;
	std::vector<matched_t> matched;
};

template<class T, class U> inline hb::match::result_t hb::match::
kuhn_munkres
(
	std::vector<T> const & fst_array,
	std::vector<U> const & snd_array,
	std::function<float(T const &, U const &)> const & evaluate
)
{
	size_t n = fst_array.size();
	size_t m = snd_array.size();
	result_t rv;

	{	/* use kuhn_munkres to match */
		/* get weight table */
		std::vector<float> weight(n * m);
		for (size_t i = 0U; i < n; i++)
			for (size_t j = 0U; j < m; j++)
				weight[i * m + j] = evaluate(fst_array[i], snd_array[j]);

		/*
		 *	wgt, sample:
		 *	-- F0 F1 F2 Fn
		 *	S0
		 *	S1
		 *	S2
		 *	S3
		 *	Sm
		 */
		struct Helper
		{
			static std::vector<matched_t> 
				km(std::vector<float> const & wgt, size_t n, size_t m)
			{
				std::vector<matched_t> rv;
				if (n == 0U || m == 0U || wgt.size() < n * m)
					return rv;

				std::vector<float> w_nm(wgt);
				bool need_swap = n > m;
				if (need_swap) {
					n ^= m ^= n ^= m;
					for (size_t i = 0U; i < n; i++)
						for (size_t j = 0U; j < m; j++)
							w_nm[i * m + j] = wgt[i + j * n];
				}

				static float const MAX = FLT_MAX;
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
								rv.push_back({j, i, w_nm[i * m + j]});
							else
								rv.push_back({i, j, w_nm[i * m + j]});
						}
					}
				}

				return rv;
			}
		};
		rv.matched = Helper::km(weight, n, m);
	}

	{	/* find not matched */
		std::vector<bool> fst_matched(n);
		std::vector<bool> snd_matched(m);
		for (auto & mth : rv.matched) {
			fst_matched[mth.fst] = true;
			snd_matched[mth.snd] = true;
		}

		rv.not_matched_of_fst.reserve(n - rv.matched.size());
		for (size_t i = 0; i < n; i++)
			if (!fst_matched[i]) 
				rv.not_matched_of_fst.push_back(i);

		rv.not_matched_of_snd.reserve(m - rv.matched.size());
		for (size_t j = 0; j < m; j++)
			if (!snd_matched[j]) 
				rv.not_matched_of_snd.push_back(j);
	}

	return rv;
}