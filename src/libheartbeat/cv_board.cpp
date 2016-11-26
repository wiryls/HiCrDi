/****************************************************************************
 *
 *  @file       cv_board.cpp
 *  @brief      Implement of cv_board.hpp
 *
 *
 *  Date        Name        Description
 *  16/11/23    MYLS        Creation.
 *
 ***************************************************************************/

#include <opencv2/opencv.hpp>

#include "cv_board.hpp"

/****************************************************************************
 *  basic
 ***************************************************************************/

struct hb::cv_board::Pimpl
{
    cv::Mat img;
};

hb::cv_board::cv_board() : pimpl(std::make_unique<Pimpl>()) {}

hb::cv_board::cv_board(const cv_board & rhs) : pimpl(std::make_unique<Pimpl>(*rhs.pimpl)) {}

hb::cv_board::cv_board(cv_board && rhs) : pimpl(std::move(rhs.pimpl)) {}

hb::cv_board::~cv_board() {}

/****************************************************************************
 *  method
 ***************************************************************************/

hb::cv_board::operator status_t() const
{
    auto & p = *pimpl;
    if (p.img.empty())
        return status_t::uninitialized;
    else
        return status_t::ready;
}

void hb::cv_board::clear()
{
    auto & p = *pimpl;
    p.img.release();
}

void hb::cv_board::set(uint8_t * img_8u1c, size_t hgt, size_t wid)
{
    if (!(img_8u1c || hgt || wid))
        return;

    auto & p = *pimpl;
    cv::Mat(hgt, wid, CV_8UC1, img_8u1c).copyTo(p.img);
}

void hb::cv_board::put(int x, int y, const char * text, double scale, int thickness, double color)
{
    if (*this != status_t::ready)
        return;

    auto & img = pimpl->img;
    y = img.rows - y;

    cv::putText
    (
        img,
        text,
        cv::Point(x, y),
        cv::FONT_HERSHEY_COMPLEX,
        scale,
        cv::Scalar::all(color),
        thickness
    );
}

void hb::cv_board::put(int x, int y, size_t w, size_t h, int thickness, double color)
{
    if (*this != status_t::ready)
        return;

    auto & p = *pimpl;
    auto & img = p.img;
    x =            x - (w >> 1);
    y = img.rows - y - (h >> 1);

    cv::Rect2i r(x, y, w, h);
    cv::rectangle(img, r, cv::Scalar::all(color), thickness);
}

void hb::cv_board::put(int x0, int y0, int x1, int y1, int thickness, double color, int type)
{
    if (*this != status_t::ready)
        return;

    auto & p = *pimpl;
    auto & img = p.img;

    y0 = img.rows - y0;
    y1 = img.rows - y1;

    cv::line(img, cv::Point2i(x0, y0), cv::Point2i(x1, y1), cv::Scalar::all(color), thickness, type);
}

void hb::cv_board::dump(uint8_t * bgra32, size_t wid, size_t hgt) const
{
    if (*this != status_t::ready)
        return;

    auto & p = *pimpl;
    auto & img = p.img;

    auto idx  = 0U;
    auto iter = img.datastart;
    auto end  = img.dataend;

    while (iter != end) {
        bgra32[idx++] = *iter;
        bgra32[idx++] = *iter;
        bgra32[idx++] = *iter;
        bgra32[idx++] = 0xFFU;
        iter++;
    }
}