#pragma once

#include "TypeDefs.h"
#include <string>

class AString {
	ABSTRACT_CLASS_TYPEDEFS(AString) {}

public:
	virtual const Char*	CStr() const = 0;
};

class String : public AString {
protected:
	std::string	m_Data;

public:
	String() {
	}

	String( const Char* str ) {
		m_Data.assign(str);
	}

	const Char*	CStr() const {
		return m_Data.c_str();
	}

	Char*	Str() {
		return const_cast<Char*>(m_Data.c_str());
	}

	UInt32	Length() const {
		return m_Data.size();
	}

	Bool	operator<( const String& rhs ) const {
		return StrICmp( m_Data.c_str(), rhs.m_Data.c_str() ) < 0;
	}

	String&	operator=( const String& rhs ) {
		m_Data = rhs.m_Data;
		return *this;
	}

	String&	operator+=(const String& rhs) {
		m_Data += rhs.m_Data;
		return *this;
	}

	Char&	operator[]( Int32 index ) {
		return Str()[ index ];
	}

	const Char&	operator[]( Int32 index ) const {
		return m_Data[ index ];
	}

	String&	operator=( const Char* rhs ) {
		m_Data = rhs;
		return *this;
	}

	void	Resize(UInt32 newSize) {
		m_Data.resize(newSize);
	}

	Int32 FindLastOf( Char ch, Int32 offset ) const {
		return m_Data.find_last_of( ch, offset );
	}

	Int32 FindLastOf( Char ch ) const {
		return m_Data.find_last_of( ch, -1 );
	}

	static constexpr Int32 FindLastOf(const Char* str, UInt32 strLen, Char ch) {
		for (int ix = strLen - 1; ix >= 0; --ix) {
			if (str[ix] == ch) {
				return ix;
			}
		}

		return -1;
	}

	static constexpr Int32 FindLastOf(const Char* str, Char ch) {
		UInt32 strLen = 0;
		while (str[strLen] ) {
			++strLen;
		}

		return FindLastOf(str, strLen, ch);
	}

	Int32 FindIndexOf(const String& lookingFor) {
		return m_Data.find(lookingFor.CStr(), (size_t)0);
	}

	static Int32 FindIndexOf(const Char* str, const Char* lookingFor) {
		if(!lookingFor || !lookingFor[0]) {
			return -1;
		}
		auto matchPtr = strstr(str, lookingFor);
		return matchPtr != nullptr ? matchPtr - str : -1;
	}

	void	Replace(UInt32 offset, Int32 count, const Char* newSubString) {
		m_Data.replace(offset, count, newSubString);
	}

	static const Char* Replace(const Char* str, UInt32 offset, Int32 count, const Char* newSubString);

public:
	inline static UInt32	Length( const Char* str ) {
		/*UInt32 strLen = 0;
		while (str[strLen]) {
			++strLen;
		}
		return strLen;
		*/
		return strlen( str );
	}

	inline static Int32	StrICmp( const Char* lhs, const Char* rhs ) {
		return _stricmp( lhs, rhs );
	}

	static Bool	StrCpy(Char* dst, UInt32 size, const Char* src);

	static Char* Format(const char* format, va_list& args) {
		static constexpr int NumBuffers = 5;
		static constexpr int MaxBufferSize = 256;
		static int currentIndex = 0;
		static Char buffers[NumBuffers][MaxBufferSize];

		Char* currentBuffer = buffers[currentIndex];

		currentIndex = (currentIndex + 1) % NumBuffers;
		vsprintf_s(currentBuffer, MaxBufferSize, format, args);
		return currentBuffer;
	}

	static Char* Format(const char* format, ...);

	template<typename TAction>
	static void ConvertFor(const char* formattedString, TAction action) {
		size_t formattedStringLen = String::Length(formattedString);
		size_t wcStrLen = formattedStringLen * sizeof(wchar_t);
		auto wcStr = STACK_ALLOC(wchar_t, wcStrLen);
		size_t convertedChars = 0;

		// Convert mbStr to wcStr
		// _TRUNCATE allows the function to copy as much as fits
		errno_t err = mbstowcs_s(&convertedChars, wcStr, wcStrLen, formattedString, _TRUNCATE);
		assert(!err);
		action(wcStr, NULL);
	}
};