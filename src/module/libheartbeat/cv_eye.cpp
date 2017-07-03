/****************************************************************************
 *
 *  @file       cv_eye.cpp
 *  @brief      Implement of cv_eye.hpp
 *
 *
 *  Date        Name        Description
 *  16/04/21    MYLS        Creation.
 *
 ***************************************************************************/

#include <opencv2/opencv.hpp>

#include "utility/log.hpp"

#include "cv_eye.hpp"
#include "km_mate.hpp"
#include "cv_board.hpp"
#include "datatype.hpp"

/****************************************************************************
 *  config
 ***************************************************************************/

namespace hb { namespace cfg { namespace match
{
    extern const double contrast_alpha;
    extern const double binarization_threshold;

    extern cv::Mat  dino_0_img;
    extern cv::Mat  dino_1_img;
    extern cv::Mat    bird_img;
    extern cv::Mat  cactus_img;
    extern cv::Mat restart_img;

    extern bool is_loading_completed;

    extern const int delta_dino_1to0_x;
    extern const int delta_dino_1to0_y;

    extern const int tm_method;
    extern const int tm_th_dino;
    extern const int tm_th_cactus;
    extern const int tm_th_bird;
    extern const int tm_th_restart;
} } }

/****************************************************************************
 *  internal
 ***************************************************************************/

static void image_process(cv::Mat const & src, cv::Mat & dst)
{
    /* [1]grayscale    --- convert to single-channel */
    cv::cvtColor(src, dst, cv::COLOR_BGRA2GRAY);

    /* [2]contrast     --- highlight foreground */
    dst.convertTo(dst, -1, hb::cfg::match::contrast_alpha, 0.0);

    /* [3]equalization --- increase the color discrimination */
    cv::equalizeHist(dst, dst);

    /* [4]binarization --- remove background */
    auto total   = static_cast<size_t>(dst.total());
    auto nonzero = static_cast<size_t>(cv::countNonZero(dst));
    auto method  = (66U * total > 100U * nonzero) ? CV_THRESH_BINARY : CV_THRESH_BINARY_INV;

    cv::threshold(dst, dst, hb::cfg::match::binarization_threshold, 255.0, method);

    nonzero      = static_cast<size_t>(cv::countNonZero(dst));
    if (total < (nonzero << 1))
        cv::bitwise_not(dst, dst);

    /* [5]open         --- remove clouds, roads and some noise */
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::erode (dst, dst, kernel);
    cv::dilate(dst, dst, kernel);

    /* References:
     * 1. http://docs.opencv.org/2.4/doc/tutorials/core/basic_linear_transform/basic_linear_transform.html
     */
}

static bool find_existence(cv::Mat const & src, cv::Mat const & tmpl, int threshold)
{
    cv::Mat score;
    auto method = hb::cfg::match::tm_method;
    cv::matchTemplate(src, tmpl, score, method);
    if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED)
        score.convertTo(score, -1, -1.0, 1.0);

    double max;
    cv::minMaxLoc(score, nullptr, &max, nullptr, nullptr);
    return static_cast<int>(max * 255.0) > threshold;
}

static std::vector<hb::Object> find_one(cv::Mat const & src, cv::Mat const & tmpl, int threshold)
{
    cv::Mat score;
    auto method = hb::cfg::match::tm_method;
    cv::matchTemplate(src, tmpl, score, method);

    /* note:
     * "For SQDIFF and SQDIFF_NORMED, the best matches are lower values.
     *  For all the other methods, the higher the better."
     */
    if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED)
        score.convertTo(score, -1, -1.0, 1.0);

    /* find the only one matched */
    double max;
    cv::Point match;
    cv::minMaxLoc(score, nullptr, &max, nullptr, &match);

    std::vector<hb::Object> rv;
    if (static_cast<int>(max * 255.0) > threshold)
        rv.push_back({ match.x, match.y, tmpl.cols, tmpl.rows});

    return rv;

    /* References:
     * [Template Matching]
     * (http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html)
     */
}

std::vector<hb::Object> find_several(cv::Mat const & src, cv::Mat const & tmpl, int threshold)
{
    cv::Mat score;
    auto method = hb::cfg::match::tm_method;
    cv::matchTemplate(src, tmpl, score, method);

    if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED)
        score.convertTo(score, CV_8UC1, -255.0, 255.0);
    else
        score.convertTo(score, CV_8UC1, 255.0);

    cv::Mat mask;
    cv::threshold(score, mask, threshold, 255, CV_THRESH_BINARY);

    std::vector<hb::Object> rv;
    for (auto iter = mask.datastart; iter != mask.dataend; iter++) { /* note: fast but not safe */
        if (*iter == 255) {
            int i = static_cast<int>(iter - mask.datastart);
            int x = i % mask.cols;
            int y = i / mask.cols;

            /* fill the hole for searching next one */
            cv::Rect rect;
            cv::floodFill(mask, cv::Point(x, y), cv::Scalar::all(0), &rect);

            /* get the max score */
            double max;
            cv::Point match;
            cv::Mat roi = score(rect);
            cv::minMaxLoc(roi, nullptr, &max, nullptr, &match);
            if (static_cast<int>(max * 100.0) > threshold)
                rv.push_back({ x + match.x, y + match.y, tmpl.cols, tmpl.rows});
        }
    }

    return rv;
}

