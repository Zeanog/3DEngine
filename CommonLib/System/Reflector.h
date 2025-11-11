#pragma once

#include "Map.h"
#include "List.h"
#include "StaticString.h"
#include "System\Singleton.h"
#include <rapidjson/document.h>

template<>
class ContainerIterator<rapidjson::Value> {
public:
	typedef typename rapidjson::Value	TContainer;

	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;

	typedef typename TUndecorated::ValueIterator	Iterator;
	typedef typename TUndecorated::ConstValueIterator	ConstIterator;

	typedef typename TUndecorated::ValueIterator	ReverseIterator;
	typedef typename TUndecorated::ConstValueIterator ConstReverseIterator;

	static Iterator	Begin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstIterator	Begin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static Iterator	End(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstIterator	End(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}

	static ReverseIterator	ReverseBegin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ReverseIterator	ReverseEnd(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
};

template<>
class ContainerIterator<rapidjson::Value&> {
public:
	typedef typename rapidjson::Value	TContainer;

	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;

	typedef typename TUndecorated::ValueIterator	Iterator;
	typedef typename TUndecorated::ConstValueIterator	ConstIterator;

	typedef typename TUndecorated::ValueIterator	ReverseIterator;
	typedef typename TUndecorated::ConstValueIterator ConstReverseIterator;

	static Iterator	Begin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstIterator	Begin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static Iterator	End(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstIterator	End(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}

	static ReverseIterator	ReverseBegin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ReverseIterator	ReverseEnd(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
};

template<>
class ContainerIterator<const rapidjson::Value&> {
public:
	typedef typename rapidjson::Value	TContainer;

	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;

	typedef typename TUndecorated::ValueIterator	Iterator;
	typedef typename TUndecorated::ConstValueIterator	ConstIterator;

	typedef typename TUndecorated::ValueIterator	ReverseIterator;
	typedef typename TUndecorated::ConstValueIterator ConstReverseIterator;

	static Iterator	Begin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstIterator	Begin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static Iterator	End(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstIterator	End(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}

	static ReverseIterator	ReverseBegin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ReverseIterator	ReverseEnd(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
};

template<>
class ContainerIterator<rapidjson::Document> {
public:
	typedef typename rapidjson::Document	TContainer;

	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;

	typedef typename TUndecorated::ValueIterator	Iterator;
	typedef typename TUndecorated::ConstValueIterator	ConstIterator;

	typedef typename TUndecorated::ValueIterator	ReverseIterator;
	typedef typename TUndecorated::ConstValueIterator ConstReverseIterator;

	static Iterator	Begin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstIterator	Begin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static Iterator	End(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstIterator	End(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}

	static ReverseIterator	ReverseBegin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ReverseIterator	ReverseEnd(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
};

template<>
class ContainerIterator<rapidjson::Document&> {
public:
	typedef typename rapidjson::Document	TContainer;

	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;

	typedef typename TUndecorated::ValueIterator	Iterator;
	typedef typename TUndecorated::ConstValueIterator	ConstIterator;

	typedef typename TUndecorated::ValueIterator	ReverseIterator;
	typedef typename TUndecorated::ConstValueIterator ConstReverseIterator;

	static Iterator	Begin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstIterator	Begin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static Iterator	End(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstIterator	End(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}

	static ReverseIterator	ReverseBegin(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.Begin();
	}
	static ReverseIterator	ReverseEnd(typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		assert(container.IsArray());
		return container.End();
	}
};

#define FOREACH_MEMBER( iterName, jsonValue )	\
for (auto iterName = jsonValue.MemberBegin(), iterName##End = jsonValue.MemberEnd(); iterName != iterName##End; ++iterName)

class IValueParser {
	CLASS_TYPEDEFS(IValueParser)

public:
	virtual void Copy(const rapidjson::Value& src, Byte* dest, UInt64 size) const = 0;
};

template<typename _TValue>
class AValueParser : public IValueParser {
public:
	typedef _TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const = 0;

	virtual void Copy(const rapidjson::Value& src, Byte* dest, UInt64 size) const override {
		assert(sizeof(TValue) == size);
		TValue val{};
		Get(src, val);
		(*(TValue*)dest) = val;
		//TODO: Known Issue - If this is an object that contains a pointer(aka List, etc...) this may cause some memory issues.
	}
};

template<typename TValue>
class ValueParser;

//TODO: TEST!!!!!
template<typename _TElemValue>
class ValueParser<List<_TElemValue>> : public AValueParser<List<_TElemValue>> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;//This is actually List<_TValue>

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		auto parser = Singleton<ValueParser<_TElemValue>>::GetInstance();
		FOREACH( iter, value ) {
			_TElemValue elem{};
			parser->Get(*iter, elem);
			outValue.Add(elem);
		}
	}

	virtual void Copy(const rapidjson::Value& src, Byte* dest, UInt64 size) const override {
		assert(dest);
		//assert(sizeof(TValue) == size);
		//TODO: Possibly use size to validate
		TValue& list = *(TValue*)dest;
		Get(src, list);
	}
};

template<typename _TMapValue>
class ValueParser<Map<StaticString, _TMapValue>> : public AValueParser<Map<StaticString, _TMapValue>> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		auto parser = Singleton<ValueParser<_TMapValue>>::GetInstance();

		assert(value.IsObject());
		FOREACH_MEMBER(iter, value) {
			_TValue elem{};
			parser->Get(iter->value, elem);
			outValue.Add(iter->name.GetString(), elem);
		}
	}

	virtual void Copy(const rapidjson::Value& src, Byte* dest, UInt64 size) const override {
		assert(dest);
		//assert(sizeof(TValue) == size);
		//TODO: Possibly use size
		//TValue& list = *(TValue*)dest;
		//Get(src, list);
	}
};
//TODO: TEST!!!!!

template<>
class ValueParser<Float32> : public AValueParser<Float32> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsNumber());
		if(value.IsInt()) {
			outValue = (TValue)value.GetInt();
		}
		else if(value.IsUint()) {
			outValue = (TValue)value.GetUint();
		}
		/*else if (value.IsInt64()) {
			outValue = (TValue)value.GetInt64();
		}
		else if (value.IsUint64()) {
			outValue = (TValue)value.GetUint64();
		}*/
		else if (value.IsDouble()) {
			outValue = (TValue)value.GetDouble();
		}
		else {
			assert(0);//Unknown number type
		}
	}
};

template<>
class ValueParser<Float64> : public AValueParser<Float64> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue	TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsNumber());
		if (value.IsInt()) {
			outValue = (TValue)value.GetInt();
		}
		else if (value.IsUint()) {
			outValue = (TValue)value.GetUint();
		}
		/*else if (value.IsInt64()) {
			outValue = (TValue)value.GetInt64();
		}
		else if (value.IsUint64()) {
			outValue = (TValue)value.GetUint64();
		}*/
		else if (value.IsDouble()) {
			outValue = (TValue)value.GetDouble();
		}
		else {
			assert(0);//Unknown number type
		}
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
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsUint());
		outValue = (TValue)value.GetUint();
	}
};

