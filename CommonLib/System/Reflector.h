#pragma once

#include "Map.h"
#include "StaticString.h"
#include "System\Singleton.h"

#include "rapidjson\document.h"

class IValueParser {
	CLASS_TYPEDEFS(IValueParser)

public:
	virtual void Copy(rapidjson::Value& src, void* dest, UInt64 size) = 0;
};

template<typename TValue>
class ValueParser;

template<>
class ValueParser<float> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(ValueParser<float>) {
	}

public:
	typedef float		TValue;

public:
	static TValue	Get(rapidjson::Value& value) {
		assert(value.IsDouble());
		return value.GetDouble();
	}

	virtual void Copy(rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsDouble());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<double> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(ValueParser<double>) {
	}

public:
	typedef double	TValue;

public:
	static TValue	Get(rapidjson::Value& value) {
		assert(value.IsDouble());
		return value.GetDouble();
	}

	virtual void Copy(rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsDouble());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<Byte> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(ValueParser<Byte>) {
	}

public:
	typedef Byte		TValue;

public:
	static TValue	Get(rapidjson::Value& value) {
		assert(value.IsInt());
		return (TValue)value.GetInt();
	}

	virtual void Copy(rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsInt());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<Int32> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(ValueParser<Int32>) {
	}

public:
	typedef Int32	TValue;

public:
	static TValue	Get(rapidjson::Value& value) {
		assert(value.IsInt());
		return value.GetInt();
	}

	virtual void Copy(rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsInt());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<UInt32> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(ValueParser<UInt32>) {
	}

public:
	typedef UInt32	TValue;

public:
	static TValue	Get(rapidjson::Value& value) {
		assert(value.IsInt());
		return (TValue)value.GetInt();
	}

	virtual void Copy(rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsInt());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<>
class ValueParser<Int64> : public IValueParser {
	INHERITEDCLASS_TYPEDEFS(ValueParser, IValueParser)
	SINGLETON_DECLARATIONS(ValueParser<Int64>) {
	}

public:
	typedef Int64	TValue;

public:
	static TValue	Get(rapidjson::Value& value) {
		assert(value.IsInt64());
		return value.GetInt64();
	}

	virtual void Copy(rapidjson::Value& src, void* dest, UInt64 size) override {
		assert(src.IsInt64());
		assert(sizeof(TValue) == size);
		TValue val = Get(src);
		memcpy_s(dest, size, &val, size);
	}
};

template<typename _TMemberList>
class IReflector {
public:
	typedef	_TMemberList	TMemberList;
};

class AReflector {
	CLASS_TYPEDEFS( AReflector )

protected:
	struct MemberInfo {
		UInt64		Offset;
		UInt64		Size;
		IValueParser* Parser;
	};

	Map<StaticString, MemberInfo>			m_MemberInfoMap;

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

	template<typename TObject>
	Bool	Set(const StaticString& memberName, TObject* obj, rapidjson::Value& value) {
		if (!m_MemberInfoMap.Contains(memberName)) {
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		void* memberAddr = (void*)((Byte*)obj + info.Offset);
		info.Parser->Copy(value, memberAddr, info.Size);
		return true;
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

	/*template<typename TObject>
	Bool	Get(const StaticString& memberName, TObject& obj, void*& outValue) {
		if (!m_MemberOffsets.Contains(memberName)) {
			return false;
		}

		UInt64 offset = m_MemberOffsets[memberName];
		void* memberAddr = (void*)((&obj) + offset);

		auto size = m_MemberSizes[memberName];
		memcpy_s(outValue , size, memberAddr, size);

		return true;
	}*/
};

#include <cstddef>

#define ADD_MEMBERINFO( type, member )	\
m_MemberInfoMap.Add(#member, MemberInfo{ (UInt64)offsetof(type, member), sizeof(type::member), Singleton<ValueParser<decltype(type::member)>>::GetInstance() } );	\

template<typename TClass>
class Reflector;
