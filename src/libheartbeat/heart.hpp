/*********************************************************************
 *
 *  @file       Heart.hpp
 *  @brief
 *
 *
 *  Date        Name        Description
 *  16/04/21    MYLS        Creation.
 *
 *********************************************************************/
#pragma once

#include <memory>

namespace hb {
    class Heart;
}

/**
 *  \brief  A beating Heart
 */
class hb::Heart
{

public:

    void start();

    void pause();

    void resume();

    void stop();

public:

    bool view(uint8_t const bgra32[], size_t wid, size_t hgt);

    void know(uint8_t       bgra32[], size_t wid, size_t hgt) const;

    size_t plan() const;

    bool dead() const;

    bool ready() const;

public:

    /**
     *  \brief constructor
     */
    Heart();

    /**
     *  \brief destructor
     */
    ~Heart();

private:

    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;
};
