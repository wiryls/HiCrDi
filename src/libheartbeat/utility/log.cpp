/**********************************************************************
 *
 *  @file       log.hpp
 *  @brief      a simple class for logging
 *
 *
 *  Date        Name        Description
 *  02/09/15    MYLS        Creation.
 *
 *********************************************************************/

#include <typeinfo>
#include <mutex>

#include "settings.hpp"
#include "log.hpp"

void cm::logger::
out_console()
{
    std::cout << std::endl;
}

void cm::logger::out_file()
{
    file << std::endl;
}

void cm::logger::lock(bool if_lock)
{
    static std::mutex mutex;
    if (if_lock)
        mutex.lock();
    else
        mutex.unlock();
}

void cm::logger::
set(type_t type, target_t state)
{
    if (state == target_t::FILE && !file.is_open()) {
        file.open(the_settings::Instance().get("log.path", "log.txt"));
        if (!file.is_open()) {
            err(typeid(logger), " cannot create log file!");
            return;
        }
    }

    switch (type)
    {
    case type_t::MSG: { msg_target = state; break; }
    case type_t::ERR: { err_target = state; break; }
    case type_t::DBG: { dbg_target = state; break; }
    default: break;
    }
}

cm::logger::target_t    cm::logger::msg_target = target_t::CONSOLE;
cm::logger::target_t    cm::logger::err_target = target_t::CONSOLE;

#ifdef _DEBUG
cm::logger::target_t    cm::logger::dbg_target = target_t::CONSOLE;
#else
cm::logger::target_t    cm::logger::dbg_target = target_t::OFF;
#endif

std::ofstream    cm::logger::file;
