#pragma once

#include <tuple>

// Base template
template<typename TFunc>
struct FunctionTraits;

// Specialization for regular functions
template<typename R, typename... _TArgs>
struct FunctionTraits<R(_TArgs...)> {
	using TReturn = R;
	using TArgs = std::tuple<_TArgs...>;
	static constexpr std::size_t NumArgs = sizeof...(_TArgs);

	template<std::size_t N>
	using TArg = std::tuple_element_t<N, TArgs>;
};

template<typename R, typename C, typename... _TArgs>
struct FunctionTraits<R(C::*)(_TArgs...)> {
	using TReturn = R;
	using TArgs = std::tuple<_TArgs...>;
	static constexpr std::size_t NumArgs = sizeof...(_TArgs);

	template<std::size_t N>
	using TArg = std::tuple_element_t<N, TArgs>;
	/*typedef typename MethodInfo<R(C::*)(TArgs...)> TMethodInfo;*/
};

template<typename R, typename C, typename... _TArgs>
struct FunctionTraits<R(C::*)(_TArgs...) const> {
	using TReturn = R;
	using TArgs = std::tuple<_TArgs...>;
	static constexpr std::size_t NumArgs = sizeof...(_TArgs);

	template<std::size_t N>
	using TArg = std::tuple_element_t<N, TArgs>;
	/*typedef typename MethodInfo<R(C::*)(TArgs...) const> TMethodInfo;*/
};