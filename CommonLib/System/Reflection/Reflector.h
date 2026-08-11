#pragma once

#include "System/StaticString.h"
#include "System/Singleton.h"
#include "System/Map.h"

template<typename TObject>
class Reflector;

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