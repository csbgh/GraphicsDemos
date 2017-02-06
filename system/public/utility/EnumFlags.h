#ifndef _DS_ENUM_FLAGS_H
#define _DS_ENUM_FLAGS_H

// #include "DemoTypes.h"

template <typename T>
static bool CheckFlags(T a, T b)
{
	return CheckFlags(a, b, std::integral_constant<bool, std::is_enum<T>::value>());
}

template<typename T>
inline static bool CheckFlags(T a, T b, std::false_type)
{
	return (a & b) == b;
}

template<typename T>
static bool CheckFlags(T a, T b, std::true_type)
{
	return (static_cast<typename std::underlying_type<T>::type>(a)
			& static_cast<typename std::underlying_type<T>::type>(b))
			== static_cast<typename std::underlying_type<T>::type>(b);
}

template<typename E>
constexpr auto ToIntegral(E e) -> typename std::underlying_type<E>::type
{
	return static_cast<typename std::underlying_type<E>::type>(e);
}

// E : Enum
#define ENUM_FLAGS(E)																										\
	using T = std::underlying_type<E>::type;																				\
	inline E	operator |	(E lhs, E rhs)	{ return static_cast<E>(static_cast<T>(lhs) | static_cast<T>(rhs)); }			\
	inline E	operator &	(E lhs, E rhs)	{ return static_cast<E>(static_cast<T>(lhs) & static_cast<T>(rhs)); }			\
	inline E	operator ^	(E lhs, E rhs)	{ return static_cast<E>(static_cast<T>(lhs) ^ static_cast<T>(rhs)); }			\
	inline E	operator ~	(E lhs)			{ return static_cast<E>(~static_cast<T>(lhs));						}			\
	inline E&	operator |=	(E& lhs, E rhs)	{ lhs = lhs | rhs; return lhs; }												\
	inline E&	operator &=	(E& lhs, E rhs)	{ lhs = lhs & rhs; return lhs; }												\
	inline E&	operator ^=	(E& lhs, E rhs)	{ lhs = lhs ^ rhs; return lhs; }

#endif // _DS_ENUM_FLAGS_H