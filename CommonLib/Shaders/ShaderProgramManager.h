#pragma once

#include "System\AssetManager.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include "System\Singleton.h"

class ShaderProgramManager {
	SINGLETON_DECLARATIONS(ShaderProgramManager);

public:
	typedef AssetManager<ShaderProgram_GLSL>	TContainer;
	typedef TContainer::TAsset	TAsset;

protected:
	TContainer			m_Container;

public:
	~ShaderProgramManager();

	void		Shutdown() {
		m_Container.Shutdown();
	}

	void			Set(const Char* name, TAsset* asset) {
		Set(StaticString(name), asset);
	}

	void			Set(const String& name, TAsset* asset) {
		Set(name.CStr(), asset);
	}

	void			Set(const StaticString& name, TAsset* asset) {
		m_Container.Add(name, asset);
	}

	const TAsset*	Get(const Char* name) {
		return Get(StaticString(name));
	}

	const TAsset*	Get(const String& name) {
		return Get(name.CStr());
	}

	const TAsset*	Get(const StaticString& name);

	//void	ReloadAll();
};