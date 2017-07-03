/***************************************************************************
 *
 *  @file       types.hpp
 *  @brief
 *
 *
 *  Date        Name        Description
 *  16/09/05    MYLS        Creation.
 *
 ***************************************************************************/
#pragma once

#include <cstdint>
#include <map>

/****************************************************************************
 *
 ***************************************************************************/

namespace hb
{

    template<typename Value_T> class RangedValue;

    template<typename Elem_T, uint64_t _Fst, uint64_t... _Rst> class SparseTable;
}

/****************************************************************************
 *  RangedValue
 ***************************************************************************/

template<typename Value_T> class hb::RangedValue
{
public:

    RangedValue(Value_T const & min, Value_T const & max, Value_T const & val = Value_T())
        : val_min(min)
        , val_max(max)
        , val(val)
    {
        if (min > max)
            throw std::invalid_argument("`min` > `max`");
        *this = val;
    }

    Value_T & operator = (Value_T const & rhs)
    {
        if (rhs < val_min)
            val = val_min;
        else if (val_max < rhs)
            val = val_max;
        else
            val = rhs;
        return *this;
    }

    operator Value_T&()
    {
        return val;
    }

    operator Value_T const &() const
    {
        return const_cast<RangedValue &>(*this);
    }

private:
    Value_T val;
    Value_T val_min;
    Value_T val_max;
};


template<typename Elem_T, uint64_t _Fst, uint64_t... _Rst> class hb::SparseTable
{
    typedef uint64_t index_t;

public:

    template<typename... Args_T> Elem_T & operator () (Args_T... args)
    {
        index_t key = index<_Fst, _Rst...>(args...);
        return table[key];
    }

    template<typename... Args_T> Elem_T const & operator () (Args_T... args) const
    {
        index_t key = index<_Fst, _Rst...>(args...);
        auto iter = table.find(key);
        if (iter != table.end()) {
            return *iter;
        } else {
            static const Elem_T default_value;
            return default_value;
        }
    }

    size_t size() const
    {
        return table.size();
    }

    void clear()
    {
        table.clear();
    }

private:

    template<index_t _Fst, index_t... _Rst> struct Mul
    {
    private:
        enum : index_t { LHS = _Fst };
        enum : index_t { RHS = Mul<_Rst...>::Result };
    public:
        enum : index_t { Result = LHS * RHS };
        static_assert(Result / LHS == RHS, "overflow!");
    };

    template<index_t _Fst> struct Mul<_Fst>
    {
        enum : index_t { Result = _Fst };
    };

public:

    enum : index_t { Total = Mul<_Fst, _Rst...>::Result };

private:

    template<index_t _Ary> static index_t inline index_unpack(index_t p, index_t h)
    {
        static_assert(_Ary, "ary should greater than 0!");
        if (h >= _Ary)
            h = _Ary - 1U;// throw std::out_of_range("index out of range.");
        return p * index_t(_Ary) + h;
    }

    template<index_t _Ary, index_t... _R, typename... Args_T> static index_t inline index_unpack(index_t p, index_t h, Args_T... t)
    {
        static_assert(_Ary, "ary should greater than 0!");
        if (h >= _Ary)
            h = _Ary - 1U;// throw std::out_of_range("index out of range.");
        return index_unpack<_R...>(p * index_t(_Ary) + h, t...);
    }

    template<index_t _Ary, index_t... _R, typename... Args_T> static index_t inline index(index_t h, Args_T... t)
    {
        static_assert(sizeof...(Args_T) == sizeof...(_R), "number of params not matched!");
        static_assert(_Ary, "ary should greater than 0!");
        if (h >= _Ary)
            h = _Ary - 1U;// throw std::out_of_range("index out of range.");
        return index_unpack<_R...>(h, t...);
    }

private:

    std::map<index_t, Elem_T> table;
};