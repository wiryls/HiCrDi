/****************************************************************************
 *
 *  @file       cv_eye.hpp
 *  @brief
 *
 *
 *  Date        Name        Description
 *  16/11/20    MYLS        Creation.
 *
 ***************************************************************************/
#pragma once

#include <cstdint>
#include <memory>
#include "ibeat.hpp"

namespace hb
{
    class cv_board;
    class km_mate;

    class cv_eye;
}

/****************************************************************************
 *  declaration
 ***************************************************************************/

/**
 *  \brief  TODO:
 */
class hb::cv_eye : public IBeat<cv_eye>
{

public:

    operator status_t () const override;

    void     clear    ()       override;

public:

    const Self & operator >> (km_mate & other) const;

    const Self & operator >> (cv_board & other) const;

    bool is_dino_found() const;

    bool is_dino_dead() const;

    void set(uint8_t const * bgra32, size_t wid, size_t hgt);

public:

    cv_eye();

    cv_eye(const cv_eye & rhs);

    cv_eye(cv_eye && rhs);

    ~cv_eye();

private:

    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;
};
