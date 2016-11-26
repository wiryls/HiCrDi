/*********************************************************************
 *
 *  @file       test.cpp
 *  @brief      I'll do some experiments.
 *
 *
 *  Date        Name        Description
 *  21/04/16    MYLS        Creation.
 *
 *********************************************************************/

/* headers */
#define _CRT_SECURE_NO_WARNINGS
#include <sstream>
#include <iostream>
#include <string>
#include <array>
#include <deque>

#include <opencv2/opencv.hpp>

#include <libheartbeat.hpp>

#include "cv_eye.hpp"


/* a simple test */

template<typename fst_t, typename snd_t, typename ... rst_t> size_t
process(fst_t & fst, snd_t & snd, rst_t ... others)
{
    if (!fst)
        return 0U;

    fst >> snd;

    return 1U + process(snd, others...);
}

template<typename fst_t, typename snd_t> size_t
process(fst_t & fst, snd_t & snd)
{
    return static_cast<size_t>(fst >> snd);
}

/* function */



int main(void)
{
    std::cout << std::hex << hb::version() << std::endl;
    // std::cout << process(_1, _2, _1, _2, _1) << std::endl;

    // std::cout << hb::config::A << std::endl;

    return 0;
}
