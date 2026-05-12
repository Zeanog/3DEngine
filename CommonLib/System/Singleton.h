#pragma once

template< class TClass >
class Singleton {
public:
	static constexpr TClass*	GetInstance() {
		static TClass instance;
		return &instance;
	}
};

#define SINGLETON_DECLARATIONS( self ) \
friend class Singleton<self>;	\
protected:						\
	self()