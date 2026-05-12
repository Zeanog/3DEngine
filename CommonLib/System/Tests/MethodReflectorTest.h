#pragma once

#include "System/Reflector.h"

class MethodReflectorTest {
	CLASS_TYPEDEFS(MethodReflectorTest)

protected:
	UInt32 m_Value;

public:
	DECLARE_GETSET(Value)

	void F() {
		m_Value = 1;
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

//TODO: Add Support for getters and setters with similar names but different signatures (e.g. Value() and Value(UInt32 value)) and possibly properties with the same name as well (e.g. Value member and Value() method)
template<>
class Reflector<MethodReflectorTest> : public AReflectorJson {
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector)
	{
		METHOD_INFO_TYPE_FOR(TReflected, F1)* methodInfo;
		if (FindMethodInfo("F1", methodInfo)) {
			methodInfo->Call((TReflected*)nullptr, "Hello", 5);
		}
	}

public:
	typedef MethodReflectorTest	TReflected;

	template<typename TMethodInfo>
	Bool FindMethodInfo(const StaticString& methodName, TMethodInfo*& outMethodInfo) const {
		return m_MethodList.FindMethodInfo(methodName, outMethodInfo);
	}

	Bool HasMethod(const StaticString& methodName) const {
		return m_MethodList.HasMethod(methodName);
	}

protected:
	REGISTER_METHODS_3(TReflected, m_MethodList, F, F1, F2)
};