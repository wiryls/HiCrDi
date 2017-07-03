/**********************************************************************
 *
 *  @file       settings.hpp
 *  @brief      used to read a configuration file.
 *
 *
 *  Date        Name        Description
 *  15/08/29    MYLS        Creation.
 *  16/03/18    MYLS        Move from boost::property_tree to my
 *                          INIParser.
 *
 *********************************************************************/

#pragma once

#include <sstream>

#include "singleton.hpp"
#include "iniparser.hpp"

namespace cm {
    class settings;
    typedef settings the_settings;
}

 /**
  *  \brief  class settings, used to read a configuration file.
  */
class cm::settings : public cm::singleton<settings> {

    friend class cm::singleton < settings >;

public:

    /**
     *  \brief  get, get the value from [section.key]
     *
     *  \param  key             "section.key"
     *  \param  default_value   if value not exists, default_value will be returned.
     *
     *  \return         if value exists then return it, else return default value.
     */
    template <typename _T> auto get(const std::string & key, const _T & default_value = _T()) const;

    const char * get(const std::string & key, const char * default_value = nullptr) const;

private:

    /**
     *  \brief  private constructor for singleton
     */
    settings();

    /**
     *  \brief  err msg
     */
    void err_fail_to_convert(const char * from, const char * to) const;

    /**
     *  \brief  dbg msg
     */
    void dbg_use_default_value(const char * key) const;

private:

    cm::iniparser ini_parser;           /**< to parser ini file */
};


/*********************************************************************
 *  get
 *********************************************************************/

template<> inline auto cm::settings::
get(const std::string & key, const std::string & default_value) const
{
    auto & value = ini_parser.get_string(key);
    if (value.empty()) {
        dbg_use_default_value(key.c_str());
        return default_value;
    }

    return value;
}

template<typename _T> inline auto cm::settings::
get(const std::string & key, const _T & default_value) const
{
    auto & value = ini_parser.get_string(key);
    if (value.empty()) {
        dbg_use_default_value(key.c_str());
        return default_value;
    }

    _T rv;
    std::istringstream iss(value);
    if (iss >> rv)
        return rv;

    err_fail_to_convert(key.c_str(), typeid(_T).name());
    return default_value;
}




