#pragma once

#include "Typedefs.h"

template< typename TClass >
class Singleton {
public:
	static TClass*	GetInstance() {
		static TClass instance;
		return &instance;
	}
};

#define SINGLETON_DECLARATIONS( self ) \
friend class Singleton<self>;	\
protected:						\
	self()