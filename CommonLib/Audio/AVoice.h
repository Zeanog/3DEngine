#pragma once

#include "System/Typedefs.h"

class AVoice {
    CLASS_TYPEDEFS(AVoice)

	friend class AudioSystem;

protected:
	virtual void	Destroy() = 0;
};

template< typename _TData >
void DestroyVoice(_TData*& ptr) {
	if (ptr) {
		ptr->DestroyVoice();
	}
	ptr = NULL;
}