static void merge_adjacent(std::vector<hb::Object>& rects, int delta_width, int delta_height)
{
    size_t n = rects.size();
    for (size_t i = 0U; i < n; i++) {
        auto & lhs = rects[i];
        int x    = lhs.x -  delta_width;
        int y    = lhs.y -  delta_height;
        int wid  = lhs.w + (delta_width  << 1);
        int hgt  = lhs.h + (delta_height << 1);
        for (size_t j = i + 1U; j < n; j++) {
            auto & rhs = rects[j];
            int xmin = x          - rhs.w;
            int ymin = y          - rhs.h;
            int xmax = xmin + wid + rhs.w;
            int ymax = ymin + hgt + rhs.h;
            if (xmin <= rhs.x && rhs.x <= xmax && ymin <= rhs.y && rhs.y <= ymax) {
                int left   = std::min(lhs.x, rhs.x);
                int right  = std::max(lhs.x + lhs.w, rhs.x + rhs.w);
                int top    = std::min(lhs.y, rhs.y);
                int bottom = std::max(lhs.y + lhs.h, rhs.y + rhs.h);
                lhs.x      = left;
                lhs.y      = top;
                lhs.h      = bottom - top;
                lhs.w      = right - left;
                if (j != --n)
                    std::swap(rects[n], rhs);

                /* restart merging */
                i = ~size_t(0U);
            }
        }
    }

    n = rects.size() - n;
    while (n-- > 0U)
        rects.pop_back();
}

static void object_coordinate_transform(int w, int h, hb::Object & o)
{
    o.x =     o.x + (o.w >> 1);
    o.y = h - o.y - (o.h >> 1);
}

/****************************************************************************
 *  impl
 ***************************************************************************/

struct hb::cv_eye::Pimpl
{
    cv::Mat img;

    bool                is_game_over;
    std::vector<Object> dino;
    std::vector<Object> birds;
    std::vector<Object> cactus;
};

hb::cv_eye::cv_eye() : pimpl(std::make_unique<Pimpl>()) {}

hb::cv_eye::cv_eye(const cv_eye & rhs) : pimpl(std::make_unique<Pimpl>(*rhs.pimpl)) {}

hb::cv_eye::cv_eye(cv_eye && rhs) : pimpl(std::move(rhs.pimpl)) {}

hb::cv_eye::~cv_eye() {}

/****************************************************************************
 *  method
 ***************************************************************************/

hb::cv_eye::operator status_t() const
{
    auto & p = *pimpl;

    if (!hb::cfg::match::is_loading_completed)
        return status_t::error;
    else if (!p.img.empty())
        return status_t::ready;
    else
        return status_t::uninitialized;
}

void hb::cv_eye::clear()
{
    auto & p = *pimpl;

    p.is_game_over = false;
    p.img.release();
    p.dino.clear();
    p.birds.clear();
    p.cactus.clear();
}

const hb::cv_eye::Self & hb::cv_eye::operator >> (km_mate & other) const
{
    if (*this != status_t::ready)
        return *this;

    auto & p = *pimpl;

    if (p.is_game_over) {
        other.set(true);
        return *this; /* game_over */
    }

    if (p.dino.empty())
        return *this; /* invaild */

    other.set(p.dino.front(), p.birds, p.cactus);

    return *this;
}

const hb::cv_eye::Self & hb::cv_eye::operator >> (cv_board & other) const
{
    auto & p   = *pimpl;
    auto & img = p.img;
    other.set(img.data, img.rows, img.cols);

    if (!p.dino.empty()) {
        char buffer[20];
        auto o = p.dino.front();
        auto x = o.x;
        auto y = o.y;
        std::sprintf(buffer, "(%d, %d)", x, y);
        other.put(0, 5, buffer, 0.5, 1, 128.0);
        other.put(x, y, o.w, o.h, 1, 128.0);
    }

    return *this;
}

bool hb::cv_eye::is_dino_found() const
{
    return !(pimpl->dino.empty());
}

bool hb::cv_eye::is_dino_dead() const
{
    return pimpl->is_game_over;
}

void hb::cv_eye::set(uint8_t const * bgra32, size_t wid, size_t hgt)
{
    auto & p = *pimpl;
    auto & img = p.img;

    if (bgra32 && wid && hgt) {
        image_process(cv::Mat((int)hgt, (int)wid, CV_8UC4, (void *)bgra32), img);

        if (img.empty()) {
            logger::err(typeid(*this), ":: source image has something wrong.");
            return; /* invaild image */
        }

        p.is_game_over = find_existence(img, hb::cfg::match::restart_img, hb::cfg::match::tm_th_restart);
        if (p.is_game_over) {
            p.dino.clear();
            p.birds.clear();
            p.cactus.clear();
            return; /* game_over */
        }

        p.dino = find_one(img, hb::cfg::match::dino_0_img, hb::cfg::match::tm_th_dino);
        if (p.dino.empty()) {
            p.dino = find_one(img, hb::cfg::match::dino_1_img, hb::cfg::match::tm_th_dino);
            if (!p.dino.empty()) {
                auto & dino = p.dino.front();
                dino.x += cfg::match::delta_dino_1to0_x;
                dino.y += cfg::match::delta_dino_1to0_y;
            }
        }

        if (p.dino.empty()) {
            p.birds.clear();
            p.cactus.clear();
            return; /* cannot find dinosaur */
        }

        p.birds  = find_several(img, hb::cfg::match::bird_img  , hb::cfg::match::tm_th_bird  );
        p.cactus = find_several(img, hb::cfg::match::cactus_img, hb::cfg::match::tm_th_cactus);

        merge_adjacent(p.cactus, 16, 0);

        int w = img.cols;
        int h = img.rows;
        for (auto & o : p.dino)   object_coordinate_transform(w, h, o);
        for (auto & o : p.birds)  object_coordinate_transform(w, h, o);
        for (auto & o : p.cactus) object_coordinate_transform(w, h, o);

        // logger::msg("point(", p.dino[0].x, ",", p.dino[0].y, ")");
    }
}

