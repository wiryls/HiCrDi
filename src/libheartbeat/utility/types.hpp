/***************************************************************************
 *	@file		types.hpp
 *	@brief		
 *
 *
 *	Date        Name        Description
 *	09/05/16	MYLS		Creation.
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
	
	template<typename _Value_T> class RangedValue;

	template<typename _Elem_T, uint64_t _Fst, uint64_t... _Rst> class SparseTable;
}

/****************************************************************************
 *	RangedValue
 ***************************************************************************/

template<typename _Value_T> class hb::RangedValue
{
public:

	RangedValue(_Value_T const & min, _Value_T const & max, _Value_T const & val = _Value_T())
		: val_min(min)
		, val_max(max)
		, val(val)
	{
		if (min > max)
			throw std::invalid_argument("`min` > `max`");
		*this = val;
	}

	_Value_T & operator = (_Value_T const & rhs)
	{
		if (rhs < val_min)
			val = val_min;
		else if (val_max < rhs)
			val = val_max;
		else
			val = rhs;
		return *this;
	}

	operator _Value_T&()
	{
		return val;
	}

	operator _Value_T const &() const
	{
		return const_cast<RangedValue &>(*this);
	}

private:
	_Value_T val;
	_Value_T val_min;
	_Value_T val_max;
};


template<typename _Elem_T, uint64_t _Fst, uint64_t... _Rst> class hb::SparseTable
{
	typedef uint64_t index_t;

public:

	template<typename... _Args> _Elem_T & operator () (_Args... args) 
	{
		index_t key = index<_Fst, _Rst...>(args...);
		return table[key];
	}

	template<typename... _Args> _Elem_T const & operator () (_Args... args) const
	{
		index_t key = index<_Fst, _Rst...>(args...);
		auto iter = table.find(key);
		if (iter != table.end()) {
			return *iter;
		} else {
			static const _Elem_T default_value;
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

	template<index_t _Ary, index_t... _R, typename... _Args> static index_t inline index_unpack(index_t p, index_t h, _Args... t)
	{
		static_assert(_Ary, "ary should greater than 0!");
		if (h >= _Ary) 
			h = _Ary - 1U;// throw std::out_of_range("index out of range.");
		return index_unpack<_R...>(p * index_t(_Ary) + h, t...);
	}

	template<index_t _Ary, index_t... _R, typename... _Args> static index_t inline index(index_t h, _Args... t)
	{
		static_assert(sizeof...(_Args) == sizeof...(_R), "number of params not matched!");
		static_assert(_Ary, "ary should greater than 0!");
		if (h >= _Ary) 
			h = _Ary - 1U;// throw std::out_of_range("index out of range.");
		return index_unpack<_R...>(h, t...);
	}

private:

	std::map<index_t, _Elem_T> table;
};