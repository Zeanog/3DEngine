#include "System/Win32/Error.h"

#include <windows.h>

#if _WIN32
const Char*	GetErrorMessage(int errorCode) {
	static const int numBuffers = 5;
	static const int bufferSize = 256;
	static Char	buffers[numBuffers][bufferSize];
	static int	currentBufferIndex = 0;

	currentBufferIndex = (currentBufferIndex + 1) % numBuffers;
	Char* buffer = buffers[currentBufferIndex];

	size_t size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)buffer, bufferSize, NULL);

	return buffer;
}
#endif