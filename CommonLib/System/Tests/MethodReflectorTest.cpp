#include "MethodReflectorTest.h"

void Func() {
	Reflector<MethodReflectorTest>::MethodInfoSignatureForF1* methodInfo;
	if (Singleton<Reflector<MethodReflectorTest>>::GetInstance()->FindMethodInfo("F1", methodInfo)) {
		auto ret = methodInfo->Call((Byte*)nullptr, "test", 5);
	}
}