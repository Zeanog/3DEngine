#pragma once

#if WIN32
static constexpr UInt32 BUS_BIT_SIZE = 32;
#else
static constexpr UInt32 BUS_BIT_SIZE = 64;
#endif
static constexpr UInt32 BUS_BYTE_SIZE = (BUS_BIT_SIZE / 8);

template< typename _TData >
class Param {
protected:
	template< typename _TData, bool _LessThanEqualBusSize >
	class Helper {
	public:
		typedef _TData		Type;
	};

	template< typename _TData >
	class Helper<_TData, false> {
	public:
		typedef const _TData& Type;
	};

public:
	typedef _TData TData;
	typedef typename Helper<TData, sizeof(TData) <= BUS_BYTE_SIZE && !std::is_class<TData>::value>::Type	Type;
};