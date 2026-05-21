#pragma once

#include "System/Reflector.h"

class MethodReflectorTest {
	CLASS_TYPEDEFS(MethodReflectorTest)

protected:
	UInt32 m_Value;

public:
	DECLARE_GETSET(Value)

	//TODO: Make a typelist of method types per method name.
	void Value() {
		m_Value = 1;
	}

	void Value(UInt32 ui, Bool b) {
		m_Value = ui;
	}

	Bool F1(const char* str, UInt32 num) {
		assert(str);
		m_Value = num;
		return true;
	}

	Int32 F2() {
		m_Value = 3;
		return -1;
	}
};

#include "System/Functors/TypeList.h"

template<>
class Reflector<MethodReflectorTest> : public AReflectorJson {
private:
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector) {}

public:
	DEFINE_METHODINFO_ACCESSORS(m_MethodList)

	typedef MethodReflectorTest	TReflected;

	//I wish I could auto-magically determine these
	using TMethodSignatures_Value = TYPELIST_2( void(TReflected::*)(), void(TReflected::*)(UInt32, Bool) );
	DEFINE_SIGNATURE_ACCESSORS_FOR(Value)

	using TMethodSignatures_F1 = TYPELIST_1( Bool(TReflected::*)(const char*, UInt32) );
	DEFINE_SIGNATURE_ACCESSOR_FOR(F1)

	using TMethodSignatures_F2 = TYPELIST_1( Int32(TReflected::*)() );
	DEFINE_SIGNATURE_ACCESSOR_FOR(F2)

protected:
	REGISTER_METHODS_4(
		MethodReflectorTest, m_MethodList,
		SignatureFor_Value<0>, Value,
		SignatureFor_Value<1>, Value,
		SignatureFor_F1, F1,
		SignatureFor_F2, F2
	)
};