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

template<typename TValue>
class ValueParser;

template<>
class ValueParser<float> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef float		TValue;

public:
	static TValue	Get(const rapidjson::Value& value) {
		assert(value.IsDouble());
		return value.GetDouble();
	}

	virtual void Copy(const rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsDouble());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<double> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef double	TValue;

public:
	static TValue	Get(const rapidjson::Value& value) {
		assert(value.IsDouble());
		return value.GetDouble();
	}

	virtual void Copy(const rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsDouble());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<Byte> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef Byte		TValue;

public:
	static TValue	Get(const rapidjson::Value& value) {
		assert(value.IsInt());
		return (TValue)value.GetInt();
	}

	virtual void Copy(const rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsInt());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<Int32> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef Int32	TValue;

public:
	static TValue	Get(const rapidjson::Value& value) {
		assert(value.IsInt());
		return value.GetInt();
	}

	virtual void Copy(const rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsInt());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<UInt32> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef UInt32	TValue;

public:
	static TValue	Get(const rapidjson::Value& value) {
		assert(value.IsInt());
		return (TValue)value.GetInt();
	}

	virtual void Copy(const rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsInt());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<Int64> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef Int64	TValue;

public:
	static TValue	Get(const rapidjson::Value& value) {
		assert(value.IsInt64());
		return value.GetInt64();
	}

	virtual void Copy(const rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsInt64());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
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
	Bool	Set(const StaticString& memberName, TObject* obj, const rapidjson::Value& value) {
		if (!m_MemberInfoMap.Contains(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		void* memberAddr = (void*)((Byte*)obj + info.Offset);
		info.Parser->Copy(value, memberAddr, info.Size);
		return true;
	}

	template<typename TObject, typename TMember>
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

#define ADD_MEMBERINFO( type, member )	\
m_MemberInfoMap.Add(#member, MemberInfo{ (UInt64)offsetof(type, member), sizeof(type::member), Singleton<ValueParser<decltype(type::member)>>::GetInstance() } );

template<typename TClass>
class Reflector;
