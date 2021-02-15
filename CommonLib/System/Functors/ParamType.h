#pragma once

#include "System/Typedefs.h"

#define BUS_BIT_SIZE	(32)
#define BUS_BYTE_SIZE	(BUS_BIT_SIZE / 8)

template< typename _TData, Bool _LessThanEqualBusSize >
class ParamTypeHelper {
public:
	typedef _TData		Type;
};

template< typename _TData >
class ParamTypeHelper<_TData, false> {
public:
	typedef const _TData&		Type;
};


template< typename _TData >
class ParamType {
public:
	typedef _TData TData;
	typedef typename ParamTypeHelper<TData, sizeof(TData) <= BUS_BYTE_SIZE>::Type	Type;
};