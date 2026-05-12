#pragma once

#include "System/StaticString.h"
#include "System/Singleton.h"
#include "System/JsonValueParsers.h"

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
		}
		else {
			return NextNode.FindMethodInfo<TRequestedMethodInfo>(methodName, outMethodInfo);
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

#define METHODNODE_1( methodInfo1Type, method1 ) METHODNODE_TYPE_1(methodInfo1Type)(method1, TNull())
#define METHODNODE_2( methodInfo1Type, method1, methodInfo2Type, method2 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_1(methodInfo2Type)>(method1, METHODNODE_1(methodInfo2Type, method2))
#define METHODNODE_3( methodInfo1Type, method1, methodInfo2Type, method2, methodInfo3Type, method3 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_2(methodInfo2Type, methodInfo3Type)>(method1, METHODNODE_2(methodInfo2Type, method2, methodInfo3Type, method3))
#define METHODNODE_4( methodInfo1Type, method1, methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_3(methodInfo2Type, methodInfo3Type)>(method1, METHODNODE_3(methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4))
#define METHODNODE_5( methodInfo1Type, method1, methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4, methodInfo5Type, method5 ) MethodListNode<methodInfo1Type, METHODNODE_TYPE_4(methodInfo2Type, methodInfo3Type, methodInfo4Type, methodInfo5Type)>(method1, METHODNODE_4(methodInfo2Type, method2, methodInfo3Type, method3, methodInfo4Type, method4, methodInfo5Type, method5))

template<typename TReturn, class TObject, typename... TArgs>
struct MethodInfo {
public:
	using TMethod = TReturn(TObject::*)(TArgs...);

protected:
	StaticString	m_MethodName;
	TMethod			m_Method;

public:
	MethodInfo(StaticString name, TMethod method) : m_MethodName(name), m_Method(method) {}

	DECLARE_GETSET(MethodName)

	template<typename TCaller>
	constexpr Bool CanCall() const {
		return std::same_as<TCaller, TObject> || std::is_base_of<TObject, TCaller>::value;
	}

	template<typename TCaller>
	TReturn Call(TCaller* caller, TArgs... args) const {
		assert(caller);
		assert(m_Method);
		
		if (CanCall<TCaller>()) {
			return  (caller->*m_Method)(args...);
		}
		else {
			return TReturn();
		}
	}
};

#include <tuple>

// Base template
template<typename T>
struct function_traits;

// Specialization for regular functions
template<typename R, typename... TArgs>
struct function_traits<R(TArgs...)> {
	using TReturn = R;
	
	static constexpr std::size_t NumArgs = sizeof...(TArgs);

	// Helper to get a specific argument type by index
	template<std::size_t N>
	using TArg = std::tuple_element_t<N, std::tuple<TArgs...>>;
};

template<typename R, typename C, typename... TArgs>
struct function_traits<R(C::*)(TArgs...)> {
	using TReturn = R;
	static constexpr std::size_t NumArgs = sizeof...(TArgs);

	// Helper to get a specific argument type by index
	template<std::size_t N>
	using TArg = std::tuple_element_t<N, std::tuple<TArgs...>>;

	typedef typename MethodInfo<R, C, TArgs...> TMethodInfo;
};

#define METHOD_INFO_TYPE_FOR( owner, method ) typename function_traits<decltype(&owner::method)>::TMethodInfo

#define CREATE_METHOD_INFO_FOR( owner, method ) METHOD_INFO_TYPE_FOR(owner, method)(#method, &owner::method)

#define REGISTER_METHODS_1( owner, methodListName, method1 ) \
METHODNODE_TYPE_1(METHOD_INFO_TYPE_FOR(owner, method1))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1),								\
		TNull()	\
};

#define REGISTER_METHODS_2( owner, methodListName, method1, method2 ) \
METHODNODE_TYPE_2(METHOD_INFO_TYPE_FOR(owner, method1), METHOD_INFO_TYPE_FOR(owner, method2))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1),								\
		METHODNODE_1(	\
			METHOD_INFO_TYPE_FOR(owner, method2), CREATE_METHOD_INFO_FOR(owner, method2)	\
		)	\
};

#define REGISTER_METHODS_3( owner, methodListName,  method1, method2, method3 ) \
METHODNODE_TYPE_3(METHOD_INFO_TYPE_FOR(owner, method1), METHOD_INFO_TYPE_FOR(owner, method2), METHOD_INFO_TYPE_FOR(owner, method3))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1),								\
		METHODNODE_2(	\
			METHOD_INFO_TYPE_FOR(owner, method2), CREATE_METHOD_INFO_FOR(owner, method2),	\
			METHOD_INFO_TYPE_FOR(owner, method3), CREATE_METHOD_INFO_FOR(owner, method3)	\
		)	\
};

#define REGISTER_METHODS_4( owner, methodListName,  method1, method2, method3, method4 ) \
METHODNODE_TYPE_4(METHOD_INFO_TYPE_FOR(owner, method1), METHOD_INFO_TYPE_FOR(owner, method2), METHOD_INFO_TYPE_FOR(owner, method3), METHOD_INFO_TYPE_FOR(owner, method4))	methodListName = {	\
		CREATE_METHOD_INFO_FOR(owner, method1),								\
		METHODNODE_3(	\
			METHOD_INFO_TYPE_FOR(owner, method2), CREATE_METHOD_INFO_FOR(owner, method2),	\
			METHOD_INFO_TYPE_FOR(owner, method3), CREATE_METHOD_INFO_FOR(owner, method3),	\
			METHOD_INFO_TYPE_FOR(owner, method4), CREATE_METHOD_INFO_FOR(owner, method4)	\
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

class AReflector {
	ABSTRACT_CLASS_TYPEDEFS(AReflector) {}

protected:
	struct MemberInfo {
		UInt64				Offset;
		UInt64				Size;
	};
	Map<StaticString, MemberInfo>	m_MemberInfoMap;

protected:
	template<typename TMember> //TODO: Possibly handle container types and pointers
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