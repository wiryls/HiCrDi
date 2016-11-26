/****************************************************************************
 *
 *  @file       ibeat.hpp
 *  @brief
 *
 *
 *  Date        Name        Description
 *  16/09/05    MYLS        Creation.
 *  16/11/20    MYLS        change to a new one.
 *
 ***************************************************************************/
#pragma once

namespace hb
{
    template<typename Beat_T> class IBeat;
}

/****************************************************************************
 *  declaration
 ***************************************************************************/

/**
 *  \brief  TODO:
 */
template<typename Beat_T> class hb::IBeat
{
public:

    typedef Beat_T Self;

public:

    enum struct status_t
    {
        uninitialized,
        ready,
        error,
    };

public:

    virtual operator status_t () const = 0;

    virtual void     clear    ()       = 0;

public:

    /**
     *  \brief virtual destructor
     */
    virtual ~IBeat() = 0;

};


/****************************************************************************
 *  implementation
 ***************************************************************************/

template<class Beat_T> inline hb::IBeat<Beat_T>::~IBeat() {}
