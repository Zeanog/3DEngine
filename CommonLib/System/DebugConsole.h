#pragma once

#include "..\System/Typedefs.h"
#include "..\System/Singleton.h"

#include <stdio.h>

class DebugConsole {
	CLASS_TYPEDEFS(DebugConsole);

	SINGLETON_DECLARATIONS(DebugConsole) {
		m_IsOpen = false;
	}

protected:
	int		m_OutputFileDescriptor;
	FILE*	m_pOutput;

	Bool	m_IsOpen;

public:
	Bool	Open();

	void	Close();

	inline Bool	IsOpen() const {
		return m_IsOpen;
	}

	Bool	Write(const Char* msg);

protected:
	Bool	AttachIO();
	void	DetachIO();
};