#pragma once

#include "System/StaticString.h"
#include "System/Singleton.h"
#include "System/Functors/TypeList.h"
#include "System/Map.h"
#include "System/List.h"

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
};

template<typename _TMethod>
struct MethodInfo;

template<typename _TReturn, class TObject, typename... TArgs>
struct MethodInfo<_TReturn(TObject::*)(TArgs...)> : public AMethodInfo<_TReturn(TObject::*)(TArgs...)> {
	INHERITED_CLASS_TYPEDEFS(MethodInfo<_TReturn(TObject::*)(TArgs...)>, AMethodInfo<_TReturn(TObject::*)(TArgs...)>)

protected:
	std::function<_TReturn(TObject*, TArgs...)> m_FuncWrapper;

public:
	using TReturn = _TReturn;
	using TMethod = typename TSuper::TMethod;

public:
	MethodInfo(const StaticString& name, TMethod method) : TSuper(name, method) {}
	MethodInfo(const Char* name, TMethod method) : TSuper(name, method) {}

	Bool CanCall(TObject* caller) const {
		return this->m_Method != nullptr && caller != nullptr;
	}

	TReturn Call(TObject* caller, TArgs... args) const {
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

template<typename _TReturn, class TObject, typename... TArgs>
struct MethodInfo<_TReturn(TObject::*)(TArgs...) const> : public AMethodInfo<_TReturn(TObject::*)(TArgs...) const> {
	INHERITED_CLASS_TYPEDEFS(MethodInfo<_TReturn(TObject::*)(TArgs...) const>, AMethodInfo<_TReturn(TObject::*)(TArgs...) const>)

public:
	using TReturn = _TReturn;
	using TMethod = typename TSuper::TMethod;

public:
	MethodInfo(const StaticString& name, TMethod method) : TSuper(name, method) {}
	MethodInfo(const Char* name, TMethod method) : TSuper(name, method) {}

	Bool CanCall(TObject* caller) const {
		return this->m_Method != nullptr && caller != nullptr;
	}

	TReturn Call(TObject* caller, TArgs... args) const {
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

template< class TMethodInfo, class TNextNode >
struct MethodListNode {
	TMethodInfo	MethodInfo;
	TNextNode	NextNode;

	MethodListNode(TMethodInfo info, TNextNode&& next) : MethodInfo(info), NextNode(next) {
	}

	template<typename TRequestedMethodInfo>
	Bool FindMethodInfoAt(int index, TRequestedMethodInfo*& outMethodInfo) const {
		if (std::same_as<TRequestedMethodInfo, TMethodInfo> && index <= 0) {
			outMethodInfo = (TRequestedMethodInfo*)&MethodInfo;
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
		if (std::same_as<TRequestedMethodInfo, TMethodInfo> && MethodInfo.MethodName() == methodName) {
			outMethodInfo = (TRequestedMethodInfo*)&MethodInfo;
			return true;
		}

		if constexpr (std::same_as<TNextNode, TNull>) {
			outMethodInfo = nullptr;
			return false;
		} else {
			return NextNode.FindMethodInfo(methodName, outMethodInfo);
		}
	}

	template<typename TRequestedMethodInfo>
	TRequestedMethodInfo* FindMethodInfo(const StaticString& methodName) const {
		if (std::same_as<TRequestedMethodInfo, TMethodInfo> && MethodInfo.MethodName() == methodName) {
			return (TRequestedMethodInfo*)&MethodInfo;
		}

		if constexpr (std::same_as<TNextNode, TNull>) {
			return nullptr;
		} else {
			return NextNode.FindMethodInfo<TRequestedMethodInfo>(methodName);
		}
	}

	template<UInt32 Index, typename TReturn, typename TCaller, typename ...TArgs>
	TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args ) const {
		if (MethodInfo.MethodName() != methodName) {
			if constexpr (std::same_as<TNextNode, TNull>) {
				return TReturn();//TODO: Throw exception
			}
			else {
				return NextNode.Call<Index, TReturn, TCaller, TArgs...>(methodName, caller, args...);
			}
		}

		if constexpr (Index <= 0) {
			if constexpr (std::same_as<typename TMethodInfo::TReturn, TReturn>) {
				return MethodInfo.Call(caller, args...);
			}
			else {
				assert(false);//TODO: Throw exception
				return TReturn();
			}
		}

		if constexpr (std::same_as<TNextNode, TNull>) {
			return TReturn();//TODO: Throw exception
		}
		else {
			return NextNode.Call<Index - 1, TReturn, TCaller, TArgs...>(methodName, caller, args...);
		}
	}

	/*template<typename TReturn, typename TCaller, typename ...TArgs>
	TReturn Call(const StaticString& methodName, TCaller* caller, TArgs... args) const {
		return Call<0, TReturn>(methodName, caller, args...);
	}*/

	Bool HasMethod(const StaticString& methodName) const {
		if (MethodInfo.MethodName() == methodName) {
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
		outMethodNames.Add(MethodInfo.MethodName());
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

//TODO: Possilby use std::tuple instead so we can use variadic templates instead of macros.  But this is probably good enough for now.
#define METHODNODE_TYPE_1( methodInfo1Type ) MethodListNode<methodInfo1Type, TNull>
#define METHODNODE_TYPE_2( methodInfo1Type, methodInfo2Type ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_1(methodInfo2Type)>
#define METHODNODE_TYPE_3( methodInfo1Type, methodInfo2Type, methodInfo3Type ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_2(methodInfo2Type, methodInfo3Type)>
#define METHODNODE_TYPE_4( methodInfo1Type, methodInfo2Type, methodInfo3Type, methodInfo4Type ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_3(methodInfo2Type, methodInfo3Type, methodInfo4Type)>
#define METHODNODE_TYPE_5( methodInfo1Type, methodInfo2Type, methodInfo3Type, methodInfo4Type, methodInfo5Type ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_4(methodInfo2Type, methodInfo3Type, methodInfo4Type, methodInfo5Type)>
#define METHODNODE_TYPE_6( methodInfo1Type, methodInfo2Type, methodInfo3Type, methodInfo4Type, methodInfo5Type, methodInfo6Type ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_5(methodInfo2Type, methodInfo3Type, methodInfo4Type, methodInfo5Type, methodInfo6Type)>

#define METHODNODE_1( methodInfo1Type, method1 ) METHODNODE_TYPE_1(methodInfo1Type)(method1, TNull())
#define METHODNODE_2( methodInfo1Type, method1, methodInfo2Type, method2 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_1(methodInfo2Type)>(method1, METHODNODE_1(methodInfo2Type, method2))
#define METHODNODE_3( methodInfo1Type, method1, methodInfo2Type, method2, methodInfo3Type, method3 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_2(methodInfo2Type, methodInfo3Type)>(method1, METHODNODE_2(methodInfo2Type, method2, methodInfo3Type, method3))
#define METHODNODE_4( methodInfo1Type, method1, methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_3(methodInfo2Type, methodInfo3Type)>(method1, METHODNODE_3(methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4))
#define METHODNODE_5( methodInfo1Type, method1, methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4, methodInfo5Type, method5 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_4(methodInfo2Type, methodInfo3Type, methodInfo4Type, methodInfo5Type)>(method1, METHODNODE_4(methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4, methodInfo5Type, method5))
#define METHODNODE_6( methodInfo1Type, method1, methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4, methodInfo5Type, method5, methodInfo6Type, method6 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_5(methodInfo2Type, methodInfo3Type, methodInfo4Type, methodInfo5Type, methodInfo6Type)>(method1, METHODNODE_5(methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4, methodInfo5Type, method5, methodInfo6Type, method6))

#include "System/Functors/FunctionTraits.h"

#define METHOD_INFO_TYPE_FOR( methodPtr ) typename FunctionTraits<decltype(methodPtr)>::TMethodInfo
#define METHOD_INFO_TYPE_FROM( methodType ) typename FunctionTraits<methodType>::TMethodInfo

#define CREATE_METHOD_INFO_FOR( owner, methodType, method ) METHOD_INFO_TYPE_FROM(methodType)(StaticString(#method), (methodType)&owner::method)

#define REGISTER_METHODS_1( owner, methodListName, method1Type, method1 ) \
METHODNODE_TYPE_1(METHOD_INFO_TYPE_FOR(&owner::method1))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1Type, method1),								\
		TNull()	\
};

#define REGISTER_METHODS_2( owner, methodListName, method1Type, method1, method2Type, method2 ) \
METHODNODE_TYPE_2(METHOD_INFO_TYPE_FROM(method1Type), METHOD_INFO_TYPE_FROM(method2Type))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1Type, method1),								\
		METHODNODE_1(	\
			METHOD_INFO_TYPE_FROM(method2Type), CREATE_METHOD_INFO_FOR(owner, method2Type, method2)	\
		)	\
};

#define REGISTER_METHODS_3( owner, methodListName, method1Type, method1, method2Type, method2, method3Type, method3 ) \
METHODNODE_TYPE_3(METHOD_INFO_TYPE_FROM(method1Type), METHOD_INFO_TYPE_FROM(method2Type), METHOD_INFO_TYPE_FROM(method3Type))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1Type, method1),								\
		METHODNODE_2(	\
			METHOD_INFO_TYPE_FROM(method2Type), CREATE_METHOD_INFO_FOR(owner, method2Type, method2),	\
			METHOD_INFO_TYPE_FROM(method3Type), CREATE_METHOD_INFO_FOR(owner, method3Type, method3)	\
		)	\
};

#define REGISTER_METHODS_4( owner, methodListName, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4 ) \
METHODNODE_TYPE_4(METHOD_INFO_TYPE_FROM(method1Type), METHOD_INFO_TYPE_FROM(method2Type), METHOD_INFO_TYPE_FROM(method3Type), METHOD_INFO_TYPE_FROM(method4Type))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1Type, method1),								\
		METHODNODE_3(	\
			METHOD_INFO_TYPE_FROM(method2Type), CREATE_METHOD_INFO_FOR(owner, method2Type, method2),	\
			METHOD_INFO_TYPE_FROM(method3Type), CREATE_METHOD_INFO_FOR(owner, method3Type, method3),	\
			METHOD_INFO_TYPE_FROM(method4Type), CREATE_METHOD_INFO_FOR(owner, method4Type, method4)	\
		)	\
};

