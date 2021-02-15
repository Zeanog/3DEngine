#include "StaticString.h"
#include "StackString.h"
#include <windows.h>

const Char* FilePath::GetLocalPath(const StaticString& fullPath) {
	STACK_STRING(systemPath, 256);

	Int32 systemPathLength = GetCurrentDirectory(systemPath.Length(), systemPath.CStr());

	return &fullPath.CStr()[systemPathLength + 1];
}