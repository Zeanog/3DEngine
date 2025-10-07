#pragma once

#include "System/AssetManager.h"
#include "AudioLoader.h"
#include "System/Audio/Sound.h"
#include "System/Singleton.h"

class SoundManager {
	CLASS_TYPEDEFS(SoundManager)
	SINGLETON_DECLARATIONS(TSelf);

public:
	typedef AssetManager<Sound>	TContainer;
	typedef std::map<StaticString, AudioLoader*>	THandlerContainer;

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