#define REGISTER_METHODS_5( owner, methodListName, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5 ) \
METHODNODE_TYPE_5(METHOD_INFO_TYPE_FROM(method1Type), METHOD_INFO_TYPE_FROM(method2Type), METHOD_INFO_TYPE_FROM(method3Type), METHOD_INFO_TYPE_FROM(method4Type), METHOD_INFO_TYPE_FROM(method5Type))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1Type, method1),								\
		METHODNODE_4(	\
			METHOD_INFO_TYPE_FROM(method2Type), CREATE_METHOD_INFO_FOR(owner, method2Type, method2),	\
			METHOD_INFO_TYPE_FROM(method3Type), CREATE_METHOD_INFO_FOR(owner, method3Type, method3),	\
			METHOD_INFO_TYPE_FROM(method4Type), CREATE_METHOD_INFO_FOR(owner, method4Type, method4),	\
			METHOD_INFO_TYPE_FROM(method5Type), CREATE_METHOD_INFO_FOR(owner, method5Type, method5)	\
		)	\
};

#define REGISTER_METHODS_6( owner, methodListName, method1Type, method1, method2Type, method2, method3Type, method3, method4Type, method4, method5Type, method5, method6Type, method6 ) \
METHODNODE_TYPE_6(METHOD_INFO_TYPE_FROM(method1Type), METHOD_INFO_TYPE_FROM(method2Type), METHOD_INFO_TYPE_FROM(method3Type), METHOD_INFO_TYPE_FROM(method4Type), METHOD_INFO_TYPE_FROM(method5Type), METHOD_INFO_TYPE_FROM(method6Type))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1Type, method1),								\
		METHODNODE_5(	\
			METHOD_INFO_TYPE_FROM(method2Type), CREATE_METHOD_INFO_FOR(owner, method2Type, method2),	\
			METHOD_INFO_TYPE_FROM(method3Type), CREATE_METHOD_INFO_FOR(owner, method3Type, method3),	\
			METHOD_INFO_TYPE_FROM(method4Type), CREATE_METHOD_INFO_FOR(owner, method4Type, method4),	\
			METHOD_INFO_TYPE_FROM(method5Type), CREATE_METHOD_INFO_FOR(owner, method5Type, method5),	\
			METHOD_INFO_TYPE_FROM(method6Type), CREATE_METHOD_INFO_FOR(owner, method6Type, method6)	\
		)	\
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
	return m_MethodList.Call<0, TReturn, TCaller, TArgs...>(methodName, caller, args...);	\
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