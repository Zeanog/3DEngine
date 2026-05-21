#include "pch.h"
#include "CppUnitTest.h"
#include "Tests/MethodReflectorTest.h"
#include "System/String.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestReflection
{
	TEST_CLASS(UnitTestReflection)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			using TReflector = Reflector<MethodReflectorTest>;
			TReflector::MethodInfoFor_Value<1>* methodInfo;
			const Char* methodName = "Value";
			if (!Singleton<TReflector>::GetInstance()->FindMethodInfo(methodName, methodInfo)) {
				auto formattedString = String::Format("Failed to find method info for %s", methodName);
				size_t formattedStringLen = String::Length(formattedString);
				auto wcStr = STACK_ALLOC(wchar_t, (formattedStringLen*sizeof(wchar_t)));
				size_t convertedChars = 0;

				// Convert mbStr to wcStr
				// 100 is the size of the destination buffer
				// _TRUNCATE allows the function to copy as much as fits
				errno_t err = mbstowcs_s(&convertedChars, wcStr, formattedStringLen * 3, formattedString, _TRUNCATE);
				Assert::Fail(wcStr);
			}

			MethodReflectorTest testObj;
			methodInfo->Call(&testObj, 5, true);
		}

		TEST_METHOD(TestMethod2)
		{
			using TReflector = Reflector<MethodReflectorTest>;
			TReflector::MethodInfoFor_Value<1>* methodInfo;
			const Char* methodName = "F1";
			if (!Singleton<TReflector>::GetInstance()->FindMethodInfo(methodName, methodInfo)) {
				auto formattedString = String::Format("Failed to find method info for %s", methodName);
				String::ConvertFor(formattedString, Assert::Fail);	
			}

			try {
				MethodReflectorTest testObj;
				methodInfo->Call(&testObj, 5, true);
			}
			catch (std::runtime_error error) {
				auto formattedString = String::Format("Exception thrown while calling '%s' through reflector", methodName);
				String::ConvertFor(formattedString, Assert::Fail);
			}
		}
	};
}
