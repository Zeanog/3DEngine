# 3DEngine

Just a really rough engine.  Mostly low level code.

I have been working on this slowly over a couple of years.  I found a demo (https://lnkd.in/gw6GtmZ) to learn from.

I've also been using this to tryout various ideas in C++ I've had, like a foreach loop.  So some of the code is inefficient.  But I hope somewhat clever.  :D

Reflection Example:
```
class MethodReflectorTest {
	CLASS_TYPEDEFS(MethodReflectorTest)

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

template<>
class Reflector<MethodReflectorTest> : public AReflectorJson {
private:
	INHERITED_CLASS_TYPEDEFS(Reflector, AReflectorJson)
	SINGLETON_DECLARATIONS(Reflector) {
		REGISTER_MEMBER(TReflected, ValueName)
	}

public:
	DEFINE_METHODINFO_ACCESSORS(m_MethodList)

	typedef MethodReflectorTest	TReflected;

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
```


5/30/2026:
I broke lighting and the FBX loader.  I really wanted to check-in my updated reflection code.
You can now reflect methods.  System also handles method overloading.  Still needs a lot more testing.  System/Reflector.h
Also added unit tests

11/6/2025:
Added a reflection system a couple of weeks ago.  Allows users to define a reflection object for a given data structure.  Needs work in regards to complex types like lists and maps.

9/14/2025:
Added first pass at xAudio2 implementation.

2/23/2022:
TODO:  Pull out animation curves from FBX files instead of/along with key frames

