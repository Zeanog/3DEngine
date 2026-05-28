#pragma once

#include "Typedefs.h"
#include "String.h"
#include <map>
#include <functional>

class HashGenerator {
public:
	static UInt32	Generate( const Char* str ) {
		return std::hash<std::string>()(str);
	}

	static UInt32	Generate( Char ch ) {
		return std::hash<Char>()(ch);
	}
};

#include "Singleton.h"

class StringPool {
	SINGLETON_DECLARATIONS( StringPool ) {}

public:
	typedef UInt32	THashValue;
	typedef std::map<THashValue, String>	TContainer;

protected:
	TContainer	m_StringMap;

public:
	THashValue	Add( const Char* str ) {
		THashValue hash = HashGenerator::Generate( str );
		TContainer::iterator iter = m_StringMap.find( hash );
		if( iter != m_StringMap.end() ) {
			assert( iter->first == hash );
			return hash;
		}

		m_StringMap[ hash ] = str;

		return hash;
	}

	const String&	operator[]( THashValue hash ) const {
		assert( m_StringMap.find(hash) != m_StringMap.end() );
		return m_StringMap.at( hash );
	}
};

class StaticString : public AString {
protected:
	UInt32	m_Hash;

#if _DEBUG
	const Char*	m_DebugPtr = NULL;
#endif

public:
	StaticString() {
		m_Hash = 0;
	}

	StaticString( const Char* str ) {
		m_Hash = Singleton<StringPool>::GetInstance()->Add( str );
#if _DEBUG
		m_DebugPtr = (m_Hash) ? CStr() : NULL;
		UInt32 debugHash = Singleton<StringPool>::GetInstance()->Add( m_DebugPtr );
		assert( !String::StrICmp(m_DebugPtr, str) );
#endif
	}

	StaticString( const StaticString& str ) {
		m_Hash = str.m_Hash;
#if _DEBUG
		m_DebugPtr = (m_Hash) ? CStr() : NULL;
#endif
	}

	const String&	Str() const {
		return (*Singleton<StringPool>::GetInstance())[ m_Hash ];
	}

	virtual const Char*	CStr() const override {
		return Str().CStr();
	}

	UInt32		Length() const {
		return (m_Hash <= 0) ? 0 : Str().Length();
	}

	Int32 FindLastOf(Char ch, Int32 offset) const {
		return (m_Hash <= 0) ? -1 : Str().FindLastOf(ch, offset);
	}

	Int32 FindLastOf(Char ch) const {
		return (m_Hash <= 0) ? -1 : Str().FindLastOf(ch);
	}

	Int32 FindIndexOf(const Char* subStr) const {
		return (m_Hash <= 0) ? -1 : String::FindIndexOf(CStr(), subStr);
	}

	StaticString&	operator=( const StaticString& rhs ) {
		m_Hash = rhs.m_Hash;
#if _DEBUG
		m_DebugPtr = (m_Hash) ? CStr() : NULL;
#endif
		return *this;
	}

	Bool	operator==( const StaticString& rhs ) const {
		return m_Hash == rhs.m_Hash;
	}

	Bool	operator!=(const StaticString& rhs) const {
		return m_Hash != rhs.m_Hash;
	}

	Bool	operator<( const StaticString& rhs ) const {
		return m_Hash < rhs.m_Hash;
	}
};