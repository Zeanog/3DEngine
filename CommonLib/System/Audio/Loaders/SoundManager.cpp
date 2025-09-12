#include "SoundManager.h"
#include "AudioLoader_RIFF.h"
#include "AudioLoader_OggVorbis.h"

SoundManager::SoundManager() {
	m_Loaders[StaticString(".wav")] = new AudioLoader_RIFF();
	m_Loaders[StaticString(".ogg")] = new AudioLoader_OggVorbis();
}

SoundManager::~SoundManager() {
	Destroy(m_Loaders);
}

Sound* SoundManager::Get(const StaticString& path) {
	Sound* asset = m_Assets.Find(path);
	if (asset) {
		return asset;
	}

	asset = new Sound();
	if (!LoadSound(path, asset)) {
		DeletePtr(asset);
		return NULL;
	}

	m_Assets.Add(path, asset);
	return asset;
}

void SoundManager::ReloadAll() {
	FOREACH(iter, m_Assets) {
		LoadSound(iter->first, iter->second);
	}
}