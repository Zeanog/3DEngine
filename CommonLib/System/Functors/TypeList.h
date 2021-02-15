#pragma once

#include "System/Typedefs.h"

struct TInvalid {
};

struct TNull {
};

template< class THead, class TTail >
struct TypeListNode {
	typedef THead	Head;
	typedef TTail	Tail;
};

#define TYPELIST( THead, TTail )				TypeListNode<THead, TTail>

#define TYPELIST_1( T1 )						TypeListNode< T1, TNull >
#define TYPELIST_2( T1, T2 )					TypeListNode< T1, TYPELIST_1(T2) >
#define TYPELIST_3( T1, T2, T3 )				TypeListNode< T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4( T1, T2, T3, T4 )			TypeListNode< T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5( T1, T2, T3, T4, T5 )		TypeListNode< T1, TYPELIST_4(T2, T3, T4, T5) >
#define TYPELIST_6( T1, T2, T3, T4, T5, T6 )	TypeListNode< T1, TYPELIST_5(T2, T3, T4, T5, T6) >

///////////////////////////////////////////////////

template< class TList >
struct Length;

template<>
struct Length<TNull> {
	enum { Value = 0 };
};

template< class THead, class TTail >
struct Length< TYPELIST(THead, TTail) > {
	enum { Value = 1 + Length<TTail>::Value };
};

///////////////////////////////////////////////////

template< class TList, Int32 index >
struct TypeAt;

template< class THead, class TTail >
struct TypeAt< TYPELIST(THead, TTail), 0 > {
	typedef	THead	Result;
};

template< class THead, class TTail, UInt32 index >
struct TypeAt< TYPELIST(THead, TTail), index > {
	typedef	typename TypeAt<TTail, index - 1>::Result	Result;
};

///////////////////////////////////////////////////

template< class TList, Int32 index, typename TDefault >
struct _TypeAt;

template< class THead, class TTail, typename TDefault >
struct _TypeAt< TYPELIST(THead, TTail), 0, TDefault > {
	typedef	THead	Result;
};

template< UInt32 index, typename TDefault >
struct _TypeAt< TNull, index, TDefault > {
	typedef	TDefault	Result;
};

template< class THead, class TTail, UInt32 index, typename TDefault >
struct _TypeAt< TYPELIST(THead, TTail), index, TDefault > {
	typedef	typename _TypeAt<TTail, index - 1, TDefault>::Result	Result;
};