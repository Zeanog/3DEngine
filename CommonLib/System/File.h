#pragma once

#include "System/StaticString.h"
#include "StackString.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

template< typename TData >
class List;

class File {
public:
	static constexpr Char	Delimiters[] = { '\\', '/' };
	static StaticString		WorkingDirectory;
	static StaticString		WorkingDataDirectory;

public:
	//TODO: Possibly move these to a 'FileSystem' class
	static void			SetWorkingDirectory(const Char* path);
	static const Char* RebuildFullPath(String& path, const Char* removePath, const Char* addPath);
	static const Char* RebuildFullPath(const Char* path, const Char* removePath, const Char* addPath);
	static const Char* RebuildFullPath(String& path, const Char* newWorkingDirPath);
	static const Char* RebuildFullPath(const Char* path, const Char* newWorkingDirPath);
	static const Char* BuildFullPath(StaticString& relativePath);
	static const Char* BuildFullPath(const Char* relativePath);
	static const Char* RebuildFullPath(String& inoutFullPath);
	static const Char* RebuildFullPath(const Char* inFullPath);
	static const Char* RebuildFullDataPath(String& inoutFullPath);
	static const Char* RebuildFullDataPath(const Char* inFullPath);

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

		UInt32 numElementsRead = fread_s( &val, sizeof(TValue), sizeof(TValue), 1, m_File );
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

		UInt32 numElementsRead = fread_s( (void*)val, sizeof(TValue) * numElements, sizeof(TValue), numElements, m_File );
		if( numElementsRead == numElements ) {
			return true;
		}

		assert( !ReportedError() );
		return false;
	}

	Bool ReadContents( List<Byte>& outContents);

	Bool ReadContents(String& outContents) {
		assert(IsOpen());

		outContents.Resize(Length());
		UInt32 numElementsRead = fread_s((void*)outContents.CStr(), outContents.Length(), 1, outContents.Length(), m_File);
		outContents.Resize(numElementsRead);

		assert(!ReportedError());
		return numElementsRead <= outContents.Length();
	}

	Bool ReadContents(StackString& outContents) {
		assert(IsOpen());

		errno = 0;

		UInt32 numElementsRead = fread_s((void*)outContents.CStr(), outContents.Length(), 1, outContents.Length(), m_File);
		assert(numElementsRead <= outContents.Allocated());

		outContents.Resize(numElementsRead);
		outContents[numElementsRead] = '\0';//numElementsRead == outContents.Allocated() will cause us to put a character out of bounds

		assert(!ReportedError());
		return outContents.Length() == String::Length(outContents.CStr());
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

	UInt32	Length() const {
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
		auto errNum = errno;
		UInt32 remaining = Length() - Tell();
		return errNum > 0;
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

class FilePath {
public:
	static const Char* GetExtension(const String& str) {
		Int32 index = str.FindLastOf('.');
		if (index < 0) {
			return NULL;
		}

		return &(str.CStr()[index]);
	}

	static Bool	HasExtension(const String& str, const Char* ext) {
		const Char* lhs = GetExtension(str);
		return !String::StrICmp(lhs, ext);
	}

	static const Char* GetExtension(const StaticString& str) {
		Int32 index = str.FindLastOf('.');
		if (index < 0) {
			return NULL;
		}

		return &(str.CStr()[index]);
	}

	static const Char* GetExtension(const Char* str) {
		Int32 index = String::FindLastOf(str, '.');
		if (index < 0) {
			return NULL;
		}

		return &(str[index]);
	}

	static void		SetExtension(String& str, const Char* newExtension) {
		Int32 index = str.FindLastOf('.');
		if (index < 0) {
			return;
		}

		UInt32 length = str.Length() - index;
		Char* extPtr = str.Str() + index;
		for (UInt32 ix = 0; ix < length; ++ix) {
			extPtr[ix] = newExtension[ix];
		}
	}

	static Bool	HasExtension(const StaticString& str, const Char* ext) {
		auto strLen = str.Length();
		auto extLen = String::Length(ext);

		const Char* lhs = &str.CStr()[strLen - extLen];

		//const Char* lhs = GetExtension(str);
		return !String::StrICmp(lhs, ext);
	}

	static const Char* GetFileName(const StaticString& fullPath) {
		Int32 index = fullPath.FindLastOf(File::Delimiters[0]);
		if (index < 0) {
			index = fullPath.FindLastOf(File::Delimiters[1]);
			if (index < 0) {
				return fullPath.CStr();
			}
		}
		
		return &(fullPath.CStr()[index + 1]);
	}

	static const Char* GetLocalPath(const StaticString& fullPath);
};