/****************************************************************************
 *
 *  @file       km_mate.cpp
 *  @brief      Implement of km_mate.hpp
 *
 *
 *  Date        Name        Description
 *  16/11/21    MYLS        Creation.
 *
 ***************************************************************************/

#include <cstdio>
#include <algorithm>
#include <functional>
#include <numeric>
#include <deque>

#include "utility/log.hpp"
#include "utility/timer.hpp"
#include "utility/algorithm.hpp"

#include "km_mate.hpp"
#include "cv_board.hpp"
#include "my_mind.hpp"
#include "datatype.hpp"

/****************************************************************************
 *  config
 ***************************************************************************/

namespace hb { namespace cfg { namespace km
{
    extern const size_t max_speed_sample;
    extern const size_t max_origin_sample;
    
} } }

/****************************************************************************
 *  internal
 ***************************************************************************/

struct traceable_object_t
{
    float x;
    float y;
    float w;
    float h;
    std::deque<float> sx;
    std::deque<float> sy;
    float r;  /* reliability [0, 1] */
    unsigned t; /* type */
    unsigned i; /* id */
};

template<typename T> inline T avg_speed(const std::deque<T> & src)
{
    if (src.empty())
        return T(0);
    else
        return std::accumulate(src.begin(), src.end(), T(0)) / T(src.size());
}

template<typename T> inline void push_speed(std::deque<T> & src, T && val)
{
    src.push_back(val);
    while (src.size() > hb::cfg::km::max_speed_sample)
        src.pop_front();
}

traceable_object_t make_traceable_object
(
    const hb::Object & tar,
    int x,
    int y,
    int w,
    int h,
    int type
)
{
    return std::move(traceable_object_t
    {
        static_cast<decltype(traceable_object_t::x )>(tar.x - x),
        static_cast<decltype(traceable_object_t::y )>(tar.y - y),
        static_cast<decltype(traceable_object_t::w )>(tar.w + w),
        static_cast<decltype(traceable_object_t::h )>(tar.h + h),
                    decltype(traceable_object_t::sx)(hb::cfg::km::max_speed_sample),
                    decltype(traceable_object_t::sy)(hb::cfg::km::max_speed_sample),
        static_cast<decltype(traceable_object_t::r )>(0.5f), // is this ok?
        static_cast<decltype(traceable_object_t::t )>(type),
        static_cast<decltype(traceable_object_t::i )>(0)
    });
}

hb::Info make_info(const traceable_object_t & src)
{
    float avg_sx = avg_speed(src.sx);
    float avg_sy = avg_speed(src.sy);
    return std::move(hb::Info
    {
        static_cast<decltype(hb::Info::x )>(src.x ),
        static_cast<decltype(hb::Info::y )>(src.y ),
        static_cast<decltype(hb::Info::w )>(src.w ),
        static_cast<decltype(hb::Info::h )>(src.h ),
        static_cast<decltype(hb::Info::sx)>(avg_sx),
        static_cast<decltype(hb::Info::sy)>(avg_sy)
    });
}

float cmp_traceable_object(const traceable_object_t & lhs, const traceable_object_t & rhs)
{
    {   /* type */
        if (lhs.t != rhs.t && lhs.t && rhs.t)
            return 0.0f;
    }

    float location;
    {   /* location */
        float delta_x = lhs.x - rhs.x;
        float delta_y = lhs.y - rhs.y;

        if (delta_x > 800.0f || delta_y > 800.0f)
            location = 0.0f;
        else
            location = 1.0f / (1.0f + 0.0001f * (delta_x * delta_x + delta_y * delta_y));
    }

    float shape;
    {   /* shape */
        float delta_w = std::abs(lhs.w - rhs.w);
        float delta_h = std::abs(lhs.h - rhs.h);
        float delta_r = std::abs(lhs.w / lhs.h - rhs.w / rhs.h);
        shape = 1.0f / (1.0f + 0.1f * (delta_w + delta_h) + 3.3f * delta_r);
    }

    float score
        = 0.2f * shape
        + 0.8f * location
        ;

    if (score < 0.4f)
        score = 0.0f;

    return score;
}

