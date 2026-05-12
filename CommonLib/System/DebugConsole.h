#pragma once

#include "System/Typedefs.h"
#include "System/Singleton.h"
//#include <windows.h>
#include <stdio.h>

class DebugConsole {
	CLASS_TYPEDEFS(DebugConsole)
	SINGLETON_DECLARATIONS(DebugConsole) {
	}

protected:
	FILE*		m_hStdOutFile = nullptr;
	FILE*		m_hErrFile = nullptr;

public:
	Bool	Open();
	void	Close();

	inline Bool	IsOpen() const {
		return m_hStdOutFile != nullptr && m_hErrFile != nullptr;
	}

	Bool	Write(const Char* format, ...);

protected:
	Bool	AttachIO();
	void	DetachIO();
};