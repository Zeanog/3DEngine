#pragma once

#include "StaticString.h"
#include "Map.h"

template< class _TAsset >
class AssetManager {
	CLASS_TYPEDEFS( AssetManager )

public:
	typedef _TAsset						TAsset;
	typedef std::map<StaticString, TAsset*>	TContainer;
	
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
		return m_Container.size();
	}

	void	Add( const StaticString& path, TAsset* asset ) {
		m_Container[path] = asset;
	}

	TAsset*	Find( const StaticString& path ) {
		TIterator iter = m_Container.find( path );
		if( iter == m_Container.end() ) {
			return NULL;
		}

		return iter->second;
	}

	const TAsset*	Find( const StaticString& path ) const {
		TConstIterator iter = m_Container.find( path );
		if( iter == m_Container.end() ) {
			return NULL;
		}

		return *iter;
	}

	TIterator	Begin() {
		return m_Container.begin();
	}

	TIterator	End() {
		return m_Container.end();
	}

	TConstIterator	Begin() const {
		return m_Container.begin();
	}

	TConstIterator	End() const {
		return m_Container.end();
	}

	TIterator	begin() {
		return m_Container.begin();
	}

	TIterator	end() {
		return m_Container.end();
	}

	TConstIterator	begin() const {
		return m_Container.cbegin();
	}

	TConstIterator	end() const {
		return m_Container.cend();
	}

	//
	TReverseIterator	rbegin() {
		return m_Container.rbegin();
	}

	TReverseIterator	rend() {
		return m_Container.rend();
	}

	TConstReverseIterator	crbegin() {
		return m_Container.crbegin();
	}

	TConstReverseIterator	crend() {
		return m_Container.crend();
	}
	//

	void	Clear() {
		m_Container.clear();
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