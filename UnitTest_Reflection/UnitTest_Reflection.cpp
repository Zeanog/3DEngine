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
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::Fail(msg, NULL);
				});
			}

			try {
				InheritanceTest testObj;
				methodInfo->Call(&testObj, 9);
			}
			catch (std::runtime_error error) {
				auto formattedString = String::Format("Caught error: %s", error.what());
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::Fail(msg, NULL);
				});
			}
		}

		TEST_METHOD(TestMethod2)
		{
			using TReflector = Reflector<MethodReflectorTest>;
			TReflector::MethodInfoFor_Value<0>* methodInfo;

			const Char* methodName = "F1";
			if (Singleton<TReflector>::GetInstance()->FindMethodInfo(methodName, methodInfo)) {
				auto formattedString = String::Format("Shouldn't have found method info for %s", methodName);
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::Fail(msg, NULL);
				});
			}
		}

		TEST_METHOD(TestMethod3)
		{
			using TReflector = Reflector<MethodReflectorTest>;

			TReflector::MethodInfoFor_Value<1>* methodInfo;
			const Char* methodName = "Value";

			if (!Singleton<TReflector>::GetInstance()->FindMethodInfo(methodName, methodInfo)) {
				auto formattedString = String::Format("Failed to find method info for %s", methodName);
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::Fail(msg, NULL);
				});
			}

			try {
				ParentReflectorTest testObj2;
				methodInfo->Call(&testObj2, 1509);
			}
			catch (std::runtime_error error) {
				auto formattedString = String::Format("Caught expected error: %s", error.what());
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::IsTrue(true, msg);
				});
			}
		}
	};
}
