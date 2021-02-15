#pragma once

#include "FunctionHandler.h"
#include "MemberFunctionHandler.h"
#include "IFunctorImpl.h"

template< typename _TReturn, class _TList = TNull >
class AFunctor : public IFunctorImpl<_TReturn, _TList> {
	INHERITEDCLASS_TYPEDEFS(AFunctor, TEMPLATE_2(IFunctorImpl, _TReturn, _TList));

public:
	typedef IFunctorImpl<_TReturn, _TList>		TImpl;
	typedef _TReturn	TReturn;
	typedef _TList		TParamList;

	typedef typename TSuper::TParam1	TParam1;
	typedef typename TSuper::TParam2	TParam2;
	typedef typename TSuper::TParam3	TParam3;
	typedef typename TSuper::TParam4	TParam4;
	typedef typename TSuper::TParam5	TParam5;

protected:
	TImpl*				m_pImpl;

	AFunctor() {
		m_pImpl = NULL;
	}

	AFunctor( const TSelf& f ) {
		f.m_pImpl->AddRef();
		m_pImpl = f.m_pImpl;
	}

	AFunctor(const TSelf* f) {
		f->m_pImpl->AddRef();
		m_pImpl = f->m_pImpl;
	}

public:
	virtual ~AFunctor() {
		Clear();
	}

	virtual Bool	IsValid() const override {
		return !!m_pImpl;
	}

	void	Clear() {
		if (m_pImpl) {
			m_pImpl->Release();
			m_pImpl = NULL;
		}
	}

	Bool	operator==(const TSelf& rhs) const {
		return (*m_pImpl) == (*rhs.m_pImpl);
	}

	AFunctor&		operator=(const TSelf* rhs) {
		if (m_pImpl) {
			m_pImpl->Release();
		}

		m_pImpl = rhs->m_pImpl;

		if (m_pImpl) {
			m_pImpl->AddRef();
		}

		return *this;
	}

	AFunctor&		operator=(const TSelf& rhs) {
		if (m_pImpl) {
			m_pImpl->Release();
		}

		m_pImpl = rhs.m_pImpl;

		if (m_pImpl) {
			m_pImpl->AddRef();
		}

		return *this;
	}
};

template< typename _TReturn, class _TList=TNull >
class Functor;

template< typename _TReturn >
class Functor<_TReturn, TNull> : public AFunctor<_TReturn, TNull> {
	INHERITEDCLASS_TYPEDEFS( Functor, TEMPLATE_2(IFunctorImpl, _TReturn, TNull));

public:
	enum TSame : bool {
		Same = std::is_same<typename TSuper::TReturn, void>::value
	};
	typedef typename std::conditional<TSame::Same, void, typename TSuper::TReturn>::type	RetType;

public:
	Functor() {
	}

	template< typename TFunction >
	Functor( TFunction func ) {
		AddListener(func);
	}

	template< class TObject, typename TMemberFunction >
	Functor( TObject* obj, TMemberFunction func ) {
		AddListener(obj, func);
	}

	template< typename TFunction >
	void AddListener(TFunction func) {
		this->Clear();
		this->m_pImpl = new FunctionHandler<TSelf, TFunction>(func);
	}

	template< class TObject, typename TMemberFunction >
	void AddListener(TObject* obj, TMemberFunction func) {
		this->Clear();
		this->m_pImpl = new MemberFunctionHandler<TSelf, TNull, TObject, TMemberFunction>(obj, func);
	}

	RetType	operator()() {
		if (!this->IsValid()) {
			return RetType();
		}
		return (*this->m_pImpl)();
	}
	RetType	operator()() const {
		if (!this->IsValid()) {
			return RetType();
		}
		return (*this->m_pImpl)();
	}
};

template< typename _TReturn, typename _TParam1 >
class Functor<_TReturn, TYPELIST_1(_TParam1)> : public AFunctor<_TReturn, TYPELIST_1(_TParam1)> {
	INHERITEDCLASS_TYPEDEFS(Functor, TEMPLATE_2(IFunctorImpl, _TReturn, TYPELIST_1(_TParam1)));

public:
	Functor() {
	}

	template< typename TFunction >
	Functor(TFunction func) {
		AddListener(func);
	}

	template< class TObject, typename TMemberFunction >
	Functor(TObject* obj, TMemberFunction func) {
		AddListener(obj, func);
	}

	template< typename TFunction >
	void AddListener(TFunction func) {
		this->Clear();
		this->m_pImpl = new FunctionHandler<TSelf, TFunction>(func);
	}

