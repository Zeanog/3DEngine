#pragma once

#include "String.h"

class StackString : public AString {
	INHERITED_CLASS_TYPEDEFS(StackString, AString)

protected:
	Char*	m_pData{};
	UInt32	m_Length{};
	UInt32	m_Allocated{};

protected:
	StackString() {}

public:
	StackString(Char* buffer, Int32 size) {
		m_pData = buffer;
		m_Length = size;
		m_Allocated = size;
	}

	StackString(Char* buffer, Int32 size, const Char* str) {
		m_pData = buffer;
		m_Allocated = size;

		operator=(str);
	}

	const Char*	CStr() const {
		return m_pData;
	}

	Char*	Str() {
		return m_pData;
	}

	UInt32	Length() const {
		return m_Length;
	}

	UInt32	Allocated() const {
		return m_Allocated;
	}

	void	Resize(UInt32 newSize) {
		assert(newSize < Allocated());
		m_Length = newSize;
	}

	Char&	operator[](Int32 index) {
		return m_pData[index];
	}

	const Char&	operator[](Int32 index) const {
		return m_pData[index];
	}

	StackString&	operator=(const String& rhs) {
		auto rhsLen = rhs.Length();
		assert(m_Allocated > rhsLen);
		m_Length = rhsLen;
		String::StrCpy(m_pData, m_Allocated, rhs.CStr());
		return *this;
	}

	StackString&	operator=(const StackString& rhs) {
		assert(m_Allocated > rhs.m_Length);
		m_Length = rhs.m_Length;
		String::StrCpy(m_pData, m_Allocated, rhs.CStr());

		return *this;
	}

	StackString&	operator=(const Char* rhs) {
		auto rhsLen = String::StrLen(rhs);
		assert(m_Allocated > rhsLen);
		m_Length = rhsLen;
		String::StrCpy(m_pData, m_Allocated, rhs);
		return *this;
	}
};

#define STACK_STRING( name, length)	StackString name(STACK_ALLOC(Char, (length)), (length))
#define STACK_STRING_COPY( name, cstr )	StackString name(STACK_ALLOC(Char, String::StrLen(cstr)), String::StrLen(cstr), cstr)