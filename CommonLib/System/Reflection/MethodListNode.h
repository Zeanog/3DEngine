#pragma once

#include "System/Reflection/MethodInfo.h"

struct MethodInfoHelpers {
	template<typename TReturn, class TMethodInfo, typename TObject, typename... _TArgs>
	static TReturn Call(const TMethodInfo& info, TObject* caller, _TArgs... args) {
		return info.Call(caller, args...);
	}

	template<typename TReturn, class TMethodInfo, typename TObject, typename... _TArgs>
	static TReturn Call(const TMethodInfo& info, const TObject* caller, _TArgs... args) {
		return info.Call(caller, args...);
	}
};

template< class _TNextNode, class _TMethod >
struct AMethodListNode {
	ABSTRACT_CLASS_TYPEDEFS(AMethodListNode) {}

public:
	using TMethod = _TMethod;
	using TMethodInfo = MethodInfo<TMethod>;
	using TNextNode = _TNextNode;

	TMethodInfo	Info;
	TNextNode	NextNode;

public:
	AMethodListNode(const StaticString& methodName, TMethod method, TNextNode next) : Info(methodName, method), NextNode(next) {}
	AMethodListNode(const Char* methodName, TMethod method, TNextNode&& next) : Info(methodName, method), NextNode(next) {}

	template<typename TRequestedMethodInfo>
	Bool FindMethodInfoAt(int index, TRequestedMethodInfo*& outMethodInfo) const {
		if (std::same_as<TRequestedMethodInfo, TMethodInfo> && index <= 0) {
			outMethodInfo = (TRequestedMethodInfo*)&Info;
			return true;
		}

		//We hit the end of the list without finding the method info
		if constexpr (std::same_as<TNextNode, TNull>) {
			outMethodInfo = nullptr;
			return false;
		}
		else {
			return NextNode.FindMethodInfoAt(index - 1, outMethodInfo);
		}
	}

	template<typename TRequestedMethodInfo>
	Bool FindMethodInfo(const StaticString& methodName, TRequestedMethodInfo*& outMethodInfo) const {
		if (std::same_as<TRequestedMethodInfo, TMethodInfo> && Info.MethodName() == methodName) {
			outMethodInfo = (TRequestedMethodInfo*)&Info;
			return true;
		}

		if constexpr (std::same_as<TNextNode, TNull>) {
			outMethodInfo = nullptr;
			return false;
		}
		else {
			return NextNode.FindMethodInfo(methodName, outMethodInfo);
		}
	}

	template<typename TRequestedMethodInfo>
	TRequestedMethodInfo* FindMethodInfo(const StaticString& methodName) const {
		if (std::same_as<TRequestedMethodInfo, TMethodInfo> && Info.MethodName() == methodName) {
			return (TRequestedMethodInfo*)&Info;
		}

		if constexpr (std::same_as<TNextNode, TNull>) {
			return nullptr;
		}
		else {
			return NextNode.FindMethodInfo<TRequestedMethodInfo>(methodName);
		}
	}

	Bool HasMethod(const StaticString& methodName) const {
		if (Info.MethodName() == methodName) {
			return true;
		}

		if constexpr (std::same_as<TNextNode, TNull>) {
			return false;
		}
		else {
			return NextNode.HasMethod(methodName);
		}
	}

	void EnumerateMethodNames(List<StaticString>& outMethodNames) const {
		outMethodNames.Add(Info.MethodName());
		if constexpr (!std::same_as<TNextNode, TNull>) {
			NextNode.EnumerateMethodNames(outMethodNames);
		}
	}

	UInt32 constexpr NumMethods() const {
		if constexpr (std::same_as<TNextNode, TNull>) {
			return 1;
		}

		return 1 + NextNode.NumMethods();
	}
};

template< class TNextNode, class TMethod >
struct MethodListNode;

template<class TNextNode, typename TReturn, typename TObject, typename... _TArgs>
struct MethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...)> : public AMethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...)> {
	using TAMethodListNode = AMethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...)>;
	INHERITED_CLASS_TYPEDEFS(MethodListNode, TAMethodListNode)

public:
	MethodListNode(const StaticString& methodName, TSuper::TMethod method, TNextNode&& next) : TSuper(methodName, method, next) {}
	MethodListNode(const Char* methodName, TSuper::TMethod method, TNextNode&& next) : TSuper(methodName, method, next) {}

	template<Bool ExplicitArgs, typename... __TArgs>
	static constexpr bool CanCallWith() {
		if constexpr (sizeof...(_TArgs) != sizeof...(__TArgs)) {
			return false;
		}
		else {
			if constexpr (ExplicitArgs) {
				return std::is_same_v<std::tuple<_TArgs...>, std::tuple<__TArgs...>>;
			}
			else {
				return std::is_convertible_v<std::tuple<_TArgs...>, std::tuple<__TArgs...>>;
			}
		}
	}

	template<typename TReturn, typename TCaller, typename ...TArgs>
	TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args) const {
		if (this->Info.MethodName() == methodName) {
			constexpr bool canCall = CanCallWith<false, TArgs...>();
			if constexpr (canCall) {
				return MethodInfoHelpers::Call<TReturn>(this->Info, caller, args...);
			}
			else {
				if constexpr (std::same_as<TNextNode, TNull>) {
#if _DEBUG
					throw std::runtime_error(String::Format("Unable to find method '%s'", this->Info.MethodName().CStr()));
#else
					return TReturn();
#endif
				}
				else {
					return this->NextNode.Call<TReturn>(methodName, caller, args...);
				}
			}
		}
		else {
			if constexpr (std::same_as<TNextNode, TNull>) {
#if _DEBUG
				throw std::runtime_error(String::Format("Unable to find method '%s'", this->Info.MethodName().CStr()));
#else
				return TReturn();
#endif
			}
			else {
				return this->NextNode.Call<TReturn>(methodName, caller, args...);
			}
		}
	}
};

