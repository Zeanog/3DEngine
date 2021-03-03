#pragma once

#include "../System/Typedefs.h"
#include <list>

template< class _TData >
class Pool {
public:
	typedef _TData		TData;
	typedef std::list<TData*>					TContainer;
	typedef typename TContainer::iterator		Iterator;
	typedef typename TContainer::const_iterator	ConstIterator;

	typedef typename TContainer::iterator		iterator;
	typedef typename TContainer::iterator	const_iterator;

protected:
	TContainer	m_Container;

public:
	virtual ~Pool() {
	}

	virtual void Add(TData* data) {
		m_Container.push_back(data);
	}

	void Remove(TData* data) {
		m_Container.remove(data);
	}

	void Clear() {
		m_Container.clear();
	}

	UInt32 Count() const {
		return m_Container.size();
	}

	Iterator	Begin() {
		return m_Container.begin();
	}

	ConstIterator	Begin() const {
		return m_Container.begin();
	}

	Iterator	End() {
		return m_Container.end();
	}

	ConstIterator	End() const {
		return m_Container.end();
	}

	Iterator	end() {
		return m_Container.end();
	}

	ConstIterator	end() const {
		return m_Container.cend();
	}

	Iterator	begin() {
		return m_Container.begin();
	}

	ConstIterator	begin() const {
		return m_Container.cbegin();
	}
};

