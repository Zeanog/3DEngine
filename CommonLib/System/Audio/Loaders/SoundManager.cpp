#include "SoundManager.h"
#include "AudioLoader_RIFF.h"
#include "AudioLoader_OggVorbis.h"
#include "AudioLoader_MP3.h"
#include "System/DebugConsole.h"

SoundManager::SoundManager() {
	m_Loaders[StaticString(".wav")] = new AudioLoader_RIFF();
	m_Loaders[StaticString(".ogg")] = new AudioLoader_OggVorbis();
	m_Loaders[StaticString(".mp3")] = new AudioLoader_MP3();
}

SoundManager::~SoundManager() {
	Destroy(m_Loaders);
}

Sound* SoundManager::Get(const StaticString& path) {
	Sound* asset{};
	if (m_Assets.Find(path, asset)) {
		return asset;
	}

	asset = new Sound();
	if (!Load(path, asset)) {
		DeletePtr(asset);
		return NULL;
	}

	m_Assets.Add(path, asset);
	return asset;
}

void SoundManager::ReloadAll() {
	FOREACH(iter, m_Assets) {
		Load(iter->first, iter->second);
	}
}

Bool SoundManager::Load(const StaticString& path, Sound* asset) {
	assert(asset);

	Singleton<DebugConsole>::GetInstance()->Write("Loading '%s'...\n", path.CStr());

	AAudioLoader* loader{};
	if (!m_Loaders.Find(StaticString(FilePath::GetExtension(path)), loader) || !loader->Load(path)) {
		Singleton<DebugConsole>::GetInstance()->Write("Failed to load '%s'!\n", path.CStr());
		return false;

	}

	if (!asset->UploadData(*loader)) {
		Singleton<DebugConsole>::GetInstance()->Write("Failed to upload '%s'!\n", path.CStr());
		return false;
	}

	loader->Clear();
	return true;
}