void update_traceable_object(traceable_object_t & dst, const traceable_object_t & src, float delta_time)
{
    {
        push_speed(dst.sx, (src.x - dst.x) / delta_time);
        push_speed(dst.sy, (src.y - dst.y) / delta_time);
    }

    dst.r += delta_time * 0.5f;

    //static const float delta_time_min = 0.0f;
    //static const float delta_time_max = 1.0f;
    //delta_time = std::min(std::max(delta_time, delta_time_min), delta_time_max);

    dst.x = src.x;
    dst.y = src.y;
    dst.w = src.w;
    dst.h = src.h;

    if (dst.r < 0.0f)
        dst.r = 0.0f;
    else if (dst.r > 1.0f)
        dst.r = 1.0f;
}

void update_traceable_object(traceable_object_t & dst, float delta_time)
{
    float avg_sx = avg_speed(dst.sx);
    float avg_sy = avg_speed(dst.sy);

    dst.x += delta_time * avg_sx;
    dst.y += delta_time * avg_sy;
    dst.r -= delta_time * 2.0f;

    if (dst.r < 0.0f)
        dst.r = 0.0f;
    else if (dst.r > 1.0f)
        dst.r = 1.0f;
}

void merge_traceable_objects
(
    const std::vector<traceable_object_t> & src,
          std::vector<traceable_object_t> & dst,
    float                                   delta_time
)
{
    /* predict next location */
    for (auto & o : dst) {
        o.x += delta_time * avg_speed(o.sx);
        o.y += delta_time * avg_speed(o.sy);
    }

    /* km */
    std::function<float (const traceable_object_t &, const traceable_object_t &)> evaluate = cmp_traceable_object;
    auto result = hb::match::kuhn_munkres(src, dst, evaluate);

    /* roll back location */
    for (auto & o : dst) {
        o.x -= delta_time * avg_speed(o.sx);
        o.y -= delta_time * avg_speed(o.sy);
    }

    /* update matched */
    for (auto & mth : result.matched)
        update_traceable_object(dst[mth.snd], src[mth.fst], delta_time);

    /* update not matched */
    for (auto idx : result.not_matched_of_snd)
        update_traceable_object(dst[idx], delta_time);

    /* remove low scored */
    auto cnt = dst.size();
    for (size_t i = 0U; i < cnt; i++)
        if (dst[i].r < 0.5f)
            if (i != --cnt)
                std::swap(dst[i], dst[cnt]);
    cnt = dst.size() - cnt;
    while (cnt-- > 0U)
        dst.pop_back();

    /* add new object (without id) */
    for (auto idx : result.not_matched_of_fst)
        dst.push_back(src[idx]);
}

/****************************************************************************
 *  pimpl
 ***************************************************************************/

struct hb::km_mate::Pimpl
{
    bool is_running;
    int count;
    std::deque<int> origin_x;
    std::deque<int> origin_y;
    std::vector<traceable_object_t> objects;
    cm::timer timer;
};

hb::km_mate::km_mate() : pimpl(std::make_unique<Pimpl>()) {}

hb::km_mate::km_mate(const km_mate & rhs) : pimpl(std::make_unique<Pimpl>(*rhs.pimpl)) {}

hb::km_mate::km_mate(km_mate && rhs) : pimpl(std::move(rhs.pimpl)) {}

hb::km_mate::~km_mate() {}

/****************************************************************************
 *  method
 ***************************************************************************/

hb::km_mate::operator status_t() const
{
    auto & p = *pimpl;
    if (p.is_running)
        return status_t::ready;
    else
        return status_t::uninitialized;
}

void hb::km_mate::clear()
{
    auto & p = *pimpl;

    p.count = 0U;
    p.origin_x.clear();
    p.origin_y.clear();
    p.is_running = false;
    p.timer.stop();
    p.objects.clear();
}

