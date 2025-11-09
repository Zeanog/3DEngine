#pragma once

#include "TypeDefs.h"
#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <TCHAR.h>

class AString {
protected:
	AString() {
	}

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
		m_Data = str;
	}

	const Char*	CStr() const {
		return m_Data.c_str();
	}

	Char*	CStr() {
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
		return CStr()[ index ];
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

	static Int32 FindLastOf(const Char* str, Char ch) {
		for( int ix = String::StrLen(str) - 1; ix >= 0; --ix ) {
			if( str[ix] == ch ) {
				return ix;
			}
		}
		return -1;
	}

	Int32 FindIndexOf(const String& lookingFor) {
		return m_Data.find(lookingFor.CStr(), (size_t)0);
	}

	static Int32 FindIndexOf(const Char* str, const Char* lookingFor) {
		auto matchPtr = strstr(str, lookingFor);
		return matchPtr != nullptr ? matchPtr - str : -1;
	}

	void	Replace(UInt32 offset, Int32 count, const Char* newSubString) {
		m_Data.replace(offset, count, newSubString);
	}

	static const Char* Replace(const Char* str, UInt32 offset, Int32 count, const Char* newSubString);

public:
	inline static UInt32	StrLen( const Char* str ) {
		return strlen( str );
	}

	inline static Int32	StrICmp( const Char* lhs, const Char* rhs ) {
		return _stricmp( lhs, rhs );
	}

	inline static Bool	StrCpy( Char* dst, UInt32 size, const Char* src) {
		 return !strcpy_s(dst, size, src);
	}

	static const Char* Format(const char* format, va_list& args) {
		static const int NumBuffers = 5;
		static const int MaxBufferSize = 256;
		static int currentIndex = 0;
		static Char buffers[NumBuffers][MaxBufferSize];

		Char* currentBuffer = buffers[currentIndex];

		currentIndex = (currentIndex + 1) % NumBuffers;
		vsprintf_s(currentBuffer, MaxBufferSize, format, args);
		return currentBuffer;
	}

	static const Char*	Format( const char* format, ... ) {
		va_list args;
		va_start(args, format);
		auto str = Format(format, args);
		va_end(args);
		return str;
	}
};