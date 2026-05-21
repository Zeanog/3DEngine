#pragma once

#include "System/StaticString.h"
#include "System/Singleton.h"
#include "System/Functors/TypeList.h"
#include "System/Map.h"

template<typename TObject>
class Reflector;

template< class TMethodInfo, class TNextNode >
struct MethodListNode {	
	TMethodInfo	MethodInfo;
	TNextNode	NextNode;

	MethodListNode(TMethodInfo info, TNextNode&& next) : MethodInfo(info), NextNode(next) {
	}

	template<typename TRequestedMethodInfo>
	Bool FindMethodInfoAt(int index, TRequestedMethodInfo*& outMethodInfo) const {
		if (index <= 0 && std::same_as<TRequestedMethodInfo, TMethodInfo>) {
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
	Bool FindMethodInfo(StaticString methodName, TRequestedMethodInfo*& outMethodInfo) const {
		if (MethodInfo.MethodName() == methodName && std::same_as<TRequestedMethodInfo, TMethodInfo>) {
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
	TRequestedMethodInfo* FindMethodInfo(StaticString methodName) const {
		if (MethodInfo.MethodName() == methodName && std::same_as<TRequestedMethodInfo, TMethodInfo>) {
			return (TRequestedMethodInfo*)&MethodInfo;
		}

		if constexpr (std::same_as<TNextNode, TNull>) {
			return nullptr;
		} else {
			return NextNode.FindMethodInfo<TRequestedMethodInfo>(methodName);
		}
	}

	Bool HasMethod(StaticString methodName) const {
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
};

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

#include <stdexcept>

template<typename TReturn, class TObject, typename... TArgs>
struct MethodInfo {
public:
	using TMethod = TReturn(TObject::*)(TArgs...);

protected:
	StaticString	m_MethodName;
	TMethod			m_Method;

public:
	MethodInfo(const StaticString& name, TMethod method) : m_MethodName(name), m_Method(method) {}
	MethodInfo(const Char* name, TMethod method) : m_MethodName(name), m_Method(method) {}

	DECLARE_GETSET(MethodName)

	template<typename TCaller>
	static constexpr bool CanCall() {//TODO: TEST THIS!  Especially is_base_of use.  TCaller can be a parent class of TObject, but not a child class.
		return std::same_as<TCaller, TObject> || std::is_base_of<TCaller, TObject>::value;
	}

	template<typename TCaller>
	Bool CanCall( TCaller* caller ) const {
		return CanCall<TCaller>() && caller != nullptr;
	}

	template<typename TCaller>
	TReturn Call(TCaller* caller, TArgs... args) {
		if(!caller || !m_Method) {
#if _DEBUG
			throw std::runtime_error(String::Format("Invalid caller or method for method '%s'", m_MethodName.CStr()));
#else
			return TReturn();
#endif		
		}
		
		if constexpr (!CanCall<TCaller>()) {
#if _DEBUG
			throw std::runtime_error(String::Format("Invalid caller type for method '%s'", m_MethodName.CStr()));
#else
			return TReturn();
#endif
		}
		else {
			return (caller->*m_Method)(args...);
		}
	}
};

#include "System/Functors/FunctionTraits.h"

#define METHOD_INFO_TYPE_FOR( methodPtr ) typename FunctionTraits<decltype(methodPtr)>::TMethodInfo
#define METHOD_INFO_TYPE_FROM( methodType ) typename FunctionTraits<methodType>::TMethodInfo

#define CREATE_METHOD_INFO_FOR( owner, methodType, method ) METHOD_INFO_TYPE_FROM(methodType)(StaticString(#method), (methodType)&owner::method)

#define REGISTER_METHODS_1( owner, methodListName, method1Type, method1 ) \
METHODNODE_TYPE_1(METHOD_INFO_TYPE_FOR(&owner::method1))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1Type, method1),								\
		TNull()	\
};

#define REGISTER_METHODS_2( owner, methodListName, method1, method2 ) \
METHODNODE_TYPE_2(METHOD_INFO_TYPE_FOR(&owner::method1), METHOD_INFO_TYPE_FOR(owner, method2))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1),								\
		METHODNODE_1(	\
			METHOD_INFO_TYPE_FOR(&owner::method2), CREATE_METHOD_INFO_FOR(owner, method2)	\
		)	\
};

#define REGISTER_METHODS_3( owner, methodListName, method1, method2, method3 ) \
METHODNODE_TYPE_3(METHOD_INFO_TYPE_FOR(&owner::method1), METHOD_INFO_TYPE_FOR(&owner::method2), METHOD_INFO_TYPE_FOR(&owner::method3))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1),								\
		METHODNODE_2(	\
			METHOD_INFO_TYPE_FOR(&owner::method2), CREATE_METHOD_INFO_FOR(owner, method2),	\
			METHOD_INFO_TYPE_FOR(&owner::method3), CREATE_METHOD_INFO_FOR(owner, method3)	\
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

#define REGISTER_METHODS_5( owner, methodListName,  method1, method2, method3, method4, method5 ) \
METHODNODE_TYPE_5(METHOD_INFO_TYPE_FOR(owner, method1), METHOD_INFO_TYPE_FOR(owner, method2), METHOD_INFO_TYPE_FOR(owner, method3), METHOD_INFO_TYPE_FOR(owner, method4), METHOD_INFO_TYPE_FOR(owner, method5))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1),								\
		METHODNODE_4(	\
			METHOD_INFO_TYPE_FOR(owner, method2), CREATE_METHOD_INFO_FOR(owner, method2),	\
			METHOD_INFO_TYPE_FOR(owner, method3), CREATE_METHOD_INFO_FOR(owner, method3),	\
			METHOD_INFO_TYPE_FOR(owner, method4), CREATE_METHOD_INFO_FOR(owner, method4),	\
			METHOD_INFO_TYPE_FOR(owner, method5), CREATE_METHOD_INFO_FOR(owner, method5)	\
		)	\
};

