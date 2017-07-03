/****************************************************************************
 *
 *  @file       Heart.cpp
 *  @brief      Implement of Heart.hpp
 *
 *
 *  Date        Name        Description
 *  16/04/21    MYLS        Creation.
 *
 ***************************************************************************/

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "utility/timer.hpp"
#include "utility/log.hpp"
#include "cv_eye.hpp"
#include "km_mate.hpp"
#include "my_mind.hpp"
#include "cv_board.hpp"
#include "Heart.hpp"

/****************************************************************************
 *  private Pimpl
 ***************************************************************************/

struct hb::Heart::Pimpl
{
    bool is_runnning;
    float last_time;
    cm::timer timer;

    cv_board processor00;
    cv_eye   processor01;
    km_mate  processor02;
    my_mind  processor03;

    void clear_most();
};

/****************************************************************************
 *  private
 ***************************************************************************/

void hb::Heart::Pimpl::clear_most()
{
    is_runnning = false;
    last_time = 0.f;

    processor01.clear();
    processor03.clear();
    processor02.clear();
}


/****************************************************************************
 *  public
 ***************************************************************************/

void hb::Heart::start()
{
    pimpl->timer.start();
    pimpl->is_runnning = true;
}

void hb::Heart::pause()
{
    pimpl->timer.pause();
    pimpl->clear_most();
}

void hb::Heart::resume()
{
    pimpl->timer.resume();
    pimpl->last_time = pimpl->timer.get_elapsed_time() * 1e-3f;
    pimpl->is_runnning = true;
}

void hb::Heart::stop()
{
    pimpl->timer.stop();
    pimpl->clear_most();
    //pimpl->processor02.clear();
}

bool hb::Heart::view(uint8_t const bgra32[], size_t wid, size_t hgt)
{
    auto & p = *pimpl;

    /* get bgra img */
    p.processor01.set(bgra32, wid, hgt);

    /* process */
    if (p.processor01 == cv_eye::status_t::ready) {
        p.processor01 >> p.processor02;
        if (p.processor02 == km_mate::status_t::ready) {
            p.processor02 >> p.processor03;
        }
    }

    return p.processor01.is_dino_found() || p.processor01.is_dino_dead();
}

size_t hb::Heart::plan() const
{
    auto & p = *pimpl;
    if (!p.is_runnning)
        return size_t();
    else
        return p.processor03.get();
}

bool hb::Heart::dead() const
{
    auto & p = *pimpl;

    if (!p.is_runnning)
        return false;
    else
        return p.processor01.is_dino_dead();
}

bool hb::Heart::ready() const
{
    auto & p = *pimpl;
    return true;
}

void hb::Heart::know(uint8_t bgra32[], size_t wid, size_t hgt) const
{
    auto & p = *pimpl;

    p.processor01 >> p.processor00;
    p.processor02 >> p.processor00;
    p.processor03 >> p.processor00;

    p.processor00.dump(bgra32, wid, hgt);
}


hb::Heart::Heart() : pimpl(std::make_unique<Pimpl>()) { stop(); }

hb::Heart::~Heart() {}
