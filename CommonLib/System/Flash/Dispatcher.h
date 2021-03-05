#pragma once

#include "System/Typedefs.h"
#include "System/Functors/TypeList.h"
#include "System/Singleton.h"
#include "TinyXML/tinyxml2.h"

using namespace tinyxml2;
class FlashEngine;

template<typename _TReturn, typename _TList>
class Dispatcher;

template<typename _TReturn>
class Dispatcher<_TReturn, TNull> {
public:
	typedef Functor<_TReturn, TNull>	THandler;

protected:
	THandler	m_Handler;

	template<typename _TReturn>
	struct Helper {
		static void	Dispatch(FlashEngine* engine, const THandler& handler) {
			engine->SetReturnValue(TypeConvert<_TReturn>::Format(handler()));
		}
	};

	template<>
	struct Helper<void> {
		static void	Dispatch(FlashEngine* engine, const THandler& handler) {
			handler();
		}
	};

public:
	template< typename TFunction >
	void	Register(TFunction func) {
		m_Handler.Set(func);
	}

	template< class TObject, typename TMemberFunction >
	void Register(TObject* obj, TMemberFunction func) {
		m_Handler.Set(obj, func);
	}

	void	Dispatch(FlashEngine* engine, XMLHandle* element) {
		Helper<_TReturn>::Dispatch(engine, m_Handler);
	}
};

template<typename _TReturn, typename _TParam1>
class Dispatcher<_TReturn, TYPELIST_1(_TParam1)> {
public:
	typedef Functor<_TReturn, TYPELIST_1(_TParam1)>	THandler;

protected:
	THandler	m_Handler;

	template<typename _TReturn>
	struct Helper {
		static void	Dispatch(FlashEngine* engine, const THandler& handler, const Char* encodedParam1) {
			engine->SetReturnValue( TypeConvert<_TReturn>::Format(handler(TypeConvert<_TParam1>::Parse(encodedParam1))) );
		}
	};

	template<>
	struct Helper<void> {
		static void	Dispatch(FlashEngine* engine, const THandler& handler, const Char* encodedParam1) {
			handler(TypeConvert<_TParam1>::Parse(encodedParam1));
		}
	};

public:
	template< typename TFunction >
	void	Register( TFunction func ) {
		m_Handler.Clear();
		m_Handler.AddListener(func);
	}

	template< class TObject, typename TMemberFunction >
	void Register(TObject* obj, TMemberFunction func) {
		m_Handler.Clear();
		m_Handler.AddListener(obj, func);
	}

	void	Dispatch(FlashEngine* engine, XMLHandle* element) {
		XMLHandle hArguments = element->FirstChildElement("arguments");

		const Char* param1 = hArguments.FirstChild().FirstChild().ToNode()->Value();
		Helper<_TReturn>::Dispatch(engine, m_Handler, param1);
	}
};

template<typename _TReturn, typename _TParam1, typename _TParam2>
class Dispatcher<_TReturn, TYPELIST_2(_TParam1, _TParam2)> {
public:
	typedef Functor<_TReturn, TYPELIST_2(_TParam1, _TParam2)>	THandler;

protected:
	THandler	m_Handler;

	template<typename _TReturn>
	struct Helper {
		static void	Dispatch(FlashEngine* engine, const THandler& handler, const Char* encodedParam1, const Char* encodedParam2) {
			engine->SetReturnValue(TypeConvert<_TReturn>::Format(handler(TypeConvert<_TParam1>::Parse(encodedParam1), TypeConvert<_TParam2>::Parse(encodedParam2))));
		}
	};

	template<>
	struct Helper<void> {
		static void	Dispatch(FlashEngine* engine, const THandler& handler, const Char* encodedParam1, const Char* encodedParam2) {
			handler(TypeConvert<_TParam1>::Parse(encodedParam1), TypeConvert<_TParam2>::Parse(encodedParam2));
		}
	};

public:
	template< typename TFunction >
	void	Register(TFunction func) {
		m_Handler.Set(func);
	}

	template< class TObject, typename TMemberFunction >
	void Register(TObject* obj, TMemberFunction func) {
		m_Handler.Set(obj, func);
	}

	void	Dispatch(FlashEngine* engine, XMLHandle* element) {
		XMLHandle hArguments = element->FirstChildElement("arguments");

		const Char* param1 = hArguments.FirstChild().FirstChild().Node()->Value();
		const Char* param2 = hArguments.FirstChild().ToNode()->NextSibling().FirstChild().Node()->Value();

		Helper<_TReturn>::Dispatch(engine, m_Handler, param1, param2);
	}
};