template<class TNextNode, typename TReturn, typename TObject, typename... _TArgs>
struct MethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...) const> : public AMethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...) const> {
	using TAMethodListNode = AMethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...) const>;
	INHERITED_CLASS_TYPEDEFS(MethodListNode, TAMethodListNode)

public:
	MethodListNode(const StaticString& methodName, TSuper::TMethod method, TNextNode&& next) : TSuper(methodName, method, next) {}
	MethodListNode(const Char* methodName, TSuper::TMethod method, TNextNode&& next) : TSuper(methodName, method, next) {}

	template<Bool ExplicitArgs, typename... __TArgs>
	static constexpr bool CanCallWith() {
		if constexpr (sizeof...(_TArgs) != sizeof...(__TArgs)) {
			return false;
		}
		else {
			if constexpr (ExplicitArgs) {
				return std::is_same_v<std::tuple<_TArgs...>, std::tuple<__TArgs...>>;
			}
			else {
				return std::is_convertible_v<std::tuple<_TArgs...>, std::tuple<__TArgs...>>;
			}
		}
	}

	template<typename TReturn, typename TCaller, typename ...TArgs>
	TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args) const {
		if (this->Info.MethodName() == methodName) {
			constexpr bool canCall = CanCallWith<false, TArgs...>();
			if constexpr (canCall) {
				return MethodInfoHelpers::Call<TReturn>(this->Info, caller, args...);
			}
			else {
				if constexpr (std::same_as<TNextNode, TNull>) {
#if _DEBUG
					throw std::runtime_error(String::Format("Unable to find method '%s'", this->Info.MethodName().CStr()));
#else
					return TReturn();
#endif
				}
				else {
					return this->NextNode.Call<TReturn>(methodName, caller, args...);
				}
			}
		}
		else {
			if constexpr (std::same_as<TNextNode, TNull>) {
#if _DEBUG
				throw std::runtime_error(String::Format("Unable to find method '%s'", this->Info.MethodName().CStr()));
#else
				return TReturn();
#endif
			}
			else {
				return this->NextNode.Call<TReturn>(methodName, caller, args...);
			}
		}
	}
};

//TODO: Possilby use std::tuple instead so we can use variadic templates instead of macros.  But this is probably good enough for now.
#define METHODNODE_TYPE_1( method1Type ) MethodListNode<TNull, method1Type>
#define METHODNODE_TYPE_2( method1Type, method2Type ) MethodListNode<METHODNODE_TYPE_1(method2Type), method1Type>
#define METHODNODE_TYPE_3( method1Type, method2Type, method3Type ) MethodListNode<METHODNODE_TYPE_2(method2Type, method3Type), method1Type>
#define METHODNODE_TYPE_4( method1Type, method2Type, method3Type, method4Type ) MethodListNode<METHODNODE_TYPE_3(method2Type, method3Type, method4Type), method1Type>
#define METHODNODE_TYPE_5( method1Type, method2Type, method3Type, method4Type, method5Type ) MethodListNode<METHODNODE_TYPE_4(method2Type, method3Type, method4Type, method5Type), method1Type>
#define METHODNODE_TYPE_6( method1Type, method2Type, method3Type, method4Type, method5Type, method6Type ) MethodListNode<METHODNODE_TYPE_5(method2Type, method3Type, method4Type, method5Type, method6Type), method1Type>

