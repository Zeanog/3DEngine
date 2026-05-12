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

#include <type_traits>
template<typename TData>
class ATypeInfo {
protected:
	ATypeInfo() {}

public:
	typedef TData TDecorated;
	enum : Bool {
		IsClass = std::is_class<TData>::value,
		IsPointer = std::is_pointer<TData>::value,
		IsUnion = std::is_union<TData>::value
	};

	template<typename TTo>
	constexpr Bool	CanConvertTo() const {
		return std::is_convertible<TData, TTo>::value;
	}
};

template<typename TData>
class TypeInfo : public ATypeInfo<TData> {
public:
	typedef TData	TUndecorated;
};

template<typename TData>
class TypeInfo<const TData> : public ATypeInfo<const TData> {
public:
	typedef TData	TUndecorated;
};

template<typename TData>
class TypeInfo<TData&> : public ATypeInfo<TData&> {
public:
	typedef TData	TUndecorated;
};

template<typename TData>
class TypeInfo<const TData&> : public ATypeInfo<const TData&> {
public:
	typedef TData	TUndecorated;
};

template<typename TData>
class TypeInfo<TData*> : public ATypeInfo<TData*> {
public:
	typedef TData	TUndecorated;
};

template<typename TData>
class TypeInfo<const TData*> : public ATypeInfo<const TData*> {
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
	for( auto&& iterName = ContainerIterator<decltype(stlContainer)>::Begin(stlContainer), &&iterName##End = ContainerIterator<decltype(stlContainer)>::End(stlContainer); iterName != iterName##End; iterName++ )

#define FOREACH_CONST( iterName, stlContainer )	\
	for( auto&& iterName = ContainerIterator<decltype(stlContainer)>::Begin(stlContainer), &&iterName##End = ContainerIterator<decltype(stlContainer)>::End(stlContainer); iterName != iterName##End; iterName++ )

#define FOREACH_REV( iterName, stlContainer )	\
	for( auto&& iterName = ContainerIterator<decltype(stlContainer)>::ReverseBegin(stlContainer), &&iterName##End = ContainerIterator<decltype(stlContainer)>::ReverseEnd(stlContainer); iterName != iterName##End; iterName++ )

#define FOREACH_CONST_REV( iterName, stlContainer )	\
	for( auto&& iterName = ContainerIterator<decltype(stlContainer)>::ReverseBegin(stlContainer), &&iterName##End = ContainerIterator<decltype(stlContainer)>::ReverseEnd(stlContainer); iterName != iterName##End; iterName++ )

#define FOR( indexType, indexName, startIndex, endIndex, indexStride) \
	for( indexType indexName = (startIndex); indexName < (endIndex); indexName += (indexStride) )

#define STACK_ALLOC( type, num )	((num > 0) ? (type*)_alloca( sizeof(type)*num ) : nullptr)

#define CLASS_TYPEDEFS( classType )	\
public:								\
	typedef classType	TSelf;		\
private:

#define ABSTRACT_CLASS_TYPEDEFS( classType )	\
	CLASS_TYPEDEFS( classType )					\
protected:										\
	classType()

#define INHERITED_CLASS_TYPEDEFS( classType, superClass )	\
	CLASS_TYPEDEFS( classType )								\
public:														\
	typedef superClass		TSuper;							\
private:

#define ABSTRACT_INHERITED_CLASS_TYPEDEFS( classType, superClass )	\
	CLASS_TYPEDEFS( classType )								\
public:														\
	typedef superClass		TSuper;							\
protected:													\
	classType()

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

template< class _TContainer >
class DeleteContentsHelper {
public:
	typedef _TContainer	TContainer;

public:
	static void Delete(TContainer& container ) {
		FOREACH(iter, container) {
			DeletePtr(*iter);
		}
	}
};

template< class _TContainer >
void DeleteContents(_TContainer& stlContainer ) {
	DeleteContentsHelper<_TContainer>::Delete(stlContainer);
}

template< typename _TData >
void Destroy( _TData& stlContainer ) {
	DeleteContents( stlContainer );
	stlContainer.Clear();
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

#define DECLARE_GETSET( propName )			\
Param<decltype(m_##propName)>::Type		propName() const {					\
	return m_##propName;								\
}												\
void			propName(Param<decltype(m_##propName)>::Type val) {			\
	m_##propName = val;								\
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
protected:									\
type	m_##propName{};						\
public:										\
DEFINE_GETSET_EX( propName, m_##propName )	\
private:

#define DEFINE_GETSET( propName )	DEFINE_GETSET_EX(propName, m_##propName)