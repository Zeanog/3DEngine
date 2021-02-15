#include "MeshManager.h"
#include "ModelLoader_FBX.h"

MeshManager::MeshManager() {
	m_Loaders[StaticString(".fbx")] = new MeshLoader_FBX();
	m_Loaders[StaticString(".mesh")] = new MeshLoader_Simple();
}

MeshManager::~MeshManager() {
	Destroy(m_Loaders);
}

Neo::Mesh* MeshManager::Get(const StaticString& path) {
	//STACK_STRING(fullPath, 256 + relativePath.Length());
	//
	//	Int32 fullPathLength = GetCurrentDirectory(fullPath.Length(), fullPath.CStr());
	//	String::StrCpy(fullPath.CStr() + fullPathLength, fullPath.Length(), "/");
	//	++fullPathLength;
	//	String::StrCpy(fullPath.CStr() + fullPathLength, fullPath.Length(), relativePath.CStr());

	Neo::Mesh* asset = m_Assets.Find(path);
	if (asset) {
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