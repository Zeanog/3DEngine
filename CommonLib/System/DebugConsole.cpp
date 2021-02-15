#include "System/DebugConsole.h"
#include "System/Win32/Error.h"
#include "System/String.h"

#include <windows.h>
#include <Wincon.h>

#include <io.h>
#include <fcntl.h>
#include <iostream>

Bool DebugConsole::Open() {
	if (!AllocConsole()) {
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}

	/*if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}*/

	if (!AttachIO()) {
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}

	m_IsOpen = true;
	return true;
}

void DebugConsole::Close() {
	//DetachIO();
	FreeConsole();
	m_IsOpen = false;
}

Bool DebugConsole::AttachIO() {
	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdHandle == INVALID_HANDLE_VALUE)
	{
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}

	m_OutputFileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	if (m_OutputFileDescriptor == -1)
	{
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}

	m_pOutput = _fdopen(m_OutputFileDescriptor, "w");
	if (m_pOutput == NULL)
	{
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}

	/*int dup2Result = _dup2(_fileno(m_pOutput), _fileno(stdout));
	if (dup2Result)
	{
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}*/

	setvbuf(stdout, NULL, _IONBF, 0);
	return true;
	
}

void DebugConsole::DetachIO() {
	if (m_pOutput) {
		fclose(m_pOutput);
		m_pOutput = NULL;
		m_OutputFileDescriptor = -1;
	}
}

Bool DebugConsole::Write(const Char* msg) {
	if (!IsOpen()) {
		return false;
	}

	DWORD numWritten = -1;
	int msgLength = String::StrLen(msg);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msg, msgLength, &numWritten, NULL);
	return msgLength == numWritten;
}