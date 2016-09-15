/****************************************************************************
 *	@file		Heart.cpp
 *	@brief		Implement of Heart.hpp
 *
 *
 *	Date        Name        Description
 *	21/04/16	MYLS		Creation.
 *
 ***************************************************************************/

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "utility/timer.hpp"
#include "cvfeel.hpp"
#include "kmcogn.hpp"
#include "qlmind.hpp"
#include "Heart.hpp"

/****************************************************************************
 *	private Impl
 ***************************************************************************/

struct hb::Heart::Impl
{
	bool is_runnning;
	float last_time;
	cm::timer timer;
	
	CvFeel feel;
	KmCogn cogn;
	QlMind mind;

	View view;
	Sense sense;
	Info info;
	Action action;

	void clear_most();
};

/****************************************************************************
 *	private
 ***************************************************************************/

void hb::Heart::Impl::clear_most()
{
	is_runnning = false;
	last_time = 0.f;

	feel.clear();
	cogn.clear();

	std::swap(view,   decltype(view  )());
	std::swap(sense,  decltype(sense )());
	std::swap(info,   decltype(info  )());
	std::swap(action, decltype(action)());
}


/****************************************************************************
 *	public
 ***************************************************************************/

void hb::Heart::start()
{
	impl->timer.start();
	impl->is_runnning = true;
}

void hb::Heart::pause()
{
	impl->timer.pause();
	impl->clear_most();
}

void hb::Heart::resume()
{
	impl->timer.resume();
	impl->last_time = impl->timer.get_elapsed_time() * 1e-3f;
	impl->is_runnning = true;
}

void hb::Heart::stop()
{
	impl->timer.stop();
	impl->clear_most();
	impl->mind.clear();
}

bool hb::Heart::view(uint8_t const bgra32[], size_t wid, size_t hgt)
{
	auto & i = *impl;

	/* get bgr img */
	auto src = cv::Mat(hgt, wid, CV_8UC4, (void *)bgra32);
	cv::cvtColor(src, src, cv::COLOR_BGRA2BGR);

	if (i.is_runnning) 
	{
		/* get delta time */
		float curr_enttime = i.timer.get_elapsed_time() * 1e-3f;
		float delta_time   = curr_enttime - i.last_time;
		i.last_time        = curr_enttime;
		if (delta_time > 1.0f)
			delta_time = 1.0f;

		/* init input */
		i.view.delta_time = delta_time;
		i.view.img = src;

		/* process */
		i.feel << i.view  >> i.sense;
		if (i.sense.dinosaurs.empty())
			return false;

		i.cogn << i.sense >> i.info;
		i.mind << i.info  >> i.action;

		return true;
	} 
	else 
	{	/* process */
		i.view.img = src;
		i.feel << i.view  >> i.sense;
	
		/* is ready */
		return (!i.sense.dinosaurs.empty());
	}
}

size_t hb::Heart::plan() const
{
	auto & i = *impl;
	if (!i.is_runnning)
		return size_t();
	else
		return i.action.choice;
}

bool hb::Heart::dead() const
{
	auto & i = *impl;
	if (!i.is_runnning)
		return false;
	else
		return i.sense.is_dead;
}

bool hb::Heart::ready() const
{
	auto & i = *impl;
    return i.feel && i.cogn && i.mind;
}

void hb::Heart::know(uint8_t bgra32[], size_t wid, size_t hgt) const
{
	auto & i = *impl;
	if (i.sense.debug_view.empty() || wid * hgt != i.sense.debug_view.total())
		return;
	
	auto dst = i.sense.debug_view.clone();
	if (i.is_runnning) {
		char buffer[11];
		{
			for (auto & obj : i.sense.dinosaurs) {
				cv::Rect r = hb::toRect(obj);
				std::sprintf(buffer, "%.2f", obj.s);
				cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
				cv::rectangle(dst, r, cv::Scalar::all(128));
			}
			for (auto & obj : i.sense.cactus) {
				cv::Rect r = hb::toRect(obj);
				std::sprintf(buffer, "%.2f", obj.s);
				cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
				cv::rectangle(dst, r, cv::Scalar::all(255));
			}
			for (auto & obj : i.sense.birds) {
				cv::Rect r = hb::toRect(obj);
				std::sprintf(buffer, "%.2f", obj.s);
				cv::putText(dst, buffer, r.br(), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(128));
				cv::rectangle(dst, r, cv::Scalar::all(196));
			}
		}

		{
			for (auto & obj : i.info.objs) {
				cv::Rect r = hb::toRect(obj, i.info.x0, i.info.y0);
				std::sprintf(buffer, "%u", obj.id);
				cv::putText(dst, buffer, r.tl(), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar::all(255));
				std::sprintf(buffer, "%.2f", obj.speed_x);
				cv::putText(dst, buffer, r.tl() + cv::Point(0, 16), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar::all(225));
				cv::rectangle(dst, r, cv::Scalar::all(196), 3);
			}
		}

		{
			cv::Point p(4, 16);
			std::sprintf(buffer, "act: %u", i.action.choice);
			cv::putText(dst, buffer, p, cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(255));
			p = cv::Point(4, 32);
			std::sprintf(buffer, "cnt: %u", i.action.cnt);
			cv::putText(dst, buffer, p, cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(255));
			p = cv::Point(4, 48);
			for (auto & s : i.action.states) {
				std::sprintf(buffer, "%u", s);
				cv::putText(dst, buffer, p, cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar::all(255));
				p += cv::Point(16, 0);
			}
		}

		if (i.sense.is_dead)
			cv::bitwise_not(dst, dst);
	}

	/* note: this way of copy is not safe */
	int idx = 0;
	for (auto iter = dst.datastart; iter != dst.dataend; iter++) {
		bgra32[idx++] = *iter;
		bgra32[idx++] = *iter;
		bgra32[idx++] = *iter;
		bgra32[idx++] = 0xFFU;
	}
}


hb::Heart::Heart() : impl(std::make_unique<Impl>()) { stop(); }

hb::Heart::~Heart() {}
