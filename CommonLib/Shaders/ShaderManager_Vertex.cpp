#include "ShaderManager_Vertex.h"

#include "../System/Json.h"

ShaderManager_Vertex::ShaderManager_Vertex() {
}

ShaderManager_Vertex::~ShaderManager_Vertex() {
}

const ShaderManager_Vertex::TAsset*	ShaderManager_Vertex::Get(const StaticString& path, const Char* header) {
	TAsset* asset = m_Container.Find(path);
	if (asset) {
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