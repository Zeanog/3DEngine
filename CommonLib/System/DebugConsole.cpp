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
	return !err && AttachConsole(ATTACH_PARENT_PROCESS) && IsOpen();
}

void DebugConsole::DetachIO() {
	if (!IsOpen()) {
		return;
	}
	fclose(m_hOutputFile);
	m_hOutputFile = nullptr;
}

Bool	DebugConsole::Write(const Char* format, ...) {
	va_list args;
	__try {
		va_start(args, format);

		auto msg = String::Format(format, args);
		OutputDebugString(msg);
		if (IsOpen()) {
			printf_s(msg);
		}
	}
	__finally {
		va_end(args);
		return true;
	}
}