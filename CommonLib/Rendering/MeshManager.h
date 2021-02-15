#pragma once

#include "System\AssetManager.h"
#include "Rendering\Model.h"
#include "System\Singleton.h"

class MeshManager {
	SINGLETON_DECLARATIONS(MeshManager);

public:
	typedef AssetManager<Mesh>	TContainer;
	typedef TContainer::TAsset	TAsset;

protected:
	TContainer			m_Container;

public:
	~MeshManager();

	void		Shutdown() {
		m_Container.Shutdown();
	}

	void			Set(const Char* path, TAsset* asset) {
		Set(StaticString(path), asset);
	}

	void			Set(const String& path, TAsset* asset) {
		Set(path.CStr(), asset);
	}

	void			Set(const StaticString& path, TAsset* asset) {
		m_Container.Add(path, asset);
	}

	const TAsset*	Get(const Char* path) {
		return Get(StaticString(path));
	}

	const TAsset*	Get(const String& path) {
		return Get(path.CStr());
	}

	const TAsset*	Get(const StaticString& path);

	void	ReloadAll();
};