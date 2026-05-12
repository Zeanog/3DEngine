#include "System/DebugConsole.h"
#include "System/Win32/Error.h"
#include "System/String.h"


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

	HWND hwndConsole = GetConsoleWindow();
	assert(hwndConsole);
	if (!SetWindowPos(hwndConsole, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)) {
		const Char* errorMsg = GetErrorMessage(GetLastError());
		return false;
	}

	if (HMENU hMenu = GetSystemMenu(hwndConsole, FALSE))
	{
		EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}

	LONG currentStyle = GetWindowLong(hwndConsole, GWL_EXSTYLE);
	verify(!!SetWindowLong(hwndConsole, GWL_EXSTYLE, currentStyle | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW));

	SetConsoleTitle("Neo Debug Window!");

	return true;
}

void DebugConsole::Close() {
	DetachIO();
	verify(FreeConsole());
}

Bool DebugConsole::AttachIO() {
	if (IsOpen()) {
		return true;
	}

	auto err = freopen_s(&m_hStdOutFile, "CONOUT$", "w", stdout);
	if (err) {
		m_hStdOutFile = nullptr;
		m_hErrFile = nullptr;
		return false;
	}

	verify(!freopen_s(&m_hErrFile, "CONOUT$", "w", stderr));

	return IsOpen();
}

void DebugConsole::DetachIO() {
	if (!IsOpen()) {
		return;
	}
	fclose(m_hStdOutFile);
	m_hStdOutFile = nullptr;
	fclose(m_hErrFile);
	m_hErrFile = nullptr;
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