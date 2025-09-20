#include "../System/DebugConsole.h"
#include "../System/Win32/Error.h"
#include "String.h"
#include "StaticString.h"

Bool DebugConsole::Open() {//TODO: Make sure the console doesn't get focus unless clicked on
	if (IsOpen()) {
		return true;
	}

	if (!AllocConsole()) {
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}

	if (!AttachIO()) {
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}

	return true;
}

void DebugConsole::Close() {
	DetachIO();
	FreeConsole();
}

Bool DebugConsole::AttachIO() {
	if (IsOpen()) {
		return true;
	}

	auto err = freopen_s(&m_hOutputFile, "CONOUT$", "w", stdout);
	return !err;
}

void DebugConsole::DetachIO() {
	if (!IsOpen()) {
		return;
	}
	fclose(m_hOutputFile);
	m_hOutputFile = nullptr;
}

Bool	DebugConsole::Write(const Char* msg) {
	OutputDebugString(msg);
	if (IsOpen()) {
		printf_s(msg);
	}
	return true;
}

Bool	DebugConsole::Write(const Char* format, const Char* msg) {
	return Write(String::Format(format, msg));
}