#pragma once

#include "Map.h"
#include "StaticString.h"
#include "System\Singleton.h"

#include "rapidjson\document.h"

class IValueParser {
	CLASS_TYPEDEFS(IValueParser)

public:
	virtual void Copy(const rapidjson::Value& src, void* dest, UInt64 size) = 0;
};

template<typename _TValue>
class AValueParser : public IValueParser {
public:
	typedef _TValue		TValue;

public:
	virtual TValue	Get(const rapidjson::Value& value) = 0;

	virtual void Copy(const rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<typename TValue>
class ValueParser;

template<>
class ValueParser<float> : public AValueParser<float> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual TValue	Get(const rapidjson::Value& value) override {
		assert(value.IsDouble());
		return value.GetDouble();
	}
};

template<>
class ValueParser<double> : public AValueParser<double> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue	TValue;

public:
	virtual TValue	Get(const rapidjson::Value& value) override {
		assert(value.IsDouble());
		return value.GetDouble();
	}
};

template<>
class ValueParser<Byte> : public AValueParser<Byte> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual TValue	Get(const rapidjson::Value& value) override {
		assert(value.IsInt());
		return (TValue)value.GetInt();
	}
};

template<>
class ValueParser<Int32> : public AValueParser<Int32> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue	TValue;

public:
	virtual TValue	Get(const rapidjson::Value& value) override {
		assert(value.IsInt());
		return value.GetInt();
	}
};

template<>
class ValueParser<UInt32> : public AValueParser<UInt32> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue	TValue;

public:
	virtual TValue	Get(const rapidjson::Value& value) override {
		assert(value.IsInt());
		return (TValue)value.GetInt();
	}
};

template<>
class ValueParser<Int64> : public AValueParser<Int64> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue	TValue;

public:
	virtual TValue	Get(const rapidjson::Value& value) override {
		assert(value.IsInt64());
		return value.GetInt64();
	}
};

class AReflector {
	CLASS_TYPEDEFS( AReflector )

protected:
	//TODO: Think of ways to handle compound members like pointers, lists, custom data structures
	struct MemberInfo {
		UInt64		Offset;
		UInt64		Size;
		IValueParser* Parser;
	};

	Map<StaticString, MemberInfo>			m_MemberInfoMap;

protected:
	AReflector() {}//Disallow instantiation as this is only a partial class

public:
	template<typename TObject, typename TMember>
	Bool	Set(const StaticString& memberName, TObject* obj, const TMember& memberValue) {
		if (!m_MemberInfoMap.Contains(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		TMember* memberAddr = (TMember*)((Byte*)obj + info.Offset);
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
		void* memberAddr = (void*)((Byte*)obj + info.Offset);
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
		TMember* memberAddr = (TMember*)((Byte*)obj + info.Offset);
		outMemberValue = *memberAddr;

		return true;
	}
};

#include <cstddef>

#define REGISTER_MEMBER( type, member )	\
m_MemberInfoMap.Add(#member, MemberInfo{ (UInt64)offsetof(type, member), sizeof(type::member), Singleton<ValueParser<decltype(type::member)>>::GetInstance() } );

template<typename TClass>
class Reflector;
