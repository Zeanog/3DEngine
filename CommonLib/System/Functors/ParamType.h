#pragma once

#define BUS_BIT_SIZE	(64)
#define BUS_BYTE_SIZE	(BUS_BIT_SIZE / 8)

template< typename _TData >
class Param {
protected:
	template< typename _TData, bool _LessThanEqualBusSize >
	class ParamTypeHelper {
	public:
		typedef _TData		Type;
	};

	template< typename _TData >
	class ParamTypeHelper<_TData, false> {
	public:
		typedef const _TData& Type;
	};

public:
	typedef _TData TData;
	typedef typename ParamTypeHelper<TData, sizeof(TData) <= BUS_BYTE_SIZE>::Type	Type;
};