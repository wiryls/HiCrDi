/****************************************************************************
 *
 *  @file       cv_board.hpp
 *  @brief
 *
 *
 *  Date        Name        Description
 *  16/11/22    MYLS        Creation.
 *
 ***************************************************************************/
#pragma once

#include <memory>
#include "ibeat.hpp"

namespace hb
{
    class cv_board;
}

/****************************************************************************
 *  declaration
 ***************************************************************************/

/**
 *  \brief  TODO:
 */
class hb::cv_board : public IBeat<cv_board>
{

public:

    operator status_t () const override;

    void     clear    ()       override;

public:

    void set(uint8_t * img_8u1c, size_t hgt, size_t wid);

    // y
    // ^
    // |
    // |
    // |
    // 0----------> x

    void put(int x, int y, const char * text, double scale, int thickness, double color);

    void put(int x, int y, size_t w, size_t h, int thickness, double color);

    void put(int x0, int y0, int x1, int y1, int thickness, double color, int type);

    void dump(uint8_t * bgra32, size_t wid, size_t hgt) const;

public:

    cv_board();

    cv_board(const cv_board & rhs);

    cv_board(cv_board && rhs);

    ~cv_board();

private:

    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;
};
