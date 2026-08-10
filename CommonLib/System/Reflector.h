#pragma once

#include "System/StaticString.h"
#include "System/Singleton.h"
#include "System/Functors/TypeList.h"
#include "System/Map.h"
#include "System/List.h"

#include "System/Functors/FunctionTraits.h"

template<typename TObject>
class Reflector;

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

#include <type_traits>

template<typename _TMethod>
struct MethodInfo;

#define ARGS( methodInfoType )	std::invoke_result_t<decltype(&methodInfoType::GetArgs)>

template<typename _TReturn, class TObject, typename... _TArgs>
struct MethodInfo<_TReturn(TObject::*)(_TArgs...)> : public AMethodInfo<_TReturn(TObject::*)(_TArgs...)> {
	INHERITED_CLASS_TYPEDEFS(MethodInfo<_TReturn(TObject::*)(_TArgs...)>, AMethodInfo<_TReturn(TObject::*)(_TArgs...)>)

public:
	using TReturn = _TReturn;
	using TArgs = std::tuple<_TArgs...>;
	using TMethod = typename TSuper::TMethod;	

	static std::tuple<_TArgs...> GetArgs() {
		throw _Notvalid_impl_;
	}

public:
	MethodInfo(const StaticString& name, TMethod method) : TSuper(name, method) {}
	MethodInfo(const Char* name, TMethod method) : TSuper(name, method) {}

