/**********************************************************************
 *
 *  @file       settings.cpp
 *  @brief      Implement of class settings
 *
 *
 *  Date        Name        Description
 *  15/08/29    MYLS        Creation.
 *
 *********************************************************************/

#include "log.hpp"
#include "settings.hpp"

cm::settings::
settings()
    : ini_parser()
{
    ini_parser.load("settings.ini");
}

const char * cm::settings::get(const std::string & key, const char * default_value) const
{
    auto & value = ini_parser.get_string(key);
    if (value.empty()) {
        dbg_use_default_value(key.c_str());
        return default_value;
    }

    return value.c_str();
}

void cm::settings::err_fail_to_convert(const char * from, const char * to) const
{
    logger::err(typeid(this), ":: cannot convert [", from, "] to [", to, "]");
}

void cm::settings::dbg_use_default_value(const char * key) const
{
    logger::dbg(typeid(this), ":: cannot find key [", key, "].");
}