#define METHODNODE_1( owner, method1Type, method1 ) METHODNODE_TYPE_1(method1Type)(#method1, &owner::method1, TNull())
#define METHODNODE_2( owner, method1Type, method1, method2Type, method2 ) MethodListNode<METHODNODE_TYPE_1(method2Type), method1Type>(#method1, &owner::method1, METHODNODE_1(owner, method2Type, method2))
#define METHODNODE_3( owner, method1Type, method1, method2Type, method2, method3Type, method3 ) MethodListNode<METHODNODE_TYPE_2(method2Type, method3Type), method1Type>(#method1, &owner::method1, METHODNODE_2(owner, method2Type, method2, method3Type, method3))
#define METHODNODE_4( owner, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4 ) MethodListNode<METHODNODE_TYPE_3(method2Type, method3Type, method4Type), method1Type>(#method1, &owner::method1, METHODNODE_3(owner, method2Type, method2, method3Type, method3, method4Type, method4))
#define METHODNODE_5( owner, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5 ) MethodListNode<METHODNODE_TYPE_4(method2Type, method3Type, method4Type, method5Type), method1Type>(#method1, &owner::method1, METHODNODE_4(owner, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5))
#define METHODNODE_6( owner, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5, method6Type, method6 ) MethodListNode<METHODNODE_TYPE_5(method2Type, method3Type, method4Type, method5Type, method6Type), method1Type>(#method1, &owner::method1, METHODNODE_5(owner, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5, method6Type, method6))
#define METHOD_INFO_TYPE_FROM( methodType ) MethodInfo<methodType>
#define METHOD_INFO_TYPE_FOR( methodPtr ) METHOD_INFO_TYPE_FROM(decltype(methodPtr))

#define CREATE_METHOD_INFO_FOR( owner, methodType, method ) METHOD_INFO_TYPE_FROM(methodType)(StaticString(#method), (methodType)&owner::method)

#define REGISTER_METHODS_1( owner, methodListName, method1Type, method1 ) \
METHODNODE_TYPE_1(method1Type)	methodListName = {	\
	#method1, &owner::method1, TNull() \
};

#define REGISTER_METHODS_2( owner, methodListName, method1Type, method1, method2Type, method2 ) \
METHODNODE_TYPE_2(method1Type, method2Type)	methodListName = { \
	#method1, &owner::method1, \
	METHODNODE_1( owner, method2Type, method2 )	\
};

#define REGISTER_METHODS_3( owner, methodListName, method1Type, method1, method2Type, method2, method3Type, method3 ) \
METHODNODE_TYPE_3(method1Type, method2Type, method3Type)	methodListName = {	\
		#method1, &owner::method1,								\
		METHODNODE_2( owner, method2Type, method2, method3Type, method3 )	\
};

#define REGISTER_METHODS_4( owner, methodListName, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4 ) \
METHODNODE_TYPE_4(method1Type, method2Type, method3Type, method4Type)	methodListName = {	\
		#method1, &owner::method1,								\
		METHODNODE_3( owner, method2Type, method2, method3Type, method3, method4Type, method4 )	\
};

#define REGISTER_METHODS_5( owner, methodListName, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5 ) \
METHODNODE_TYPE_5(method1Type, method2Type, method3Type, method4Type, method5Type)	methodListName = {	\
		#method1, &owner::method1,								\
		METHODNODE_4( owner, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5 )	\
};

#define REGISTER_METHODS_6( owner, methodListName, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5, method6Type, method6 ) \
METHODNODE_TYPE_6(method1Type, method2Type, method3Type, method4Type, method5Type, method6Type)	methodListName = {	\
		#method1, &owner::method1,								\
		METHODNODE_5( owner, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5, method6Type, method6 )	\
};

#include <tuple>

#define DEFINE_SIGNATURE_ACCESSORS_FOR(methodName, ...) \
using TMethodSignatures_##methodName = std::tuple<__VA_ARGS__>;	\
template<std::size_t N>				\
using SignatureFor_##methodName = typename std::tuple_element_t<N, TMethodSignatures_##methodName>;	\
template<std::size_t N>				\
using MethodInfoFor_##methodName = METHOD_INFO_TYPE_FROM(SignatureFor_##methodName<N>);

#define DEFINE_SIGNATURE_ACCESSOR_FOR(methodName, methodSignature) \
using TMethodSignatures_##methodName = std::tuple<methodSignature>;	\
using SignatureFor_##methodName = typename std::tuple_element_t<0, TMethodSignatures_##methodName>;	\
using MethodInfoFor_##methodName = METHOD_INFO_TYPE_FROM(SignatureFor_##methodName);

#define DEFINE_METHODINFO_ACCESSORS(methodListName) \
template<typename TMethodInfo>				\
TMethodInfo* FindMethodInfo(const StaticString& methodName) const {	\
	return methodListName.FindMethodInfo(methodName);	\
}	\
template<typename TMethodInfo>				\
TMethodInfo* FindMethodInfo(const Char* methodName) const {	\
	return methodListName.FindMethodInfo(StaticString(methodName));	\
}	\
template<typename TMethodInfo>				\
Bool FindMethodInfo(const Char* methodName, TMethodInfo*& outMethodInfo) const { \
	return methodListName.FindMethodInfo(StaticString(methodName), outMethodInfo); \
} \
template<typename TMethodInfo>				\
Bool FindMethodInfo(const StaticString& methodName, TMethodInfo*& outMethodInfo) const { \
	return methodListName.FindMethodInfo(methodName, outMethodInfo); \
} \
Bool HasMethod(const StaticString& methodName) const { \
	return methodListName.HasMethod(methodName);	\
}	\
template<typename TReturn, typename TCaller, typename ...TArgs>		\
TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args) const {	\
	return m_MethodList.Call<TReturn>(methodName, caller, args...);	\
}