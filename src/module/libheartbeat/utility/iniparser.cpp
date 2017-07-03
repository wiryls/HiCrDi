/*********************************************************************
 *
 *  @file       iniparser.cpp
 *  @brief      iniparser
 *
 *
 *  Date        Name        Description
 *  04/03/15    MYLS        Creation.
 *
 *********************************************************************/

#include <map>
#include <fstream>
#include <typeinfo>
#include "log.hpp"
#include "iniparser.hpp"



struct cm::iniparser::Pimpl
{
    std::map<std::string, std::string> data;    /**< data[section.key] = value */
};

cm::iniparser::
iniparser() :
    pimpl(std::make_unique<Pimpl>())
{}

cm::iniparser::
~iniparser()
{}


bool cm::iniparser::
load(const std::string & filePath)
{
    auto & data = pimpl->data;

    /* Check is file open */
    std::fstream file(filePath, std::ios::in);
    if (!file.is_open()) {
        logger::err(typeid(*this), ":: can not load file: ", filePath);
        return false;
    }

    /* Start to parse */
    data.clear();
    std::string line, section;
    while (getline(file, line)) { /* read a line */
        trim(line);
        if (line.empty())
            continue;

        switch (line.front()) {
        case'[':    /* section */
            if (line.back() == ']')
                section = std::move(trim(line.substr(1, line.length() - 2)));
            break;
        case'#':    /* comment */
        case';':
        case' ':
            break;
        default:    /* key=value */
            size_t idxOfEqu = line.find_first_of('=');
            if (line.npos == idxOfEqu)
                break;
            auto key = line.substr(0, idxOfEqu);
            auto value = line.substr(idxOfEqu + 1);
            data[section + '.' + trim(key)] = std::move(trim(value));
            break;
        }
    }

    return true;
}

const std::string & cm::iniparser::
get_string(const std::string & key) const
{
    auto iter = pimpl->data.find(key);
    if (iter == pimpl->data.end()) {
        logger::err(typeid(*this), ":: can not find: ", key);
        static const std::string empty;
        return empty;
    }

    return iter->second;
}

void cm::iniparser::
clear()
{
    pimpl->data.clear();
}

std::string& cm::iniparser::
trim(std::string & str)
{
    if (!str.empty()) {
        static const std::string whitespace = " \t";
        str.erase(0u, str.find_first_not_of(whitespace));
        str.erase(str.find_last_not_of(whitespace) + 1u);
    }
    return str;
}

std::string cm::iniparser::
trim(std::string && str)
{
    if (!str.empty()) {
        static const std::string whitespace = " \t";
        str.erase(0u, str.find_first_not_of(whitespace));
        str.erase(str.find_last_not_of(whitespace) + 1u);
    }
    return str;
}