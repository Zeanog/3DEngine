#pragma once

#include "System\Typedefs.h"
#include "Functor.h"
#include "System/LinkedList.h"

template< class _TList >
class AMultiFunctor : public IFunctorImpl<void, _TList> {
	INHERITEDCLASS_TYPEDEFS(AMultiFunctor, TEMPLATE_2(IFunctorImpl, void, _TList));

public:
	typedef _TList		TParamList;

	typedef typename TSuper::TParam1	TParam1;
	typedef typename TSuper::TParam2	TParam2;
	typedef typename TSuper::TParam3	TParam3;
	typedef typename TSuper::TParam4	TParam4;
	typedef typename TSuper::TParam5	TParam5;

	typedef LinkedList<Functor<void, TParamList>>	TListenerContainer;
	typedef typename TListenerContainer::TValue	TValue;

protected:
	TListenerContainer			m_Listeners;

public:
	/*AMultiFunctor& operator+=(const AMultiFunctor<_TList>& rhs) {
		m_Listeners.Add(&rhs);
		return *this;
	}

	AMultiFunctor&	operator+=(const Functor<void, _TList>& rhs) {
		m_Listeners.Add(&rhs);
		return *this;
	}

	AMultiFunctor& operator-=(const AMultiFunctor<_TList>& rhs) {
		m_Listeners.Remove(&rhs);
		return *this;
	}

	AMultiFunctor&	operator-=(const Functor<void, _TList>& rhs) {
		m_Listeners.Remove(&rhs);
		return *this;
	}*/

	template< typename TFunction >
	void AddListener(TFunction func) {
		m_Listeners.Add(Functor<void, TParamList>(func));
	}

	template< class TObject, typename TMemberFunction >
	void AddListener(TObject* obj, TMemberFunction func) {
		m_Listeners.Add(Functor<void, TParamList>(obj, func));
	}

	template< typename TFunction >
	void RemoveListener(TFunction func) {
		FunctionHandler<Functor<void, _TList>, TFunction>	handler(func);
		m_Listeners.Remove(handler);
	}

	template< class TObject, typename TMemberFunction >
	void RemoveListener(TObject* obj, TMemberFunction func) {
		MemberFunctionHandler<Functor<void, _TList>, _TList, TObject, TMemberFunction>	handler(obj, func);
		m_Listeners.Remove(handler);
	}

	void	Clear() {
		m_Listeners.Clear();
	}

	virtual Bool	IsValid() const override {
		return true;//m_Listeners.Length() > 0;
	}
};

template< class _TList >
class MultiFunctor;

template<>
class MultiFunctor<TNull> : public AMultiFunctor<TNull> {
	INHERITEDCLASS_TYPEDEFS(MultiFunctor, AMultiFunctor<TNull>);

protected:
	typedef typename TSuper::TListenerContainer	TListenerContainer;

public:
	MultiFunctor() {
	}

	virtual void	operator()() override {
		//TListenerContainer::TValue	listener;
		FOREACH_CONST(iter, m_Listeners) {
			if (iter->IsValid()) {
				(*iter)();
			}
		}

		return;
	}
	virtual void	operator()() const override {
		FOREACH_CONST(iter, m_Listeners) {
			if (iter->IsValid()) {
				(*iter)();
			}
		}

		return;
	}
};

template<typename _TParam1>
class MultiFunctor<TYPELIST_1(_TParam1)> : public AMultiFunctor<TYPELIST_1(_TParam1)> {
	INHERITEDCLASS_TYPEDEFS(MultiFunctor<TYPELIST_1(_TParam1)>, AMultiFunctor<TYPELIST_1(_TParam1)>);

public:
	typedef typename TSuper::TParam1	TParam1;
	typedef typename TSuper::TListenerContainer	TListenerContainer;

public:
	MultiFunctor() {
	}

	virtual void	operator()(TParam1 param1) override {
		//typename TListenerContainer::TValue	listener;
		FOREACH_CONST(iter, this->m_Listeners) {
			if (iter->IsValid()) {
				(*iter)(param1);
			}
		}

		return;
	}
	virtual void	operator()(TParam1 param1) const override {
		FOREACH_CONST(iter, this->m_Listeners) {
			if (iter->IsValid()) {
				(*iter)(param1);
			}
		}

		return;
	}
};

template<typename _TParam1, typename _TParam2>
class MultiFunctor<TYPELIST_2(_TParam1, _TParam2)> : public AMultiFunctor<TYPELIST_2(_TParam1, _TParam2)> {
	typedef TYPELIST_2(_TParam1, _TParam2) TTypeList;
	INHERITEDCLASS_TYPEDEFS(MultiFunctor<TTypeList>, AMultiFunctor<TTypeList>);

public:
	typedef typename TSuper::TParam1	TParam1;
	typedef typename TSuper::TParam2	TParam2;
	typedef typename TSuper::TListenerContainer	TListenerContainer;

public:
	MultiFunctor() {
	}

	virtual void	operator()(TParam1 param1, TParam2 param2) override {
		FOREACH_CONST(iter, this->m_Listeners) {
			if (iter->IsValid()) {
				(*iter)(param1, param2);
			}
		}

		return;
	}
	virtual void	operator()(TParam1 param1, TParam2 param2) const override {
		FOREACH_CONST(iter, this->m_Listeners) {
			if (iter->IsValid()) {
				(*iter)(param1, param2);
			}
		}

		return;
	}
};

template<typename _TParam1, typename _TParam2, typename _TParam3>
class MultiFunctor<TYPELIST_3(_TParam1, _TParam2, _TParam3)> : public AMultiFunctor<TYPELIST_3(_TParam1, _TParam2, _TParam3)> {
	typedef TYPELIST_3(_TParam1, _TParam2, _TParam3) TTypeList;
	INHERITEDCLASS_TYPEDEFS(MultiFunctor<TTypeList>, AMultiFunctor<TTypeList>);

public:
	typedef typename TSuper::TParam1	TParam1;
	typedef typename TSuper::TParam2	TParam2;
	typedef typename TSuper::TParam3	TParam3;
	typedef typename TSuper::TListenerContainer	TListenerContainer;

public:
	MultiFunctor() {
	}

	virtual void	operator()(TParam1 param1, TParam2 param2, TParam3 param3) override {
		//typename TListenerContainer::TValue	listener;
		FOREACH_CONST(iter, this->m_Listeners) {
			if(iter->IsValid()) {
				(*iter)(param1, param2, param3);
			}
		}

		return;
	}
	virtual void	operator()(TParam1 param1, TParam2 param2, TParam3 param3) const override {
		//typename TListenerContainer::TValue	listener;
		FOREACH_CONST(iter, this->m_Listeners) {
			if (iter->IsValid()) {
				(*iter)(param1, param2, param3);
			}
		}

		return;
	}
};

template< class _TList = TNull >
using Delegate = MultiFunctor<_TList>;