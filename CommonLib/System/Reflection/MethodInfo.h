#pragma once

#include "System/StaticString.h"
#include "System/Functors/FunctionTraits.h"

#include <stdexcept>

template<typename _TMethod>
struct AMethodInfo {
	ABSTRACT_CLASS_TYPEDEFS(AMethodInfo) {}

public:
	using TMethod = _TMethod;

protected:
	StaticString	m_MethodName;
	TMethod			m_Method;

public:
	AMethodInfo(const StaticString& name, TMethod method) : m_MethodName(name), m_Method(method) {}
	AMethodInfo(const Char* name, TMethod method) : m_MethodName(name), m_Method(method) {}

	DECLARE_GETSET(MethodName)

	static constexpr UInt32 NumArgs() {
		return FunctionTraits<TMethod>::NumArgs;
	}
};

template<typename _TMethod>
struct MethodInfo;

template<typename _TReturn, class TObject, typename... _TArgs>
struct MethodInfo<_TReturn(TObject::*)(_TArgs...)> : public AMethodInfo<_TReturn(TObject::*)(_TArgs...)> {
	INHERITED_CLASS_TYPEDEFS(MethodInfo<_TReturn(TObject::*)(_TArgs...)>, AMethodInfo<_TReturn(TObject::*)(_TArgs...)>)

public:
	using TReturn = _TReturn;
	using TArgs = std::tuple<_TArgs...>;
	using TMethod = typename TSuper::TMethod;

public:
	MethodInfo(const StaticString& name, TMethod method) : TSuper(name, method) {}
	MethodInfo(const Char* name, TMethod method) : TSuper(name, method) {}

	template<Bool ExplicitArgs, typename... TPassedInArgs>
	static constexpr Bool CanCallWith() {
		if constexpr (sizeof...(_TArgs) != sizeof...(TPassedInArgs)) {
			return false;
		}
		
		if constexpr (ExplicitArgs) {
			return std::is_same_v<std::tuple<TPassedInArgs...>, TArgs>;
		}
		else {
			//Can we cast from what was passed in to what the method expects
			return std::is_convertible_v<std::tuple<TPassedInArgs...>, TArgs>;
		}
	}

	Bool CanCall(const TObject* caller) const {
		return this->m_Method != nullptr && caller != nullptr;
	}

	TReturn Call(TObject* caller, _TArgs... args) const {
		if (!CanCall(caller)) {
#if _DEBUG
			throw std::runtime_error(String::Format("Invalid caller or method ref for method '%s'", this->m_MethodName.CStr()));
#else
			return TReturn();
#endif		
		}
		return (caller->*(this->m_Method))(args...);
	}
};

template<typename _TReturn, class TObject, typename... _TArgs>
struct MethodInfo<_TReturn(TObject::*)(_TArgs...) const> : public AMethodInfo<_TReturn(TObject::*)(_TArgs...) const> {
	INHERITED_CLASS_TYPEDEFS(MethodInfo<_TReturn(TObject::*)(_TArgs...) const>, AMethodInfo<_TReturn(TObject::*)(_TArgs...) const>)

public:
	using TReturn = _TReturn;
	using TArgs = std::tuple<_TArgs...>;
	using TMethod = typename TSuper::TMethod;

public:
	MethodInfo(const StaticString& name, TMethod method) : TSuper(name, method) {}
	MethodInfo(const Char* name, TMethod method) : TSuper(name, method) {}

	template<Bool ExplicitArgs, typename... TPassedInArgs>
	static constexpr bool CanCallWith() {
		if constexpr (sizeof...(_TArgs) != sizeof...(TPassedInArgs)) {
			return false;
		}
		
		if constexpr (ExplicitArgs) {
			return std::is_same_v<std::tuple<TPassedInArgs...>, TArgs>;
		}
		else {
			//Can we cast from what was passed in to what the method expects
			return std::is_convertible_v<std::tuple<TPassedInArgs...>, TArgs>;
		}
	}

	Bool CanCall(const TObject* caller) const {
		return this->m_Method != nullptr && caller != nullptr;
	}

	TReturn Call(const TObject* caller, _TArgs... args) const {
		if (!CanCall(caller)) {
#if _DEBUG
			throw std::runtime_error(String::Format("Invalid caller or method ref for method '%s'", this->m_MethodName.CStr()));
#else
			return TReturn();
#endif		
		}
		return (caller->*(this->m_Method))(args...);
	}
};