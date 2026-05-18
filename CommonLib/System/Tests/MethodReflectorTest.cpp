#include "MethodReflectorTest.h"

void Func() {
	METHOD_INFO_TYPE_FROM(Reflector<MethodReflectorTest>::SignatureForF1)* methodInfo;
	if (Singleton< Reflector<MethodReflectorTest>>::GetInstance()->FindMethodInfo("F1", methodInfo)) {
		methodInfo->Call((MethodReflectorTest*)nullptr, "test", 5);
	}
}