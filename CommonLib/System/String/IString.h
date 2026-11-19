#pragma once

#include "System/TypeDefs.h"

class IString {
public:
	virtual const Char* CStr() const = 0;
};