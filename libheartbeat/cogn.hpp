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

private:

	struct Impl;
	std::unique_ptr<Impl> impl;
};

/****************************************************************************
 *	nested
 ***************************************************************************/
