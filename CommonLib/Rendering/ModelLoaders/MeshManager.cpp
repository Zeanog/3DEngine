#include "MeshManager.h"
#include "MeshLoader_FBX.h"

MeshManager::MeshManager() {
	m_Loaders.Add(StaticString(".fbx"), new MeshLoader_FBX());
}

MeshManager::~MeshManager() {
	Destroy(m_Loaders);
}

Neo::Mesh* MeshManager::Get(const StaticString& path) {
	Neo::Mesh* asset{};
	if (m_Assets.Find(path, asset)) {
		return asset;
	}

	asset = new Neo::Mesh();
	if (!LoadModel(path, asset)) {
		DeletePtr(asset);
		return NULL;
	}

	m_Assets.Add(path, asset);
	return asset;
}

void MeshManager::ReloadAll() {
	FOREACH(iter, m_Assets) {
		LoadModel(iter->first, iter->second);
	}
}