template<>
class ValueParser<Bool> : public AValueParser<Bool> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
		SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsBool());
		outValue = value.GetBool();
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
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsInt());
		outValue = value.GetInt();
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
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsUint());
		outValue = (TValue)value.GetUint();
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
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsInt64());
		outValue = value.GetInt64();
	}
};

template<>
class ValueParser<UInt64> : public AValueParser<UInt64> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
		SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue	TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsUint64());
		outValue = (TValue)value.GetUint64();
	}
};

template<>
class ValueParser<String> : public AValueParser<String> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
	SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsString());
		outValue = value.GetString();
	}
};

template<>
class ValueParser<StaticString> : public AValueParser<StaticString> {
	INHERITEDCLASS_TYPEDEFS(ValueParser, AValueParser)
		SINGLETON_DECLARATIONS(TSelf) {
	}

public:
	typedef TSuper::TValue		TValue;

public:
	virtual void Get(const rapidjson::Value& value, TValue& outValue) const override {
		assert(value.IsString());
		outValue = value.GetString();
	}
};

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
		if (!m_MemberInfoMap.Contains(memberName)) {//Ignore unknown members
			return false;
		}

		auto& info = m_MemberInfoMap[memberName];
		Byte* objPtr = (Byte*)obj;
		auto memberAddr = objPtr + info.Offset;
		assert(info.Offset == (memberAddr - objPtr));
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