#define REGISTER_METHODS_6( owner, methodListName,  method1, method2, method3, method4, method5, method6 ) \
METHODNODE_TYPE_6(METHOD_INFO_TYPE_FOR(owner, method1), METHOD_INFO_TYPE_FOR(owner, method2), METHOD_INFO_TYPE_FOR(owner, method3), METHOD_INFO_TYPE_FOR(owner, method4), METHOD_INFO_TYPE_FOR(owner, method5), METHOD_INFO_TYPE_FOR(owner, method6))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1),								\
		METHODNODE_5(	\
			METHOD_INFO_TYPE_FOR(owner, method2), CREATE_METHOD_INFO_FOR(owner, method2),	\
			METHOD_INFO_TYPE_FOR(owner, method3), CREATE_METHOD_INFO_FOR(owner, method3),	\
			METHOD_INFO_TYPE_FOR(owner, method4), CREATE_METHOD_INFO_FOR(owner, method4),	\
			METHOD_INFO_TYPE_FOR(owner, method5), CREATE_METHOD_INFO_FOR(owner, method5),	\
			METHOD_INFO_TYPE_FOR(owner, method6), CREATE_METHOD_INFO_FOR(owner, method6)	\
		)	\
};

#define DEFINE_SIGNATURE_ACCESSORS_FOR(methodName) \
template<std::size_t N>				\
using SignatureFor_##methodName = typename TypeAt<TMethodSignatures_##methodName, N>::Result;	\
template<std::size_t N>				\
using MethodInfoFor_##methodName = METHOD_INFO_TYPE_FROM(SignatureFor_##methodName<N>);

#define DEFINE_SIGNATURE_ACCESSOR_FOR(methodName) \
using SignatureFor_##methodName = typename TypeAt<TMethodSignatures_##methodName, 0>::Result;	\
using MethodInfoFor_##methodName = METHOD_INFO_TYPE_FROM(SignatureFor_##methodName);

#define DEFINE_METHODINFO_ACCESSORS(methodListName) \
template<typename TMethodInfo>				\
TMethodInfo* FindMethodInfo(const StaticString& methodName) const {	\
	return methodListName.FindMethodInfo(methodName);	\
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
template<typename TMethod>	\
using TMethodInfoFor = METHOD_INFO_TYPE_FROM(TMethod);

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

#define REGISTER_MEMBER( ownerType, member ) RegisterMember<decltype(ownerType::member)>(#member, (UInt64)offsetof(ownerType, member) );