	template<typename... __TArgs>
	static constexpr bool CanCallWith() {
		if constexpr (sizeof...(_TArgs) != sizeof...(__TArgs)) {
			return false;
		}
		else {
			return std::is_constructible_v<std::tuple<_TArgs...>, std::tuple<__TArgs...>>;
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

		//assert(this->ArgsMatch<TArgs...>());

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

	static std::tuple<_TArgs...> GetArgs() {
		throw _Notvalid_impl_;
	}

public:
	MethodInfo(const StaticString& name, TMethod method) : TSuper(name, method) {}
	MethodInfo(const Char* name, TMethod method) : TSuper(name, method) {}

	template<typename... __TArgs>
	static constexpr bool CanCallWith() {
		if constexpr (sizeof...(_TArgs) != sizeof...(__TArgs)) {
			return false;
		}
		else {
			return std::is_convertible_v<std::tuple<_TArgs...>, std::tuple<__TArgs...>>;
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
	AMethodListNode(const StaticString& methodName, TMethod method, TNextNode next) : Info(methodName, method) , NextNode(next){}
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
struct MethodListNode<TNextNode, TReturn (TObject::*)(_TArgs...)> : public AMethodListNode<TNextNode, TReturn (TObject::*)(_TArgs...)> {
	using TAMethodListNode = AMethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...)>;
	INHERITED_CLASS_TYPEDEFS(MethodListNode, TAMethodListNode)

public:
	MethodListNode(const StaticString& methodName, TSuper::TMethod method, TNextNode&& next) : TSuper(methodName, method, next) {}
	MethodListNode(const Char* methodName, TSuper::TMethod method, TNextNode&& next) : TSuper(methodName, method, next) {}

	template<typename... __TArgs>
	static constexpr bool CanCallWith() {
		if constexpr (sizeof...(_TArgs) != sizeof...(__TArgs)) {
			return false;
		}
		else {
			return std::is_convertible_v<std::tuple<_TArgs...>, std::tuple<__TArgs...>>;
		}
	}

	template<typename TReturn, typename TCaller, typename ...TArgs>
	TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args) const {
		if (this->Info.MethodName() == methodName) {
			constexpr bool canCall = CanCallWith<TArgs...>();
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
					return TSuper::NextNode.Call<TReturn>(methodName, caller, args...);
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
struct MethodListNode<TNextNode, TReturn (TObject::*)(_TArgs...) const> : public AMethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...) const> {
	using TAMethodListNode = AMethodListNode<TNextNode, TReturn(TObject::*)(_TArgs...) const>;
	INHERITED_CLASS_TYPEDEFS(MethodListNode, TAMethodListNode)

public:
	MethodListNode(const StaticString& methodName, TSuper::TMethod method, TNextNode&& next) : TSuper(methodName, method, next) {}
	MethodListNode(const Char* methodName, TSuper::TMethod method, TNextNode&& next) : TSuper(methodName, method, next) {}

	template<typename... __TArgs>
	static constexpr bool CanCallWith() {
		if constexpr (sizeof...(_TArgs) != sizeof...(__TArgs)) {
			return false;
		}
		else {
			return std::is_convertible_v<std::tuple<_TArgs...>, std::tuple<__TArgs...>>;
		}
	}

	template<typename TReturn, typename TCaller, typename ...TArgs>
	TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args) const {
		if (this->Info.MethodName() == methodName) {
			constexpr bool canCall = CanCallWith<TArgs...>();
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
template<UInt32 Index, typename TReturn, typename TCaller, typename ...TArgs>	\
TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args) const {	\
	return m_MethodList.Call<Index, TReturn, TCaller, TArgs...>(methodName, caller, args...);	\
}														\
template<typename TReturn, typename TCaller, typename ...TArgs>		\
TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args) const {	\
	return m_MethodList.Call<TReturn, TCaller, TArgs...>(methodName, caller, args...);	\
}

class AReflector {
	ABSTRACT_CLASS_TYPEDEFS(AReflector) {}

protected:
	struct MemberInfo {
		UInt64				Offset;
		UInt64				Size;
	};
	Map<StaticString, MemberInfo>	m_MemberInfoMap;

protected:
	template<typename TMember> //TODO: Verify we handle container types and pointers
	void	RegisterMember(const StaticString& memberName, UInt64 offset) {
		m_MemberInfoMap.Add(memberName, { offset, sizeof(TMember) });
	}

public:
	Bool	HasMember(const StaticString& memberName) const {
		return m_MemberInfoMap.Contains(memberName);
	}

	template<typename TObject, typename TMember>
	Bool	Set(const StaticString& memberName, TObject* obj, const TMember& memberValue) {
		if (!HasMember(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		assert(sizeof(TMember) == info.Size);
		TMember* memberAddr = (TMember*)((Byte*)obj + info.Offset);
		*memberAddr = memberValue;
		return true;
	}

	template<typename TObject, typename TMember>
	Bool	Set(const StaticString& memberName, TObject& obj, const TMember& memberValue) {
		return Set(memberName, &obj, memberValue);
	}

	template<typename TObject, typename TMember>
	Bool	Get(const StaticString& memberName, TObject* obj, TMember& outMemberValue) {
		if (!HasMember(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		assert(sizeof(TMember) == info.Size);
		TMember* memberAddr = (TMember*)((Byte*)obj + info.Offset);
		outMemberValue = *memberAddr;

		return true;
	}
};

#include "System/JsonValueParsers.h"

class AReflectorJson : public AReflector {
	INHERITED_CLASS_TYPEDEFS(AReflectorJson, AReflector)

protected:
	Map<StaticString, const IValueParser*>	m_ParserMap;

protected:
	AReflectorJson() {}

	template<typename TMember>
	void	RegisterMember(const StaticString& memberName, UInt64 offset) {
		TSuper::RegisterMember<TMember>(memberName, offset);
		m_ParserMap.Add(memberName, Singleton<ValueParser<TMember>>::GetInstance());
	}

public:
	template<typename TObject>
	Bool	Set(const StaticString& memberName, TObject* obj, const rapidjson::Value& memberValue) {
		if (!HasMember(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		auto memberAddr = (Byte*)obj + info.Offset;
		auto parser = m_ParserMap[memberName];
		parser->Set(memberValue, memberAddr, info.Size);
		return true;
	}

	template<typename TObject>
	Bool	Set(const StaticString& memberName, TObject& obj, const rapidjson::Value& memberValue) {
		return Set(memberName, &obj, memberValue);
	}

	template<typename TObject>
	Bool	Get(const StaticString& memberName, TObject* obj, rapidjson::Value& outMemberValue) {
		if (!HasMember(memberName)) {
			return false;
		}
		auto& info = m_MemberInfoMap[memberName];
		auto memberAddr = (Byte*)obj + info.Offset;
		auto parser = m_ParserMap[memberName];
		parser->Set(memberAddr, outMemberValue);
		return true;
	}

	template<typename TObject>
	void Get(const rapidjson::Value& value, TObject& outValue) const {
		auto reflector = Singleton<Reflector<TObject>>::GetInstance();
		FOREACH_MEMBER(memberIter, value) {
			reflector->Set(memberIter->name.GetString(), outValue, memberIter->value);
		}
	}
};

#include <cstddef>

#define REFLECTION_HELPERS_FOR(ownerType)	friend class Reflector<ownerType>;
#define REGISTER_MEMBER( ownerType, member ) RegisterMember<decltype(ownerType::member)>(#member, (UInt64)offsetof(ownerType, member) );