#pragma once

#include "../System/StaticString.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

class File {
public:
	Bool	Open( const StaticString& path, const Char* mode ) {
		return Open( path.CStr(), mode );
	}

	Bool	Open( const Char* path, const Char* mode ) {
		Close();

		if( !path || !path[0] ) {
			return false;
		}

		errno_t err = fopen_s(&m_File, path, mode);
		if( !!err ) {
			return false;
		}

		ValidateLength();

		return true;
	}

	Bool	IsOpen() const {
		return m_File != NULL;
	}

	void	Close() {
		if( m_File != NULL ) {
			fclose( m_File );
			m_File = NULL;
			m_Length = 0;
		}
	}

	template< typename TValue >
	Bool	Read( TValue& val ) {
		if( !IsOpen()) {
			return false;
		}

		UInt32 numElementsRead = fread( &val, sizeof(TValue), 1, m_File );
		if( numElementsRead == 1 ) {
			return true;
		}

		assert( !ReportedError() );
		return false;
	}

	template< typename TValue >
	Bool	Read( TValue* val, Int32 numElements ) {
		if( !IsOpen()) {
			return false;
		}

		Int32 amountToRead = (sizeof(TValue) * numElements);
		Int32 remaining = Remaining();
		assert( remaining >= amountToRead );

		UInt32 numElementsRead = fread( (void*)val, sizeof(TValue), numElements, m_File );
		if( numElementsRead == numElements ) {
			return true;
		}

		assert( !ReportedError() );
		return false;
	}

	template< typename TValue >
	Bool	Write( const TValue& val ) {
		if( !IsOpen()) {
			return false;
		}

		UInt32 numElementsWritten = fwrite( &val, sizeof(TValue), 1, m_File );
		if( numElementsWritten == 1 ) {
			return true;
		}

		assert( !ReportedError() );
		return false;
	}

	template< typename TValue >
	Bool	Write( TValue* val, Int32 numElements ) {
		if( !IsOpen()) {
			return false;
		}

		//Int32 amountToWrite = (sizeof(TValue) * numElements);
		//Int32 remaining     = Remaining();
		//assert( remaining >= amountToWrite );

		UInt32 numElementsWritten = fwrite( val, sizeof(TValue), numElements, m_File );
		if( numElementsWritten == numElements ) {
			return true;
		}

		assert( !ReportedError() );
		return false;
	}

	Int32	Length() const {
		assert( IsOpen() );
		return m_Length;
	}

	Int32	Tell() const {
		assert( IsOpen() );

		return ftell( m_File );
	}

	Int32	Remaining() const {
		assert( IsOpen() );
		assert( Length() > 0 );
		return Length() - Tell();
	}

	void	Seek( Int32 offset, Int32 origin ) const {
		assert( IsOpen() );
		fseek( m_File, offset, origin );
	}

	bool	IsEof() const {
		assert( IsOpen() );

		return !!feof( m_File );
	}

	bool	ReportedError() const {
		assert( IsOpen() );

		bool err = !ferror( m_File );
		UInt32 remaining = Length() - Tell();
		return err;
	}

	FILE*	GetHandle() {
		return m_File;
	}

protected:
	FILE*	m_File;
	UInt32	m_Length;

public:
	File() {
		m_File = NULL;
		m_Length = 0;
	}

	explicit File( FILE* file ) {
		m_File = file;

		ValidateLength();
	}

	virtual ~File() {
		Close();
	}

protected:
	void	ValidateLength() {
		Int32 pos = Tell();
		Seek( 0, SEEK_END );
		m_Length = Tell();
		Seek( pos, SEEK_SET );
	}
};

class ISerializable {
protected:

	ISerializable(File& file) {
	}

public:
	virtual Bool	ReadFrom( File& file ) = 0;
	virtual Bool	WriteTo( File& file ) const = 0;
};