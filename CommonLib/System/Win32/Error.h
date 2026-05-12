#pragma once

#include "System/Typedefs.h"

#if _WIN32
#include <Windows.h>
const Char*	GetErrorMessage(DWORD errorCode);
const Char* GetErrorMessage(HRESULT errorCode);
#endif