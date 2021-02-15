#pragma once

#include "System\AssetManager.h"
#include "Shaders/Shader_GLSL.h"
#include "System\Singleton.h"

class ShaderManager_Fragment {
	SINGLETON_DECLARATIONS(ShaderManager_Fragment);

public:
	typedef AssetManager<Shader_Fragment_GLSL>	TContainer;
	typedef TContainer::TAsset	TAsset;

protected:
	TContainer			m_Container;

	 

public:
	~ShaderManager_Fragment();

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

	const TAsset*	Get(const Char* path, const Char* header) {
		return Get(StaticString(path), header);
	}

	const TAsset*	Get(const String& path, const Char* header) {
		return Get(path.CStr(), header);
	}

	const TAsset*	Get(const StaticString& path, const Char* header);

	//void	ReloadAll();
};
