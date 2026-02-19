#pragma once
#include <concepts>

namespace badCore
{
	template <typename T, typename... Ts>
	concept ONE_OF = (std::same_as<T, Ts> || ...);

	template <typename T>
	concept BY_VALUE_TYPE = std::same_as<T, std::remove_cvref_t<T>>;

	template<typename T>
	concept PRIMITIVE_TYPE = ONE_OF<std::remove_cvref_t<T>, bool, char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double>;

	template <typename T>
	concept MATHEMATICAL_PRIMITIVE = ONE_OF<std::remove_cvref_t<T>, char, short, int, long, long long, float, double, long double>;

	template <typename T>
	concept INTEGER_TYPE = ONE_OF<std::remove_cvref_t<T>, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

	template <typename T>
	concept FLOAT_TYPE = ONE_OF<std::remove_cvref_t<T>, float, double, long double>;

	template <typename T>
	concept VECTOR_TYPE = ONE_OF<std::remove_cvref_t<T>, short, int, long, long long, float, double, long double>&& BY_VALUE_TYPE<T>;


	template<typename T>
	concept LESS_THAN_COMPARE = requires (const T & x, const T & y) {
		{ x < y }->std::convertible_to<bool>;
	};

	template <typename T>
	concept IS_SEQUENCE_COMPATIBLE = std::destructible<T> && std::is_nothrow_move_constructible_v<T> && BY_VALUE_TYPE<T>;

	template<typename T>
	concept IS_SLLIST_COMPATIBLE = std::destructible<T> && BY_VALUE_TYPE<T>;
}