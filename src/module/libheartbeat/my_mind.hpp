/****************************************************************************
 *
 *  @file       my_mind.hpp
 *  @brief
 *
 *
 *  Date        Name        Description
 *  16/11/20    MYLS        Creation.
 *
 ***************************************************************************/
#pragma once

#include <memory>
#include <vector>
#include "ibeat.hpp"

namespace hb
{
    class my_mind;
    class cv_board;
    struct Info;
    enum Action;
}

/****************************************************************************
 *  declaration
 ***************************************************************************/

/**
 *  \brief  TODO:
 */
class hb::my_mind : public IBeat<my_mind>
{

public:

    operator status_t () const override;

    void     clear    ()       override;

public:

    const Self & operator >> (cv_board & other) const;

    Action get() const;

    void set(int x0, int y0, Info && origin, std::vector<Info> && obstacle);

public:

    my_mind();

    my_mind(const my_mind & rhs);

    my_mind(my_mind && rhs);

    ~my_mind();

private:

    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;
};
