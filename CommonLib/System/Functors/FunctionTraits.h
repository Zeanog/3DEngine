#pragma once

#include <tuple>

// Base template
template<typename TFunc>
struct FunctionTraits;

// Specialization for regular functions
template<typename R, typename... TArgs>
struct FunctionTraits<R(TArgs...)> {
	using TReturn = R;
	static constexpr std::size_t NumArgs = sizeof...(TArgs);

	// Helper to get a specific argument type by index
	template<std::size_t N>
	using TArg = std::tuple_element_t<N, std::tuple<TArgs...>>;
};

template<typename R, typename C, typename... TArgs>
struct FunctionTraits<R(C::*)(TArgs...)> {
	using TReturn = R;
	static constexpr std::size_t NumArgs = sizeof...(TArgs);

	// Helper to get a specific argument type by index
	template<std::size_t N>
	using TArg = std::tuple_element_t<N, std::tuple<TArgs...>>;

	typedef typename MethodInfo<R, C, TArgs...> TMethodInfo;
};