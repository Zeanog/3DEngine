#include "ShaderManager_Fragment.h"

ShaderManager_Fragment::ShaderManager_Fragment() {
}

ShaderManager_Fragment::~ShaderManager_Fragment() {
}

const ShaderManager_Fragment::TAsset*	ShaderManager_Fragment::Get(const StaticString& path, const Char* header) {
	TAsset* asset{};
	if (m_Container.Find(path, asset)) {
		return asset;
	}

	asset = new TAsset(StaticString("#version 440 compatibility\n"));
	if (!asset->ReadFrom(path, header)) {
		DeletePtr(asset);
		return NULL;
	}

	m_Container.Add(path, asset);
	return asset;

}

//void ShaderProgramManager::ReloadAll() {
//	for (TContainer::TIterator iter = m_Container.Begin(); iter != m_Container.End(); ++iter) {
//		iter->second->ReadFrom(iter->first);
//	}
//}