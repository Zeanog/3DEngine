#pragma once

#include "StaticString.h"
#include "Map.h"

template< class _TAsset >
class AssetManager {
	CLASS_TYPEDEFS( AssetManager )

public:
	typedef _TAsset						TAsset;
	typedef Map<StaticString, TAsset*>	TContainer;
	
	typedef typename TContainer::iterator				TIterator;
	typedef typename TContainer::const_iterator			TConstIterator;
	typedef typename TContainer::reverse_iterator		TReverseIterator;
	typedef typename TContainer::const_reverse_iterator	TConstReverseIterator;

	typedef typename TContainer::iterator				iterator;
	typedef typename TContainer::const_iterator			const_iterator;
	typedef typename TContainer::reverse_iterator		reverse_iterator;
	typedef typename TContainer::const_reverse_iterator	const_reverse_iterator;

protected:
	TContainer	m_Container;

public:
	AssetManager() {
	}

	virtual ~AssetManager() {
		Shutdown();
	}

	void	Shutdown() {
		Destroy( m_Container );
	}

	UInt32	Count() const {
		return m_Container.Size();
	}

	void	Add( const StaticString& path, TAsset* asset ) {
		m_Container.Add(path, asset);
	}

	Bool	Find(const StaticString& path, TAsset*& outValue) const {
		return m_Container.Find(path, outValue);
	}

	TIterator	Begin() {
		return m_Container.begin();
	}

	TIterator	End() {
		return m_Container.end();
	}

	void	Clear() {
		m_Container.Clear();
	}
};

template< class _TAsset >
class ContainerIterator<AssetManager<_TAsset>> {
public:
	typedef typename AssetManager<_TAsset>	TContainer;
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;
	
	typedef typename TContainer::iterator				Iterator;
	typedef typename TContainer::const_iterator			ConstIterator;
	typedef typename TContainer::reverse_iterator		ReverseIterator;
	typedef typename TContainer::const_reverse_iterator	ConstReverseIterator;

	typedef typename TContainer::iterator				iterator;
	typedef typename TContainer::const_iterator			const_iterator;
	typedef typename TContainer::reverse_iterator		reverse_iterator;
	typedef typename TContainer::const_reverse_iterator	const_reverse_iterator;

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
		return container.ReverseBegin();
	}
	static ConstReverseIterator	ReverseBegin(const typename TUndecorated& container) {
		return container.ReverseBegin();
	}
	static ReverseIterator	ReverseEnd(TUndecorated& container) {
		return container.ReverseBegin();
	}
	static ConstReverseIterator	ReverseEnd(const typename TUndecorated& container) {
		return container.ReverseBegin();
	}
};