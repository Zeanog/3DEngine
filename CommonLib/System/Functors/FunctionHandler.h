#pragma once

#include "IFunctorImpl.h"

template< class TFunctorOwner, typename _TFunctionPointer >
class FunctionHandler : public IFunctorImpl_Compare<FunctionHandler<TFunctorOwner, _TFunctionPointer>, typename TFunctorOwner::TReturn, typename TFunctorOwner::TParamList> {
	INHERITEDCLASS_TYPEDEFS( FunctionHandler, TEMPLATE_3(IFunctorImpl_Compare, TEMPLATE_2(FunctionHandler, TFunctorOwner, _TFunctionPointer), typename TFunctorOwner::TReturn, typename TFunctorOwner::TParamList) );

public:
	typedef typename TFunctorOwner::TReturn	TReturn;

	typedef _TFunctionPointer	TFunctionPointer;

protected:
	TFunctionPointer	m_pFunction;

	Int32	m_RefCount;

public:
	void		AddRef() {
		++m_RefCount;
	}

	void		Release() {
		--m_RefCount;
		if (m_RefCount <= 0) {
			delete this;
		}
	}

	FunctionHandler(TFunctionPointer func) {
		m_pFunction = func;

		m_RefCount = 0;
		AddRef();
	}

	virtual Bool	IsValid() const override {
		return !!m_pFunction;
	}
	
	virtual Bool	operator==(const TSelf& rhs) const {
		if (m_pFunction != rhs.m_pFunction) {
			return false;
		}

		return true;
	}

	TReturn	operator()() {
		return (*m_pFunction)();
	}
	TReturn	operator()() const {
		return (*m_pFunction)();
	}

	TReturn	operator()( typename TSuper::TParam1 param1 ) {
		return (*m_pFunction)( param1 );
	}
	TReturn	operator()(typename TSuper::TParam1 param1) const {
		return (*m_pFunction)(param1);
	}

	TReturn	operator()( typename TSuper::TParam1 param1, typename TSuper::TParam2 param2 ) {
		return (*m_pFunction)( param1, param2 );
	}
	TReturn	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2) const {
		return (*m_pFunction)(param1, param2);
	}

	TReturn	operator()( typename TSuper::TParam1 param1, typename TSuper::TParam2 param2, typename TSuper::TParam3 param3 ) {
		return (*m_pFunction)( param1, param2, param3 );
	}
	TReturn	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2, typename TSuper::TParam3 param3) const {
		return (*m_pFunction)(param1, param2, param3);
	}
};

