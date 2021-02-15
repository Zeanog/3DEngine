#pragma once

#include "Typedefs.h"

#include <list>

template< typename _TData >
class ALinkedList {
	CLASS_TYPEDEFS(ALinkedList);

public:
	typedef _TData	TData;
	typedef std::list<TData>	TContainer;
	typedef typename TContainer::iterator	TIterator;
	typedef typename TContainer::const_iterator	TConstIterator;
	typedef typename TContainer::value_type	TValue;

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

	TData&	operator[](UInt32 index) {
		return m_Data[index];
	}

	const TData&	operator[](UInt32 index) const {
		return m_Data[index];
	}

	UInt32	Length() const {
		return m_Data.size();
	}

	void	Add(const TData& data) {
		m_Data.push_back(data);
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

	void	EnsureSize(UInt32 size) {
		if (size > Length()) {
			m_Data.reserve(size);
		}
	}

	void	Resize(UInt32 size) {
		m_Data.resize(size);
	}

	void	Clear() {
		m_Data.clear();
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
	INHERITEDCLASS_TYPEDEFS(LinkedList, ALinkedList<_TData>);

public:
public:
	typedef typename _TData	TData;

	typedef typename TSuper::TIterator		TIterator;
	typedef typename TSuper::TConstIterator	TConstIterator;

protected:

public:
};

template<class TData>
class ContainerIterator<LinkedList<TData>> {
public:
	typedef typename LinkedList<TData>	TContainer;
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;
	typedef typename TUndecorated::TIterator		Iterator;
	typedef typename TUndecorated::TConstIterator	ConstIterator;

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
};