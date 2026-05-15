#pragma once

#include "System/Reflector.h"

class MethodReflectorTest {
	CLASS_TYPEDEFS(MethodReflectorTest)

protected:
	UInt32 m_Value;

public:
	DECLARE_GETSET(Value)

	//TODO: Make a typelist of method types per method name.
	void F() {
		m_Value = 1;
	}

	void F(UInt32 ui) {
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

//TODO: Add Support for getters and setters with similar names but different signatures (e.g. Value() and Value(UInt32 value)) and possibly properties with the same name as well (e.g. Value member and Value() method)
template<>
class Reflector<MethodReflectorTest> : public AReflectorJson {
private:
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector)
	{
		/*METHOD_INFO_TYPE_FROM(TFMethodSignature2)* methodInfo;
		if (FindMethodInfo("F", methodInfo)) {
			methodInfo->Call((TReflected*)nullptr, 5);
		}*/
	}

public:
	template<typename TMethodInfo>
	Bool FindMethodInfo(const StaticString& methodName, TMethodInfo*& outMethodInfo) const {
		return m_MethodList.FindMethodInfo(methodName, outMethodInfo);
	}

	Bool HasMethod(const StaticString& methodName) const {
		return m_MethodList.HasMethod(methodName);
	}

	typedef MethodReflectorTest	TReflected;
	using TFMethodSignature1 = void(TReflected::*)();
	using TFMethodSignature2 = void(TReflected::*)(UInt32);
	using TF1MethodSignature = Bool(TReflected::*)(const char*, UInt32);
	using TF2MethodSignature = Int32(TReflected::*)();

protected:
	REGISTER_METHODS_4(TReflected, m_MethodList, TFMethodSignature1, F, TFMethodSignature2, F, TF1MethodSignature, F1, TF2MethodSignature, F2)
};