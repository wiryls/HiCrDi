/****************************************************************************
 *
 *  @file       km_mate.hpp
 *  @brief
 *
 *
 *  Date        Name        Description
 *  16/11/20    MYLS        Creation.
 *
 ***************************************************************************/
#pragma once

#include <memory>
#include "ibeat.hpp"
#include "datatype.hpp"

namespace hb
{
    class km_mate;
    class cv_board;
    class my_mind;
}

/****************************************************************************
 *  declaration
 ***************************************************************************/

/**
 *  \brief  TODO:
 */
class hb::km_mate : public IBeat<km_mate>
{

public:

    operator status_t () const override;

    void     clear    ()       override;

public:

    Self & operator >> (my_mind & other);

    Self & operator >> (cv_board & other);

public:

    void set(bool is_game_over);

    void set(const Object              & dino,
             const std::vector<Object> & birds,
             const std::vector<Object> & cactus);

public:

    km_mate();

    km_mate(const km_mate & rhs);

    km_mate(km_mate && rhs);

    ~km_mate();

private:

    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;
};
