#include "MeshManager.h"
#include "Rendering/ModelLoaders/MeshLoader_FBX.h"

MeshManager::MeshManager() {
	m_Loaders.Add(StaticString(".fbx"), new MeshLoader_FBX());
}

MeshManager::~MeshManager() {
	Destroy(m_Loaders);
}

Neo::Mesh* MeshManager::Get(const ModelDef& def) {
	Neo::Mesh* asset{};
	if (m_Assets.Find(def.Mesh, asset)) {
		return asset;
	}

	asset = new Neo::Mesh();
	if (!LoadMesh(def, asset)) {
		DeletePtr(asset);
		return NULL;
	}

	m_Assets.Add(def.Mesh, asset);
	return asset;
}

void MeshManager::ReloadAll() {
	FOREACH(iter, m_Assets) {
		//LoadMesh(iter->first, iter->second);
	}
}