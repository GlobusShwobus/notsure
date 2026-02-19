#pragma once

#include "bad_concepts.h"

static constexpr float CORE_EPSILON = 0.0001f;
static constexpr std::size_t CORE_ZERO = 0;

namespace badCore
{
	// UB if invoked with numeric_limits< MIN >, same as STL
	template <typename T>
	constexpr T core_abs(T val)noexcept
		requires MATHEMATICAL_PRIMITIVE<T>
	{
		return val < 0 ? val * -1 : val;
	}

	template <typename T>
	constexpr auto core_max(const T& x, const T& y)noexcept
		requires LESS_THAN_COMPARE<T>
	{
		return (x < y) ? y : x;
	}

	template<typename T>
	constexpr auto core_min(const T& x, const T& y)noexcept
		requires LESS_THAN_COMPARE<T>
	{
		return (x < y) ? x : y;
	}

	//undefined behavior if low is higher than high
	template<typename T>
	constexpr const T& core_clamp(const T& val, const T& low, const T& high)
		requires LESS_THAN_COMPARE<T>
	{
		if (val < low)return low;
		if (high < val)return high;
		return val;
	}

	template<typename T>
	constexpr bool core_is_minus(T x)noexcept
		requires MATHEMATICAL_PRIMITIVE<T>
	{
		return x < 0;
	}

	template<typename T>
	constexpr bool core_is_plus(T x)noexcept
		requires MATHEMATICAL_PRIMITIVE<T>
	{
		return x > 0;
	}

	template<typename T>
	constexpr void core_swap(T& a, T& b)noexcept
		requires std::is_move_constructible_v<T>&& std::is_move_assignable_v<T>
	{
		//NOTE:: may require manual rref casting if something breaks
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	template<typename T>
	constexpr bool core_is_NAN(T x)noexcept
		requires FLOAT_TYPE<T>
	{
		return x != x;
	}

	inline void secure_zero_bytes(void* ptr, std::size_t size) noexcept
	{
		//force the compiler to actually perform the operation via volitile
		volatile unsigned char* p =
			reinterpret_cast<volatile unsigned char*>(ptr);
		while (size--) {
			*p++ = 0;
		}
	}
}