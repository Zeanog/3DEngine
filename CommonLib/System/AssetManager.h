#pragma once

#include "StaticString.h"
#include "Map.h"

template< class _TAsset >
class AssetManager {
	CLASS_TYPEDEFS( AssetManager );

public:
	typedef _TAsset						TAsset;
	typedef std::map<StaticString, TAsset*>	TContainer;
	
	typedef typename TContainer::iterator		TIterator;
	typedef typename TContainer::const_iterator	TConstIterator;

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

	void	Clear() {
		m_Container.clear();
	}
};

template< class _TAsset >
class ContainerIterator<AssetManager<_TAsset>> {
public:
	typedef typename AssetManager<_TAsset>	TContainer;
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;
	typedef typename TUndecorated::TIterator		Iterator;
	typedef typename TUndecorated::TConstIterator	ConstIterator;

	static Iterator	Begin(TContainer& container) {
		return container.Begin();
	}

	static ConstIterator	Begin(const TContainer& container) {
		return container.Begin();
	}

	static Iterator	End(TContainer& container) {
		return container.End();
	}

	static ConstIterator	End(const TContainer& container) {
		return container.End();
	}
};