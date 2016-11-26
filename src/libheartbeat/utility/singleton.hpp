/*********************************************************************
 *
 *  @file       singleton.hpp
 *  @brief      Template singleton.
 *
 *
 *  Date        Name          Description
 *  15/04/23    MYLS          Creation.
 *
 *********************************************************************/
#pragma once

namespace cm {
    template<typename T> class singleton;
}

/**
 *  \brief  Template singleton
 *          usage:
 *          class T : public cm::singleton<T> {
 *              private friend class singleton<T>;
 *          ...
 */
template<typename T> class cm::singleton {

public:

    /**
     *  \brief  get the only instance of T
     *  \return    references of T
     */
    static T& Instance();

protected:

    /**
     *  \brief  no copy constructor
     */
    singleton(const singleton &) = delete;

    /**
     *  \brief  no move constructor
     */
    singleton(singleton &&) = delete;

    /**
     *  \brief  Unable to copy
     */
    singleton & operator= (const singleton &) = delete;

    /**
     *  \brief  default constructor
     */
    singleton() = default;

    /**
     *  \brief  virtual destructor
     */
    virtual ~singleton() {};

};


/*********************************************************************
 *  implement
 *********************************************************************/

template <typename T> inline T& cm::singleton<T>::
Instance()
{
    static T instance;
    return instance;

    /* note:
     * [C++ 中多线程与 Singleton 的那些事儿]
     * (http://www.cnblogs.com/liyuan989/p/4264889.html)
     */
}


