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
		TEST_METHOD(FindMethodInfo)
		{
			using TReflector = Reflector<MethodReflectorTest>;

			TReflector::MethodInfoFor_Value<1>* methodInfo;
			const Char* methodName = "Value";

			if (!Singleton<TReflector>::GetInstance()->FindMethodInfo(methodName, methodInfo)) {
				auto formattedString = String::Format("Failed to find method info for %s", methodName);
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::Fail(msg);
				});
			}

			try {
				InheritanceTest testObj;
				methodInfo->Call(&testObj, 9);
			}
			catch (std::runtime_error error) {
				auto formattedString = String::Format("Caught error: %s", error.what());
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::Fail(msg);
				});
			}
		}

		TEST_METHOD(FailToFindMethodInfo)
		{
			using TReflector = Reflector<MethodReflectorTest>;
			TReflector::MethodInfoFor_F2* methodInfo;

			const Char* methodName = "F1";
			if (Singleton<TReflector>::GetInstance()->FindMethodInfo(methodName, methodInfo)) {
				auto formattedString = String::Format("Shouldn't have found method info for %s", methodName);
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::Fail(msg);
				});
			}
		}

		TEST_METHOD(FailToCallMethodInfo)
		{
			using TReflector = Reflector<MethodReflectorTest>;

			TReflector::MethodInfoFor_Value<1>* methodInfo;
			const Char* methodName = "Value";

			if (!Singleton<TReflector>::GetInstance()->FindMethodInfo(methodName, methodInfo)) {
				auto formattedString = String::Format("Failed to find method info for %s", methodName);
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::Fail(msg);
				});
			}

			try {
				MethodReflectorTest testObj;
				ParentReflectorTest* testObjParent = &testObj;
				methodInfo->Call(testObjParent, 1509);
			}
			catch (std::runtime_error error) {
				auto formattedString = String::Format("Caught expected error: %s", error.what());
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::IsTrue(true, msg);
				});
			}
		}

		TEST_METHOD(CallSetValue)
		{
			using TReflector = Reflector<MethodReflectorTest>;

			const Char* methodName = "Value";

			try {
				typename TReflector::TReflected testObj;
				Singleton<TReflector>::GetInstance()->Call<1, void>(StaticString(methodName), &testObj, 1509);
			}
			catch (...) {
				String::ConvertFor(String::Format("Unexpected error calling %s", methodName), [](const wchar_t* msg) {
					Assert::Fail(msg);
				});
			}
		}

		TEST_METHOD(CallF1)
		{
			using TReflector = Reflector<MethodReflectorTest>;

			const Char* methodName = "F1";

			try {
				typename TReflector::TReflected testObj;
				ParentReflectorTest* testObjParent = &testObj;

				auto ret = Singleton<TReflector>::GetInstance()->Call<Bool>(StaticString(methodName), &testObj, (const Char*)"Hello", 1509);
			}
			catch(...) {
				String::ConvertFor(String::Format("Unexpected error calling %s", methodName), [](const wchar_t* msg) {
					Assert::Fail(msg);
				});
			}
		}
	};
}