hb::km_mate::Self & hb::km_mate::operator >> (my_mind & other)
{
    auto & p = *pimpl;

    /* prepar x, y */
    auto & origin_x = p.origin_x;
    auto & origin_y = p.origin_y;
    if (origin_x.empty() || origin_y.empty())
        return *this;

    auto x = *std::min_element(origin_x.begin(), origin_x.end());
    auto y = *std::min_element(origin_y.begin(), origin_y.end());

    /* prepar dino */
    Info dino
    {
        static_cast<decltype(Info::x )>(origin_x.back() - x),
        static_cast<decltype(Info::y )>(origin_y.back() - y),
        static_cast<decltype(Info::w )>(0),
        static_cast<decltype(Info::h )>(0),
        static_cast<decltype(Info::sx)>(0),
        static_cast<decltype(Info::sy)>(0)
    };

    /* prepar obstacles */
    std::vector<Info> obstacles;
    auto & objects = p.objects;
    obstacles.reserve(objects.size());
    for (auto & o : objects)
        if (o.x > -20.0f)
            obstacles.push_back(std::move(make_info(o)));

    /* send */
    other.set(x, y, std::move(dino), std::move(obstacles));
    return *this;
}

hb::km_mate::Self & hb::km_mate::operator >> (cv_board & other)
{
    auto & p = *pimpl;
    if (p.origin_x.empty() || p.origin_y.empty())
        return *this;

    auto x0 = *std::min_element(p.origin_x.begin(), p.origin_x.end());
    auto y0 = *std::min_element(p.origin_y.begin(), p.origin_y.end());

    for (auto & o : p.objects) {
        char buffer[20];
        ::sprintf_s(buffer, "%u", o.i);

        auto w = static_cast<int>(o.w);
        auto h = static_cast<int>(o.h);
        auto x = static_cast<int>(o.x) + x0;
        auto y = static_cast<int>(o.y) + y0;
        other.put(x, y, w, h, 1, 128.0);
        other.put(x - (w >> 2), y, buffer, 2.0, 3, 128.0);
    }
    return *this;
}

void hb::km_mate::set(bool is_game_over)
{
    auto & p = *pimpl;
    if (is_game_over != p.is_running)
        return;

    if (is_game_over == true) {
        /* end */
        p.origin_x.clear();
        p.origin_y.clear();
        p.objects.clear();
        p.is_running = false;
        p.timer.stop();
    } else {
        /* begin */
        p.is_running = true;
        p.timer.start();
    }
}

void hb::km_mate::set(
    const Object             & ori,
    const std::vector<Object>& birds,
    const std::vector<Object>& cactus)
{
    auto & p = *pimpl;

    /* update location */
    p.origin_x.emplace_back(ori.x);
    p.origin_y.emplace_back(ori.y);
    if (p.origin_x.size() > hb::cfg::km::max_origin_sample)
        p.origin_x.pop_front();
    if (p.origin_y.size() > hb::cfg::km::max_origin_sample)
        p.origin_y.pop_front();

    auto x = *std::min_element(p.origin_x.begin(), p.origin_x.end());
    auto y = *std::min_element(p.origin_y.begin(), p.origin_y.end());
    auto w = ori.w;
    auto h = ori.h;

    /* new objects */
    std::vector<traceable_object_t> new_objects;
    new_objects.reserve(3);

    for (auto & tgt : birds)
        new_objects.push_back(make_traceable_object(tgt, x, y, w, h, 1));
    for (auto & tgt : cactus)
        new_objects.push_back(make_traceable_object(tgt, x, y, w, h, 2));

    /* merge */
    auto delta_time = static_cast<float>(p.timer.get_elapsed_time()) / 1000.0f;
    merge_traceable_objects(new_objects, p.objects, delta_time);

    for (auto & o : p.objects)
        if (o.i == 0)
            o.i = ++p.count;

    /* restart timer */
    p.timer.start();
    p.is_running = true;
}
