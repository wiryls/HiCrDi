/*********************************************************************
 *	@file		datatype.hpp
 *	@brief
 *
 *
 *	Date        Name        Description
 *	09/05/16	MYLS		Creation.
 *
 *********************************************************************/
#pragma once

#include <vector>
#include <opencv2/core/mat.hpp>

 /****************************************************************************
  *	namespace
  ***************************************************************************/

namespace cv
{
	template<typename _Tp> class Rect_;
	typedef Rect_<int> Rect;
	class Mat;
}

namespace hb
{
	enum struct ObjectType
	{
		DINOSAUR,
		CACTUS,
		BIRD,
	};

	/**
	 *	\brief	TODO:
	 */
	struct View
	{
		cv::Mat img;
		float delta_time;
	};


	/**
	 *	\brief	TODO:
	 */
	struct Sense
	{
		struct Obj
		{
			int x;
			int y;
			int w;
			int h;
			float s;
		};

		bool is_dead;
		float delta_time;
		std::vector<Obj> dinosaurs;
		std::vector<Obj> birds;
		std::vector<Obj> cactus;
		cv::Mat debug_view;
	};

	/**
	 *	\brief	TODO:
	 */
	struct Info
	{
		struct Obj
		{
			float delta_x;
			float delta_y;
			float relative_width;
			float relative_height;
			float speed_x;
			float speed_y;
			size_t id;
		};

		std::vector<Obj> objs;
		float x0;
		float y0;
		bool is_dead;
		float delta_time;
	};

	/**
	 *	\brief	TODO:
	 */
	struct Action
	{
		enum {
			IDEL,
			JUMP,
			DOWN
		} choice;

		std::vector<size_t> states;
		size_t cnt;
	};

}

namespace hb
{
	extern cv::Rect toRect(Sense::Obj const & src);
	extern cv::Rect toRect( Info::Obj const & src, float x0, float y0);
}

/****************************************************************************
 *
 ***************************************************************************/
