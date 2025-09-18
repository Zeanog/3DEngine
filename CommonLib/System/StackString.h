#pragma once

#include "String.h"

class StackString : public AString {
public:

protected:
	Char * m_pData = NULL;
	UInt32	m_Size = 0;
	UInt32	m_Allocated = 0;

protected:
	StackString() {}

public:
	StackString(Char* buffer, Int32 size) {
		m_pData = buffer;
		m_Size = size;
		m_Allocated = size;
	}

	/*virtual Bool	ReadFrom(json_value* val) {
		m_pData = val->u.string.ptr;
		m_Size = val->u.string.length;
		return true;
	}*/

	const Char*	CStr() const {
		return m_pData;
	}

	Char*	CStr() {
		return m_pData;
	}

	UInt32	Length() const {
		return m_Size;
	}

	UInt32	Allocated() const {
		return m_Allocated;
	}

	void	Resize(UInt32 newSize) {
		assert(newSize < Allocated());
		m_Size = newSize;
	}

	/*Bool	operator<(const String& rhs) const {
	return StrICmp(m_Data.c_str(), rhs.m_Data.c_str()) < 0;
	}*/

	/*StackString&	operator=(const String& rhs) {
	m_pData = rhs.m_Data;
	return *this;
	}*/

	Char&	operator[](Int32 index) {
		return m_pData[index];
	}

	const Char&	operator[](Int32 index) const {
		return m_pData[index];
	}

	StackString&	operator=(const String& rhs) {
		String::StrCpy(m_pData, m_Allocated, rhs.CStr());
		return *this;
	}

	/*StackString&	operator=(const StaticString& rhs) {
	String::StrCpy(m_pData, m_Size, rhs.CStr());
	return *this;
	}*/

	StackString&	operator=(const StackString& rhs) {
		String::StrCpy(m_pData, m_Allocated, rhs.CStr());
		return *this;
	}

	StackString&	operator=(const Char* rhs) {
		String::StrCpy(m_pData, m_Allocated, rhs);
		return *this;
	}

	/*String&	operator=(const Char* rhs) {
	m_Data = rhs;
	return *this;
	}*/
};

#define STACK_STRING( name, sizeInBytes )	StackString name((Char*)_alloca((sizeInBytes)), (sizeInBytes))