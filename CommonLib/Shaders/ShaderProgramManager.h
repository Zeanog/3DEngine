#pragma once

#include "System\AssetManager.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include "System\Singleton.h"

class ShaderProgramManager {
	CLASS_TYPEDEFS(ShaderProgramManager)
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

	const TAsset*	Get(const String& vertPath, const String& fragPath) {
		return Get(StaticString(vertPath.CStr()), StaticString(fragPath.CStr()));
	}

	const TAsset*	Get(const StaticString& vertPath, const StaticString& fragPath);

	void	ReloadAll();

protected:
	Bool	Load(const Char* key, const StaticString& vertPath, const StaticString& fragPath, TAsset* asset) {
		assert(asset);

		if (!asset->Create(vertPath, fragPath, nullptr)) {
			DeletePtr(asset);
			return NULL;
		}


		m_Container.Add(key, asset);
		return true;
	}
};