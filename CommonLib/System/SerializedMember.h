#pragma once

#include "System\JsonSerializer.h"
#include "System\List.h"
#include "rapidjson/Document.h"

class ASerializedMember {
	CLASS_TYPEDEFS(ASerializedMember);

protected:
	String	m_Key;

public:
	const String& Key() const {
		return m_Key;
	}

	void	Key(const String& key) {
		m_Key = key;
	}

	void	Key(const Char* key) {
		m_Key = key;
	}

	ASerializedMember() {
	}

	ASerializedMember(const Char* key) {
		m_Key = key;
	}

	ASerializedMember(const String& key) {
		m_Key = key;
	}

	virtual Bool	Matches(const String& key) const {
		return !String::StrICmp(m_Key.CStr(), key.CStr());
	}

	virtual Bool	Matches(const Char* key) const {
		return !String::StrICmp(m_Key.CStr(), key);
	}

	virtual void	Read(const Value& val) = 0;
};

template<typename TValue>
class SerializedMember : public ASerializedMember {
	INHERITEDCLASS_TYPEDEFS(SerializedMember, ASerializedMember);

protected:
	TValue	m_Value;

public:
	TValue&	Value() {
		return m_Value;
	}

	const TValue&	Value() const {
		return m_Value;
	}

	SerializedMember() {
	}

	SerializedMember(const Char* key) : TSuper(key) {
	}

	SerializedMember(const String& key) : TSuper(key) {
	}

	virtual void	Read(const rapidjson::Value& val) {
		this->m_Value = JsonSerializer<TValue>::ReadFrom(val);
	}
};

template<typename _TValue>
class SerializedMember<_TValue*> : public ASerializedMember {
	INHERITEDCLASS_TYPEDEFS(SerializedMember, ASerializedMember);

public:
	typedef _TValue*	TValue;

protected:
	TValue	m_Value;

public:
	TValue&	Value() {
		return m_Value;
	}

	const TValue&	Value() const {
		return m_Value;
	}

	virtual ~SerializedMember() {
		DeletePtr(m_Value);
		m_Value = NULL;
	}

	SerializedMember() {
	}

	SerializedMember(const Char* key) : TSuper(key) {
	}

	SerializedMember(const String& key) : TSuper(key) {
	}

	virtual void	Read(const rapidjson::Value& val) {
		this->m_Value = JsonSerializer<TValue>::ReadFrom(val);
	}
};

template<typename _TValue>
class SerializedMember<List<_TValue*>> : public ASerializedMember {
	INHERITEDCLASS_TYPEDEFS(SerializedMember, ASerializedMember);

public:
	typedef List<_TValue*>	TValue;

protected:
	TValue	m_Value;

public:
	TValue&	Value() {
		return m_Value;
	}

	const TValue&	Value() const {
		return m_Value;
	}

	virtual ~SerializedMember() {
		DeleteContents(m_Value);
		m_Value.Clear();
	}

	SerializedMember() {
	}

	SerializedMember(const Char* key) : TSuper(key) {
	}

	SerializedMember(const String& key) : TSuper(key) {
	}

	virtual void	Read(const rapidjson::Value& val) {
		this->m_Value = JsonSerializer<TValue>::ReadFrom(val);
	}
};

class SerializedMemberManager {
protected:
	List<ASerializedMember*>	m_List;

public:
	void	Register(ASerializedMember* mem) {
		m_List.Add(mem);
	}

	void	Unregister(ASerializedMember* mem) {
		m_List.Remove(mem);
	}

	/*void	Deserialize(const Value::Object& obj) {
		ASerializedMember* sm = NULL;

		for (Value::Object::MemberIterator objMember = obj.MemberBegin(); objMember != obj.MemberEnd(); ++objMember) {
			FOREACH(iter, m_List) {
				sm = (*iter);
				if(sm->Matches(objMember->name.GetString())) {
					sm->Read(objMember->value);
				}
			}

		}
	}*/

	void	Deserialize(const Value::ConstObject& obj) {
		ASerializedMember* sm = NULL;

		const Char* memName = NULL;
		//for (Value::Object::ConstMemberIterator objMember = obj.MemberBegin(); objMember != obj.MemberEnd(); ++objMember) {
		FOREACH(iter, m_List) {
			sm = (*iter);
			//memName = objMember->name.GetString();
			/*if (sm->Matches(memName)) {
				sm->Read(objMember->value);
				break;
			}*/
			if (obj.HasMember(sm->Key().CStr())) {
				sm->Read(obj[sm->Key().CStr()]);
			}
		}
		//}
	}
};

#define DECLARE_SERIALIZEDMEMBER( type, name ) \
protected:								\
SerializedMember<type>	m_##name;		\
public:									\
	const type&	name() const {			\
		return m_##name.Value();			\
	}

#define REGISTER_MEMBER( key, member ) \
member.Key(key);				\
m_SerializationManager.Register((ASerializedMember*)&member);

#define DECLARE_SERIALIZERS( type ) \
template<>							\
class JsonSerializer<type> {		\
public:								\
	static type	ReadFrom(const Value& jsonVal) {	\
		assert(jsonVal.IsObject());				\
		return type(jsonVal);			\
	}										\
	static void	ReadFrom(const Value& jsonVal, type& outVal) {	\
		outVal = ReadFrom(jsonVal);									\
	}															\
};													\
template<>											\
class JsonSerializer<type*> {						\
public:																\
	static type*	ReadFrom(const Value& jsonVal) {		\
		assert(jsonVal.IsObject());											\
		return new type(jsonVal);									\
	}																		\
	static void	ReadFrom(const Value& jsonVal, type*& outVal) {		\
		outVal = ReadFrom(jsonVal);									\
	}																		\
};