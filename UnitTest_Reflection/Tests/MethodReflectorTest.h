#pragma once

#include "System/Reflector.h"

class ParentReflectorTest {
	CLASS_TYPEDEFS(ParentReflectorTest)

	virtual Bool F1(const char* str, UInt32 num) = 0;
};

class MethodReflectorTest : public ParentReflectorTest {
	INHERITED_CLASS_TYPEDEFS(MethodReflectorTest, ParentReflectorTest)
	REFLECTION_HELPERS_FOR(MethodReflectorTest)

protected:
	UInt32 m_Value;

public:
	StaticString ValueName;

	DECLARE_GETSET(Value)

	virtual Bool F1(const char* str, UInt32 num) override {
		assert(str);
		m_Value = num;
		return true;
	}

	Int32 F2() {
		m_Value = 3;
		return -1;
	}
};

class InheritanceTest : public MethodReflectorTest {
	INHERITED_CLASS_TYPEDEFS(InheritanceTest, MethodReflectorTest)

public:
	virtual Bool F1(const char* str, UInt32 num) override {
		auto ret = TSuper::F1(str, num);
		m_Value = num * num;
		return false;
	}
};

template<>
class Reflector<MethodReflectorTest> : public AReflectorJson {
private:
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector) {
		REGISTER_MEMBER(TReflected, m_Value)
		REGISTER_MEMBER(TReflected, ValueName)
	}

public:
	typedef MethodReflectorTest	TReflected;

	DEFINE_METHODINFO_ACCESSORS(m_MethodList)

	//TODO: I wish I could auto-magically generate these signatures instead of having to specify them manually.
	DEFINE_SIGNATURE_ACCESSORS_FOR(Value, Param<UInt32>::Type(TReflected::*)() const, void(TReflected::*)(Param<UInt32>::Type))
	DEFINE_SIGNATURE_ACCESSOR_FOR(F1, Bool(TReflected::*)(const char*, UInt32))
	DEFINE_SIGNATURE_ACCESSOR_FOR(F2, Int32(TReflected::*)())

protected:
	REGISTER_METHODS_4(
		TReflected, m_MethodList,
		SignatureFor_Value<0>, Value,
		SignatureFor_Value<1>, Value,
		SignatureFor_F1, F1,
		SignatureFor_F2, F2
	)
};