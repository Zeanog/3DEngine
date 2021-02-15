#pragma once

#include "IFunctorImpl.h"
#include <type_traits>
#include <typeinfo>


template< class TFunctorOwner, class _TObject, class _TMemberFunctionPointer >
class AMemberFunctionHandler : public IFunctorImpl_Compare<AMemberFunctionHandler<TFunctorOwner, _TObject, _TMemberFunctionPointer>, typename TFunctorOwner::TReturn, typename TFunctorOwner::TParamList> {
	INHERITEDCLASS_TYPEDEFS(AMemberFunctionHandler, TEMPLATE_3(IFunctorImpl_Compare, TEMPLATE_3(AMemberFunctionHandler, TFunctorOwner, _TObject, _TMemberFunctionPointer), typename TFunctorOwner::TReturn, typename TFunctorOwner::TParamList));

public:
	typedef typename TFunctorOwner::TReturn	TReturn;

	typedef _TObject*	TObjectPointer;
	typedef _TMemberFunctionPointer	TMemberFunctionPointer;

protected:
	TObjectPointer	m_pObj;
	TMemberFunctionPointer	m_pMemberFunction;

	AMemberFunctionHandler(TObjectPointer pObj, TMemberFunctionPointer pMemberFunction) {
		m_pObj = pObj;
		m_pMemberFunction = pMemberFunction;

		m_RefCount = 0;
		AddRef();
	}

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

	virtual Bool	IsValid() const override {
		return !!m_pObj && !!m_pMemberFunction;
	}

	virtual Bool	operator==(const TSelf& rhs) const override {
		if (m_pObj != rhs.m_pObj) {
			return false;
		}

		if (m_pMemberFunction != rhs.m_pMemberFunction) {
			return false;
		}

		return true;
	}
};

template< class TFunctorOwner, class _TParamList, class _TObject, class _TMemberFunctionPointer >
class MemberFunctionHandler;

template< class TFunctorOwner, class _TObject, class _TMemberFunctionPointer >
class MemberFunctionHandler<TFunctorOwner, TNull, _TObject, _TMemberFunctionPointer> : public AMemberFunctionHandler<TFunctorOwner, _TObject, _TMemberFunctionPointer> {
	INHERITEDCLASS_TYPEDEFS( MemberFunctionHandler, TEMPLATE_3(AMemberFunctionHandler, TFunctorOwner, _TObject, _TMemberFunctionPointer));

public:
	MemberFunctionHandler() {}
	MemberFunctionHandler(typename TSuper::TObjectPointer pObj, typename TSuper::TMemberFunctionPointer pMemberFunction) : TSuper(pObj, pMemberFunction) {
	}

	typename std::conditional<std::is_same<typename TSuper::TReturn, void>::value, void, typename TSuper::TReturn>::type 	operator()() {
		return (this->m_pObj->*this->m_pMemberFunction)();
	}

	typename std::conditional<std::is_same<typename TSuper::TReturn, void>::value, void, typename TSuper::TReturn>::type	operator()() const {
		return (this->m_pObj->*this->m_pMemberFunction)();
	}
};

template< class TFunctorOwner, class _TParam1, class _TObject, class _TMemberFunctionPointer >
class MemberFunctionHandler<TFunctorOwner, TYPELIST_1(_TParam1), _TObject, _TMemberFunctionPointer> : public AMemberFunctionHandler<TFunctorOwner, _TObject, _TMemberFunctionPointer> {
	INHERITEDCLASS_TYPEDEFS(MemberFunctionHandler, TEMPLATE_3(AMemberFunctionHandler, TFunctorOwner, _TObject, _TMemberFunctionPointer));

public:
	MemberFunctionHandler() {}
	MemberFunctionHandler(TSuper::TObjectPointer pObj, TSuper::TMemberFunctionPointer pMemberFunction) : TSuper(pObj, pMemberFunction) {
	}

	typename std::conditional<std::is_same<TSuper::TReturn, void>::value, void, TSuper::TReturn>::type	operator()( typename TSuper::TParam1 param1 ) {
		return (this->m_pObj->*this->m_pMemberFunction)( param1 );
	}

	typename std::conditional<std::is_same<TSuper::TReturn, void>::value, void, TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1) const {
		return (this->m_pObj->*this->m_pMemberFunction)(param1);
	}
};

template< class TFunctorOwner, class _TParam1, class _TParam2, class _TObject, class _TMemberFunctionPointer >
class MemberFunctionHandler<TFunctorOwner, TYPELIST_2(_TParam1, _TParam2), _TObject, _TMemberFunctionPointer> : public AMemberFunctionHandler<TFunctorOwner, _TObject, _TMemberFunctionPointer> {
	INHERITEDCLASS_TYPEDEFS(MemberFunctionHandler, TEMPLATE_3(AMemberFunctionHandler, TFunctorOwner, _TObject, _TMemberFunctionPointer));

public:
	MemberFunctionHandler() {}
	MemberFunctionHandler(TSuper::TObjectPointer pObj, TSuper::TMemberFunctionPointer pMemberFunction) : TSuper(pObj, pMemberFunction) {
	}

	typename std::conditional<std::is_same<TSuper::TReturn, void>::value, void, TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2) {
		return (this->m_pObj->*this->m_pMemberFunction)(param1, param2);
	}

	typename std::conditional<std::is_same<TSuper::TReturn, void>::value, void, TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2) const {
		return (this->m_pObj->*this->m_pMemberFunction)(param1, param2);
	}
};

template< class TFunctorOwner, class _TParam1, class _TParam2, class _TParam3, class _TObject, class _TMemberFunctionPointer >
class MemberFunctionHandler<TFunctorOwner, TYPELIST_3(_TParam1, _TParam2, _TParam3), _TObject, _TMemberFunctionPointer> : public AMemberFunctionHandler<TFunctorOwner, _TObject, _TMemberFunctionPointer> {
	INHERITEDCLASS_TYPEDEFS(MemberFunctionHandler, TEMPLATE_3(AMemberFunctionHandler, TFunctorOwner, _TObject, _TMemberFunctionPointer));

public:
	MemberFunctionHandler() {}
	MemberFunctionHandler(TSuper::TObjectPointer pObj, TSuper::TMemberFunctionPointer pMemberFunction) : TSuper(pObj, pMemberFunction) {
	}

	typename std::conditional<std::is_same<TSuper::TReturn, void>::value, void, TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2, typename TSuper::TParam3 param3) {
		return (this->m_pObj->*this->m_pMemberFunction)(param1, param2, param3);
	}

	typename std::conditional<std::is_same<TSuper::TReturn, void>::value, void, TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2, typename TSuper::TParam3 param3) const {
		return (this->m_pObj->*this->m_pMemberFunction)(param1, param2, param3);
	}
};