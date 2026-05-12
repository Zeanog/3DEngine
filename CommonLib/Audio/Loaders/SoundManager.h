#pragma once

#include "System/AssetManager.h"
#include "Audio/Sound.h"
#include "System/Singleton.h"

class AAudioLoader;

class SoundManager {
	CLASS_TYPEDEFS(SoundManager)
	SINGLETON_DECLARATIONS(SoundManager);

public:
	typedef AssetManager<Sound>	TContainer;
	typedef Map<StaticString, AAudioLoader*>	THandlerContainer;

protected:
	TContainer			m_Assets;

	THandlerContainer	m_Loaders;

public:
	~SoundManager();

	void		Shutdown() {
		m_Assets.Shutdown();
	}

	Sound* Get(const Char* path) {
		return Get(StaticString(path));
	}

	Sound* Get(const String& path) {
		return Get(path.CStr());
	}

	Sound* Get(const StaticString& path);

	void	ReloadAll();

protected:
	Bool	Load(const StaticString& path, Sound* asset);
};