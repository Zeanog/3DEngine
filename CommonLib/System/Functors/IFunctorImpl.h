#pragma once

#include "System/Typedefs.h"
#include "TypeList.h"
#include "System/Functors/ParamType.h"

class IBaseFunctorImpl {
public:
	typedef	TNull	TParam1;
	typedef	TNull	TParam2;
	typedef	TNull	TParam3;
	typedef	TNull	TParam4;
	typedef	TNull	TParam5;
	typedef	TNull	TParam6;

	virtual Bool	operator==(const IBaseFunctorImpl& rhs) const {
		return false;
	}

	virtual void	AddRef() {}
	virtual void	Release() {}
};

template< typename _TReturn, class _TList >
class IFunctorImpl;

template< typename _TReturn >
class IFunctorImpl<_TReturn, TNull> : public IBaseFunctorImpl {
public:
	typedef _TReturn	TReturn;

public:
	virtual TReturn	operator()() = 0;
	virtual TReturn	operator()() const = 0;
	virtual Bool	IsValid() const = 0;
	
};

template< typename _TReturn, typename _TParam1 >
class IFunctorImpl<_TReturn, TYPELIST_1(_TParam1) > : public IBaseFunctorImpl {
public:
	typedef _TReturn	TReturn;
	typedef typename ParamType<_TParam1>::Type	TParam1;

protected:

public:
	virtual TReturn	operator()(TParam1 param1 ) = 0;
	virtual TReturn	operator()(TParam1 param1) const = 0;
	virtual Bool	IsValid() const = 0;
};

template< typename _TReturn, typename _TParam1, typename _TParam2 >
class IFunctorImpl<_TReturn, TYPELIST_2(_TParam1, _TParam2) > : public IBaseFunctorImpl {
public:
	typedef _TReturn	TReturn;
	typedef typename ParamType<_TParam1>::Type	TParam1;
	typedef typename ParamType<_TParam2>::Type	TParam2;

protected:

public:
	virtual TReturn	operator()( TParam1 param1, TParam2 param2 ) = 0;
	virtual TReturn	operator()(TParam1 param1, TParam2 param2) const = 0;
	virtual Bool	IsValid() const = 0;
};

template< typename _TReturn, typename _TParam1, typename _TParam2, typename _TParam3 >
class IFunctorImpl<_TReturn, TYPELIST_3(_TParam1, _TParam2, _TParam3) > : public IBaseFunctorImpl {
public:
	typedef _TReturn	TReturn;
	typedef typename ParamType<_TParam1>::Type	TParam1;
	typedef typename ParamType<_TParam2>::Type	TParam2;
	typedef typename ParamType<_TParam3>::Type	TParam3;

protected:

public:
	virtual TReturn	operator()(TParam1 param1, TParam2 param2, TParam3 param3) = 0;
	virtual TReturn	operator()(TParam1 param1, TParam2 param2, TParam3 param3) const = 0;
	virtual Bool	IsValid() const = 0;
};

template< typename _TReturn, typename _TParam1, typename _TParam2, typename _TParam3, typename _TParam4 >
class IFunctorImpl<_TReturn, TYPELIST_4(_TParam1, _TParam2, _TParam3, _TParam4) > : public IBaseFunctorImpl {
public:
	typedef _TReturn	TReturn;
	typedef typename ParamType<_TParam1>::Type	TParam1;
	typedef typename ParamType<_TParam2>::Type	TParam2;
	typedef typename ParamType<_TParam3>::Type	TParam3;
	typedef typename ParamType<_TParam4>::Type	TParam4;

protected:

public:
	virtual TReturn	operator()(TParam1 param1, TParam2 param2, TParam3 param3, TParam4 param4) = 0;
	virtual TReturn	operator()(TParam1 param1, TParam2 param2, TParam3 param3, TParam4 param4) const = 0;
	virtual Bool	IsValid() const = 0;
};

template< typename _TReturn, typename _TParam1, typename _TParam2, typename _TParam3, typename _TParam4, typename _TParam5 >
class IFunctorImpl<_TReturn, TYPELIST_5(_TParam1, _TParam2, _TParam3, _TParam4, _TParam5) > : public IBaseFunctorImpl {
public:
	typedef _TReturn	TReturn;
	typedef typename ParamType<_TParam1>::Type	TParam1;
	typedef typename ParamType<_TParam2>::Type	TParam2;
	typedef typename ParamType<_TParam3>::Type	TParam3;
	typedef typename ParamType<_TParam4>::Type	TParam4;
	typedef typename ParamType<_TParam5>::Type	TParam5;

protected:

public:
	virtual TReturn	operator()(TParam1 param1, TParam2 param2, TParam3 param3, TParam4 param4, TParam5 param5) = 0;
	virtual TReturn	operator()(TParam1 param1, TParam2 param2, TParam3 param3, TParam4 param4, TParam5 param5) const = 0;
	virtual Bool	IsValid() const = 0;
};

template< class TChild, typename _TReturn, typename _TList >
class IFunctorImpl_Compare : public IFunctorImpl<_TReturn, _TList> {
	INHERITEDCLASS_TYPEDEFS(IFunctorImpl_Compare, TEMPLATE_2(IFunctorImpl, _TReturn, _TList));

public:
	using TSuper::operator==;
	virtual Bool	operator==(const TChild& rhs) const = 0;
	//virtual Bool	IsValid() const = 0;
};