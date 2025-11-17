#pragma once

#include "System/StaticString.h"
#include "System/Singleton.h"
#include "System/JsonLoader.h"

class AReflector {
	CLASS_TYPEDEFS(AReflector)

protected:
	struct MemberInfo {
		UInt64				Offset;
		UInt64				Size;
		const IValueParser* Parser;
	};
	Map<StaticString, MemberInfo>	m_MemberInfoMap;

protected:
	AReflector() {}//Disallow instantiation as this is only a partial class

public:
	template<typename TObject, typename TMember>
	Bool	Set(const StaticString& memberName, TObject* obj, const TMember& memberValue) {
		if (!m_MemberInfoMap.Contains(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		assert(sizeof(TMember) == info.Size);
		Byte* objPtr = (Byte*)obj;
		TMember* memberAddr = (TMember*)(objPtr + info.Offset);
		*memberAddr = memberValue;
		return true;
	}

	template<typename TObject, typename TMember>
	Bool	Set(const StaticString& memberName, TObject& obj, const TMember& memberValue) {
		return Set(memberName, &obj, memberValue);
	}

	template<typename TObject>
	Bool	Set(const StaticString& memberName, TObject* obj, const rapidjson::Value& memberValue) {
		if (!m_MemberInfoMap.Contains(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		Byte* objPtr = (Byte*)obj;
		auto memberAddr = objPtr + info.Offset;
		info.Parser->Copy(memberValue, memberAddr, info.Size);
		return true;
	}

	template<typename TObject>
	Bool	Set(const StaticString& memberName, TObject& obj, const rapidjson::Value& memberValue) {
		return Set(memberName, &obj, memberValue);
	}

	template<typename TObject, typename TMember>
	Bool	Get(const StaticString& memberName, TObject* obj, TMember& outMemberValue) {
		if (!m_MemberInfoMap.Contains(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		assert(sizeof(TMember) == info.Size);
		TMember* memberAddr = (TMember*)((Byte*)obj + info.Offset);
		outMemberValue = *memberAddr;

		return true;
	}
};

#include <cstddef>

#define REGISTER_MEMBER( ownerType, member )	\
m_MemberInfoMap.Add(#member, { (UInt64)offsetof(ownerType, member), sizeof(ownerType::member), Singleton<ValueParser<decltype(ownerType::member)>>::GetInstance() } );

template<typename TClass>
class Reflector;
