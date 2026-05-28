#pragma once

#include "Typedefs.h"

#include <list>

template< typename _TData >
class ALinkedList {
	CLASS_TYPEDEFS(ALinkedList)

public:
	typedef _TData	TData;
	typedef std::list<TData>	TContainer;
	typedef typename TContainer::value_type	TValue;

	typedef typename TContainer::iterator	TIterator;
	typedef typename TContainer::const_iterator	TConstIterator;
	typedef typename TContainer::reverse_iterator	TReverseIterator;
	typedef typename TContainer::const_reverse_iterator	TConstReverseIterator;

	typedef typename TContainer::iterator				iterator;
	typedef typename TContainer::const_iterator			const_iterator;
	typedef typename TContainer::reverse_iterator		reverse_iterator;
	typedef typename TContainer::const_reverse_iterator	const_reverse_iterator;

protected:
	TContainer	m_Data;

public:
	TIterator	Begin() {
		return m_Data.begin();
	}

	TIterator	End() {
		return m_Data.end();
	}

	TConstIterator	Begin() const {
		return m_Data.begin();
	}

	TConstIterator	End() const {
		return m_Data.end();
	}

	TIterator	begin() {
		return m_Data.begin();
	}

	TIterator	end() {
		return m_Data.end();
	}

	TConstIterator	cbegin() const {
		return m_Data.cbegin();
	}

	TConstIterator	cend() const {
		return m_Data.cend();
	}

	//
	TReverseIterator	rbegin() {
		return m_Data.rbegin();
	}

	TReverseIterator	rend() {
		return m_Data.rend();
	}

	TConstReverseIterator	crbegin() const {
		return m_Data.crbegin();
	}

	TConstReverseIterator	crend() const {
		return m_Data.crend();
	}
	//

	UInt32	Length() const {
		return m_Data.size();
	}

	void	Add(const TData& data) {
		m_Data.push_back(data);
	}

	void	AddFront(const TData& data) {
		m_Data.push_front(data);
	}

	void	Remove(const TData& data) {
		TConstIterator iter = Find(data);
		if (iter != m_Data.end()) {
			m_Data.erase(iter);
		}
	}

	Int32	AddUnique(const TData& data) {
		for (Int32 ix = 0; ix < Length(); ++ix) {
			if (m_Data[ix] == data) {
				return ix;
			}
		}
		m_Data.push_back(data);
		return Length() - 1;
	}

	TConstIterator	Find(const TData& data) const {
		FOREACH_CONST(iter, m_Data) {
			TData d = *iter;
			if (d == data) {
				return iter;
			}
		}
		return m_Data.cend();
	}

	void	Resize(UInt32 size) {
		m_Data.resize(size);
	}

	void	Clear() {
		m_Data.clear();
	}

	template<typename TFunc>
	void Sort(TFunc func) {
		m_Data.sort(func);
	}

	/*virtual Bool	ReadFrom( File& file ) {
	return true;
	}*/

	//virtual Bool	WriteTo( File& file ) const {
	//	//file.Write( Length() );
	//	//file.Write( &m_Data[0], Length() );
	//	return true;
	//}
};

template< typename _TData >
class LinkedList : public ALinkedList<_TData> {
	INHERITED_CLASS_TYPEDEFS(LinkedList, ALinkedList<_TData>);

public:
public:
	typedef typename _TData	TData;

	typedef typename TSuper::iterator	TIterator;
	typedef typename TSuper::const_iterator	TConstIterator;
	typedef typename TSuper::reverse_iterator	TReverseIterator;
	typedef typename TSuper::const_reverse_iterator	TConstReverseIterator;

	typedef typename TSuper::iterator				iterator;
	typedef typename TSuper::const_iterator			const_iterator;
	typedef typename TSuper::reverse_iterator		reverse_iterator;
	typedef typename TSuper::const_reverse_iterator	const_reverse_iterator;

protected:

public:
};

template<class TData>
class ContainerIterator<LinkedList<TData>> {
public:
	typedef typename LinkedList<TData>	TContainer;
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;
	
	typedef typename TUndecorated::iterator	Iterator;
	typedef typename TUndecorated::const_iterator	ConstIterator;

	typedef typename TUndecorated::reverse_iterator	ReverseIterator;
	typedef typename TUndecorated::const_reverse_iterator	ConstReverseIterator;

	static Iterator	Begin(TUndecorated& container) {
		return container.Begin();
	}

	static ConstIterator	Begin(const TUndecorated& container) {
		return container.Begin();
	}

	static Iterator	End(TUndecorated& container) {
		return container.End();
	}

	static ConstIterator	End(const TUndecorated& container) {
		return container.End();
	}

	static ReverseIterator	ReverseBegin(TUndecorated& container) {
		return container.rbegin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		return container.crbegin();
	}
	static ReverseIterator	ReverseEnd(TUndecorated& container) {
		return container.rend();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		return container.crend();
	}
};

template<class TData>
class ContainerIterator<const LinkedList<TData>&> {
public:
	typedef typename LinkedList<TData> TContainer;
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;

	typedef typename TUndecorated::iterator	Iterator;
	typedef typename TUndecorated::const_iterator	ConstIterator;

	typedef typename TUndecorated::reverse_iterator	ReverseIterator;
	typedef typename TUndecorated::const_reverse_iterator	ConstReverseIterator;

	static Iterator	Begin(TUndecorated& container) {
		return container.Begin();
	}

	static ConstIterator	Begin(const TUndecorated& container) {
		return container.Begin();
	}

	static Iterator	End(TUndecorated& container) {
		return container.End();
	}

	static ConstIterator	End(const TUndecorated& container) {
		return container.End();
	}

	static ReverseIterator	ReverseBegin(TUndecorated& container) {
		return container.rbegin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		return container.crbegin();
	}
	static ReverseIterator	ReverseEnd(TUndecorated& container) {
		return container.rend();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		return container.crend();
	}
};