	template< class TObject, typename TMemberFunction >
	void AddListener(TObject* obj, TMemberFunction func) {
		this->Clear();
		this->m_pImpl = new MemberFunctionHandler<TSelf, TYPELIST_1(_TParam1), TObject, TMemberFunction>(obj, func);
	}

	typename std::conditional<std::is_same<typename TSuper::TReturn, void>::value, void, typename TSuper::TReturn>::type	operator()( typename TSuper::TParam1 param1) {
		if (!this->IsValid()) {
			return typename TSuper::TReturn();
		}
		return (*this->m_pImpl)(param1);
	}
	typename std::conditional<std::is_same<typename TSuper::TReturn, void>::value, void, typename TSuper::TReturn>::type	operator()( typename TSuper::TParam1 param1 ) const {
		if (!this->IsValid()) {
			return typename TSuper::TReturn();
		}
		return (*this->m_pImpl)( param1 );
	}
};

template< typename _TReturn, typename _TParam1, typename _TParam2 >
class Functor<_TReturn, TYPELIST_2(_TParam1, _TParam2)> : public AFunctor<_TReturn, TYPELIST_2(_TParam1, _TParam2)> {
	INHERITEDCLASS_TYPEDEFS(Functor, TEMPLATE_2(IFunctorImpl, _TReturn, TYPELIST_2(_TParam1, _TParam2)));

public:
	Functor() {
	}

	template< typename TFunction >
	Functor(TFunction func) {
		AddListener(func);
	}

	template< class TObject, typename TMemberFunction >
	Functor(TObject* obj, TMemberFunction func) {
		AddListener(obj, func);
	}

	template< typename TFunction >
	void AddListener(TFunction func) {
		this->Clear();
		this->m_pImpl = new FunctionHandler<TSelf, TFunction>(func);
	}

	template< class TObject, typename TMemberFunction >
	void AddListener(TObject* obj, TMemberFunction func) {
		this->Clear();
		this->m_pImpl = new MemberFunctionHandler<TSelf, TYPELIST_2(_TParam1, _TParam2), TObject, TMemberFunction>(obj, func);
	}

	typename std::conditional<std::is_same<typename TSuper::TReturn, void>::value, void, typename TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2) {
		if (!this->IsValid()) {
			return typename TSuper::TReturn();
		}
		return (*this->m_pImpl)(param1, param2);
	}
	typename std::conditional<std::is_same<typename TSuper::TReturn, void>::value, void, typename TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2) const {
		if (!this->IsValid()) {
			return typename TSuper::TReturn();
		}
		return (*this->m_pImpl)(param1, param2);
	}
};

template< typename _TReturn, typename _TParam1, typename _TParam2, typename _TParam3 >
class Functor<_TReturn, TYPELIST_3(_TParam1, _TParam2, _TParam3)> : public AFunctor<_TReturn, TYPELIST_3(_TParam1, _TParam2, _TParam3)> {
	INHERITEDCLASS_TYPEDEFS(Functor, TEMPLATE_2(IFunctorImpl, _TReturn, TYPELIST_3(_TParam1, _TParam2, _TParam3)));

public:
	Functor() {
	}

	template< typename TFunction >
	Functor(TFunction func) {
		AddListener(func);
	}

	template< class TObject, typename TMemberFunction >
	Functor(TObject* obj, TMemberFunction func) {
		AddListener(obj, func);
	}

	template< typename TFunction >
	void AddListener(TFunction func) {
		this->Clear();
		this->m_pImpl = new FunctionHandler<TSelf, TFunction>(func);
	}

	template< class TObject, typename TMemberFunction >
	void AddListener(TObject* obj, TMemberFunction func) {
		this->Clear();
		this->m_pImpl = new MemberFunctionHandler<TSelf, TYPELIST_3(_TParam1, _TParam2, _TParam3), TObject, TMemberFunction>(obj, func);
	}

	typename std::conditional<std::is_same<typename TSuper::TReturn, void>::value, void, typename TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2, typename TSuper::TParam3 param3) {
		if (!this->IsValid()) {
			return typename TSuper::TReturn();
		}
		return (*this->m_pImpl)(param1, param2, param3);
	}
	typename std::conditional<std::is_same<typename TSuper::TReturn, void>::value, void, typename TSuper::TReturn>::type	operator()(typename TSuper::TParam1 param1, typename TSuper::TParam2 param2, typename TSuper::TParam3 param3) const {
		if (!this->IsValid()) {
			return typename TSuper::TReturn();
		}
		return (*this->m_pImpl)(param1, param2, param3);
	}
};