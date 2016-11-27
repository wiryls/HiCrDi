/****************************************************************************
 *
 *  @file       config.cpp
 *  @brief      Store all default values.
 *
 *
 *  Date        Name        Description
 *  16/11/20    MYLS        Creation.
 *
 ***************************************************************************/

#include <opencv2/opencv.hpp>
#include "utility/settings.hpp"
#include "utility/log.hpp"
#include "utility/types.hpp"

/****************************************************************************
 *  namespace beg
 ***************************************************************************/
namespace hb { namespace cfg {
/****************************************************************************
 *  static
 ***************************************************************************/

static auto & settings = cm::the_settings::Instance();

template<typename T> inline static T clamp(T min, T max, T val)
{
    if (max < min)
        logger::err("hb::cfg::clamp, (max = ", max, ") > (min = ", min, ")");

    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}

/****************************************************************************
 *  extern
 ***************************************************************************/

/*  opencv cv template iamges */

namespace match {

    extern const double contrast_alpha         = clamp(  0.0,   3.0,   1.96);
    extern const double binarization_threshold = clamp(  0.0, 255.0, 249.0 );

    extern cv::Mat  dino_0_img = cv::imread(settings.get("match.dinosaur1", "template/dinosaur_stand.bmp"), CV_LOAD_IMAGE_GRAYSCALE);
    extern cv::Mat  dino_1_img = cv::imread(settings.get("match.dinosaur2", "template/dinosaur_down.bmp" ), CV_LOAD_IMAGE_GRAYSCALE);
    extern cv::Mat    bird_img = cv::imread(settings.get("match.bird"     , "template/bird.bmp"),           CV_LOAD_IMAGE_GRAYSCALE);
    extern cv::Mat  cactus_img = cv::imread(settings.get("match.cactus"   , "template/cactus.bmp"),         CV_LOAD_IMAGE_GRAYSCALE);
    extern cv::Mat restart_img = cv::imread(settings.get("match.restart"  , "template/restart.bmp"),        CV_LOAD_IMAGE_GRAYSCALE);

    extern bool is_loading_completed
        =!( dino_0_img.empty()
        ||  dino_1_img.empty()
        ||    bird_img.empty()
        ||  cactus_img.empty()
        || restart_img.empty()
        );

    extern const int delta_dino_1to0_x = -9;
    extern const int delta_dino_1to0_y = -8;

    extern const int tm_method     = cv::TM_CCOEFF_NORMED;
    extern const int tm_th_dino    = clamp(  0, 255, 196 );
    extern const int tm_th_cactus  = clamp(  0, 255, 144 );
    extern const int tm_th_bird    = clamp(  0, 255, 196 );
    extern const int tm_th_restart = clamp(  0, 255, 160 );
}

namespace km
{
    extern const size_t max_speed_sample  = settings.get("km.max_speed_sample", 6);
    extern const size_t max_origin_sample = settings.get("km.max_origin_sample", 30);
}

namespace mycalc
{
    extern const int max_flying_enemy_height = settings.get("mycalc.max_flying_enemy_height", 40);
    extern const int mid_flying_enemy_height = settings.get("mycalc.mid_flying_enemy_height", 20);
    extern const int func_param_k            = settings.get("mycalc.func_param_k", 40);
    extern const int func_param_b            = settings.get("mycalc.func_param_b", 18);
    extern const size_t keep_jumping         = settings.get("mycalc.keep_jumping", 1U);
}

/*  other */

/****************************************************************************
 *  namespace end
 ***************************************************************************/
}}
