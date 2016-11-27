/****************************************************************************
 *
 *  @file       my_mind.cpp
 *  @brief      Implement of my_mind.hpp
 *
 *
 *  Date        Name        Description
 *  16/11/22    MYLS        Creation.
 *
 ***************************************************************************/

#include <algorithm>

#include "utility/log.hpp"
#include "datatype.hpp"
#include "my_mind.hpp"
#include "cv_board.hpp"

/****************************************************************************
 *  config
 ***************************************************************************/

namespace hb { namespace cfg { namespace mycalc
{
    extern const int max_flying_enemy_height;
    extern const int mid_flying_enemy_height;

    extern const int func_param_k;
    extern const int func_param_b;

    extern const size_t keep_jumping;
} } }

/****************************************************************************
 *  pimpl
 ***************************************************************************/

struct hb::my_mind::Pimpl
{
    int anchor_x;
    int anchor_y;
    Action lst_act;
    size_t keep_jumping;
    Info origin;
    std::vector<Info> obstacles;
};

hb::my_mind::my_mind() : pimpl(std::make_unique<Pimpl>()) {}

hb::my_mind::my_mind(const my_mind & rhs) : pimpl(std::make_unique<Pimpl>(*rhs.pimpl)) {}

hb::my_mind::my_mind(my_mind && rhs) : pimpl(std::move(rhs.pimpl)) {}

hb::my_mind::~my_mind() {}

/****************************************************************************
 *  method
 ***************************************************************************/

hb::my_mind::operator status_t() const
{
    auto & p = *pimpl;
    if (p.anchor_x == 0 && p.anchor_y == 0)
        return status_t::uninitialized;
    else
        return status_t::ready;
}

void hb::my_mind::clear()
{
    auto & p = *pimpl;

    p.keep_jumping = 0U;
    p.lst_act = Action::IDLE;
    p.anchor_x = 0;
    p.anchor_y = 0;
    p.obstacles.clear();
}

const hb::my_mind::Self & hb::my_mind::operator >> (cv_board & other) const
{
    auto & p = *pimpl;
    if (p.anchor_x == 0 && p.anchor_y == 0)
        return *this;

    /* anchor */
    auto x0 = p.anchor_x;
    auto y0 = p.anchor_y;
    other.put(0, y0, 1000,  y0, 1, 192.0, 8);
    other.put(x0, 0,   x0, 300, 1, 192.0, 8);

    /* act */
    const char * text
        = (p.lst_act == Action::JUMP)
        ? "JUMP"
        : (p.lst_act == Action::DOWN)
        ? "DOWN"
        : "IDLE"
        ;
    other.put(16, 128, text, 0.8, 2, 192.0);

    /* obstacles */
    for (auto & o : p.obstacles) {
        auto x = (o.sx * 40 / 100) + o.x + x0;
        auto y = (o.sy * 40 / 100) + o.y + y0;

        char buffer[20];
        std::sprintf(buffer, "%d", -o.sx / 90);
        other.put(x, y + 30, buffer, 0.8, 1, 128.0);

        other.put(x, y, 1, 64, 4, 64.0);
    }

    return *this;
}

hb::Action hb::my_mind::get() const
{
    auto & p = *pimpl;

    if (p.obstacles.empty())
        return p.lst_act = Action::IDLE;

    auto & closest = p.obstacles.front();
    auto & origin  = p.origin;

    /* flying enemy 1 */
    if (closest.y > hb::cfg::mycalc::max_flying_enemy_height) {
        // logger::msg("`(", closest.y, ")");
        return (p.lst_act = Action::IDLE);
    } else if (closest.y > hb::cfg::mycalc::mid_flying_enemy_height) {
        // logger::msg("-(", closest.y, ")");
        return (p.lst_act = Action::DOWN);
    }

    /* danger */
    if (closest.x > -10) {
        auto time = std::abs(closest.sx / hb::cfg::mycalc::func_param_k) + hb::cfg::mycalc::func_param_b;
        if (closest.x + closest.sx * time / 100 < (closest.w >> 2)) {
            if (p.lst_act != Action::JUMP && origin.y < 1) {
                // logger::msg("^(", th, ")");
                p.keep_jumping = hb::cfg::mycalc::keep_jumping;
                return (p.lst_act = Action::JUMP);
            } else if (p.keep_jumping != 0U) {
                p.keep_jumping--;
                // logger::msg("^^(", th, ")");
                return (p.lst_act = Action::JUMP);
            } else {
                // logger::msg("~(", origin.y, ")");
                return (p.lst_act = Action::IDLE);
            }
        }
    }

    if (p.lst_act == Action::JUMP && origin.y < 1) {
        // logger::msg("x");
        return (p.lst_act = Action::JUMP);
    }

    /* warning */
    //if (closest.x > 120 && origin.y > 1) {
    //    // logger::msg("down(", closest.y, ")");
    //    return (p.lst_act = Action::DOWN);
    //}

    // logger::msg(".");
    return (p.lst_act = Action::IDLE);
}

void hb::my_mind::set(int x0, int y0, Info && origin, std::vector<Info> && obstacles)
{
    auto & p = *pimpl;

    p.anchor_x  = x0;
    p.anchor_y  = y0;
    p.origin    = std::move(origin);
    p.obstacles = std::move(obstacles);

    auto cmp = [](const Info lhs, const Info rhs) {return lhs.x < rhs.x; };
    std::sort(p.obstacles.begin(), p.obstacles.end(), cmp);
}
