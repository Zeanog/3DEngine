#include "System/Win32/Error.h"

#include <windows.h>

#if _WIN32
const Char*	GetErrorMessage(int errorCode) {
	static Char	buffers[5][256];
	static int	currentBufferIndex = 0;

	Char* buffer = buffers[currentBufferIndex++];

	size_t size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)buffer, 256, NULL);

	return buffer;
}
#endif