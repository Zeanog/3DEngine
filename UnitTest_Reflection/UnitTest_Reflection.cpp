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

		TEST_METHOD(FailToFindAndCallMethodInfo)
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
				typename TReflector::TReflected testObj;
				typename TReflector::TReflected::TSuper* testObjParent = &testObj;
				methodInfo->Call((typename TReflector::TReflected*)testObjParent, 1509);
			}
			catch (std::runtime_error error) {
				auto formattedString = String::Format("Caught expected error: %s", error.what());
				String::ConvertFor(formattedString, [](const wchar_t* msg) {
					Assert::IsTrue(true, msg);
				});
			}
		}

		TEST_METHOD(CallGetValueFromReflector)
		{
			using TReflector = Reflector<MethodReflectorTest>;

			const Char* methodName = "Value";

			try {
				typename TReflector::TReflected testObj;
				auto ret = Singleton<TReflector>::GetInstance()->Call<UInt32>(StaticString(methodName), &testObj);
			}
			catch (const std::runtime_error& error) {
				String::ConvertFor(error.what(), [](const wchar_t* msg) {
					Assert::Fail(msg);
				});
			}
		}

		TEST_METHOD(CallF1FromReflector)
		{
			using TReflector = Reflector<MethodReflectorTest>;

			const Char* methodName = "F1";

			try {
				InheritanceTest testObjChild;
				typename TReflector::TReflected* testObj = &testObjChild;
				ParentReflectorTest* testObjParent = testObj;

				auto ret = Singleton<TReflector>::GetInstance()->Call<Bool>(StaticString(methodName), &testObjChild, "Hello", 1509);
			}
			catch (const std::runtime_error& error) {
				String::ConvertFor(error.what(), [](const wchar_t* msg) {
					Assert::Fail(msg);
					});
			}
		}
	};
}
