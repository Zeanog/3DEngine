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
typedef unsigned __int64	UInt64;
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

template< class TContainer >
class ContainerIterator {
public:
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;

	typedef typename TUndecorated::iterator	Iterator;
	typedef typename TUndecorated::const_iterator	ConstIterator;

	typedef typename TUndecorated::reverse_iterator	ReverseIterator;
	typedef typename TUndecorated::const_reverse_iterator	ConstReverseIterator;

	static Iterator	Begin(typename TUndecorated& container) {
		return container.begin();
	}
	static ConstIterator	Begin(const typename TUndecorated& container) {
		return container.cbegin();
	}
	static Iterator	End(typename TUndecorated& container) {
		return container.end();
	}
	static ConstIterator	End(const typename TUndecorated& container) {
		return container.cend();
	}

	static ReverseIterator	ReverseBegin(typename TUndecorated& container) {
		return container.rbegin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		return container.crbegin();
	}
	static ReverseIterator	ReverseEnd(typename TUndecorated& container) {
		return container.rend();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		return container.crend();
	}
};

#define FOREACH( iterName, stlContainer )	\
	for( typename ContainerIterator<decltype(stlContainer)>::Iterator iterName = ContainerIterator<decltype(stlContainer)>::Begin(stlContainer), iterName##End = ContainerIterator<decltype(stlContainer)>::End(stlContainer); iterName != iterName##End; iterName++ )

#define FOREACH_CONST( iterName, stlContainer )	\
	for( typename ContainerIterator<decltype(stlContainer)>::ConstIterator iterName = ContainerIterator<decltype(stlContainer)>::Begin(stlContainer), iterName##End = ContainerIterator<decltype(stlContainer)>::End(stlContainer); iterName != iterName##End; iterName++ )

#define FOREACH_R( iterName, stlContainer )	\
	for( typename ContainerIterator<decltype(stlContainer)>::ReverseIterator iterName = ContainerIterator<decltype(stlContainer)>::ReverseBegin(stlContainer), iterName##End = ContainerIterator<decltype(stlContainer)>::ReverseEnd(stlContainer); iterName != iterName##End; iterName++ )

#define FOREACH_CONST_R( iterName, stlContainer )	\
	for( typename ContainerIterator<decltype(stlContainer)>::ConstReverseIterator iterName = ContainerIterator<decltype(stlContainer)>::ReverseBegin(stlContainer), iterName##End = ContainerIterator<decltype(stlContainer)>::ReverseEnd(stlContainer); iterName != iterName##End; iterName++ )

#define STACK_ALLOC( type, num )	(type*)_alloca( sizeof(type)*num )

#define CLASS_TYPEDEFS( classType )	\
public:								\
	typedef classType	TSelf;		\
private:

#define INHERITEDCLASS_TYPEDEFS( classType, superClass )		\
CLASS_TYPEDEFS( classType )										\
public:															\
typedef superClass		TSuper;									\
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

template< typename _TData >
void Release(_TData*& ptr) {
	if (ptr) {
		ptr->Release();
	}
	ptr = NULL;
}

#include "System\Functors\ParamType.h"

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

#define ABSTRACT_GETSET_EX( type, propName )								\
virtual typename Param<type>::Type		propName() const = 0;				\
virtual void			propName(typename Param<type>::Type val) = 0;

#define DEFINE_GETSET_EX( propName, name )									\
inline Param<decltype(name)>::Type		propName() const {					\
	return name;															\
}																			\
inline void	__fastcall propName(Param<decltype(name)>::Type val) {			\
	name = val;																\
}

#define DEFINE_MEMBER_EX( type, propName )	\
protected:											\
type	m_##propName{};										\
public:												\
DEFINE_GETSET_EX( propName, m_##propName )					\
private:

#define DEFINE_GETSET( name )	DEFINE_GETSET_EX(decltype(m_##name), name, m_##name)