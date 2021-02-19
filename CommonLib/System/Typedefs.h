#pragma once

#include <assert.h>
#include <stdlib.h>

#ifdef UNICODE
typedef wchar_t			Char;
#else
typedef char			Char;
#endif

typedef unsigned char	Byte;
typedef unsigned char	UInt8;
typedef unsigned short	UInt16;
typedef unsigned int	UInt32;
typedef char			Int8;
typedef short			Int16;
typedef int				Int32;
typedef long			Int64;
typedef bool			Bool;

typedef float			Float32;
typedef double			Float64;

template<typename TData>
class TypeInfo {
public:
	typedef TData	TUndecorated;
};

template<typename TData>
class TypeInfo<TData&> {
public:
	typedef TData	TUndecorated;
};

template<typename TData>
class TypeInfo<TData*> {
public:
	typedef TData	TUndecorated;
};

#define STATIC_ARRAY_LENGTH( array )	(sizeof(array) / (sizeof(array[0])))

#define CPU_BUS_SIZE 32

template< typename _TValue, Bool underBusSize >
struct _ParamHelper {
	typedef _TValue		Type;
};

template< typename _TValue >
struct _ParamHelper<_TValue, false> {
	typedef const _TValue&		Type;
};

template< typename _TValue >
struct Param {
	typedef typename _ParamHelper<_TValue, sizeof(_TValue) <= CPU_BUS_SIZE>::Type Type;
};

template< class TContainer >
class ContainerIterator {
public:
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;
	typedef typename TUndecorated::iterator	Iterator;
	typedef typename TUndecorated::const_iterator	ConstIterator;

	static Iterator	Begin(TUndecorated& container) {
		return container.begin();
	}

	static ConstIterator	Begin(const TUndecorated& container) {
		return container.cbegin();
	}

	static Iterator	End(TUndecorated& container) {
		return container.end();
	}

	static ConstIterator	End(const TUndecorated& container) {
		return container.cend();
	}
};

#define FOREACH( iterName, stlContainer )	\
	for( typename ContainerIterator<decltype(stlContainer)>::Iterator iterName = ContainerIterator<decltype(stlContainer)>::Begin(stlContainer); iterName != ContainerIterator<decltype(stlContainer)>::End(stlContainer); iterName++ )

#define FOREACH_CONST( iterName, stlContainer )	\
	for( typename ContainerIterator<decltype(stlContainer)>::ConstIterator iterName = ContainerIterator<decltype(stlContainer)>::Begin(stlContainer); iterName != ContainerIterator<decltype(stlContainer)>::End(stlContainer); iterName++ )

#define STACK_ALLOC( type, num )	(type*)_alloca( sizeof(type)*num )

#define CLASS_TYPEDEFS( classType )		\
public:								\
	typedef classType	TSelf;		\
private:

#define INHERITEDCLASS_TYPEDEFS( classType, superClass )		\
CLASS_TYPEDEFS( classType );								\
public:													\
typedef superClass		TSuper;							\
private:

#define TEMPLATE_2( classType, T1, T2 )	classType<T1, T2>
#define TEMPLATE_3( classType, T1, T2, T3 )	classType<T1, T2, T3>
#define TEMPLATE_4( classType, T1, T2, T3, T4 )	classType<T1, T2, T3, T4>

#ifdef _DEBUG
#define verify	assert
#else
#define verify
#endif

template< typename _TData >
void DeletePtr( _TData*& ptr ) {
	delete ptr;
	ptr = NULL;
}

template< typename _TData >
void DeleteArray( _TData*& ptr ) {
	delete[] ptr;
	ptr = NULL;
}

template< class _TData >
void DeleteContents( _TData& stlContainer ) {
	FOREACH( iter, stlContainer ) {     
		DeletePtr( *iter );             
	}
}

template< typename _TData >
void Destroy( _TData& stlContainer ) {
	DeleteContents( stlContainer );
	stlContainer.clear();
}

#define ABSTRACT_GETSET( type, name )			\
virtual typename Param<type>::Type		name() const = 0;	\
virtual void			name(typename Param<type>::Type val) = 0;

#define DECLARE_GETSET( name )			\
Param<decltype(m_##name)>::Type		name() const {					\
	return m_##name;								\
}												\
void			name(Param<decltype(m_##name)>::Type val) {			\
	m_##name = val;								\
}