#pragma once

#include "..\System/Typedefs.h"
#include "..\System/Singleton.h"
#include <windows.h>
#include <stdio.h>

class DebugConsole {
	CLASS_TYPEDEFS(DebugConsole)

	SINGLETON_DECLARATIONS(DebugConsole) {
	}

protected:
	FILE*		m_hOutputFile = NULL;
	HANDLE		m_hStdOut = NULL;

public:
	Bool	Open();

	void	Close();

	inline Bool	IsOpen() const {
		return m_hOutputFile != NULL;
	}

	Bool	Write(const Char* msg);
	Bool	Write(const Char* format, const Char* msg);

protected:
	Bool	AttachIO();
	void	DetachIO();
};