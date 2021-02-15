#pragma once

#include "System/Typedefs.h"

template< Char Letter >
struct VirtualKey {
	enum {
		Code = (Letter - 'A') + 0x41
	};
};