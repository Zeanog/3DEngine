#include "MeshManager.h"

#include "../System/Json.h"

MeshManager::MeshManager() {
}

MeshManager::~MeshManager() {
}

const MeshManager::TAsset*	MeshManager::Get(const StaticString& path) {
	TAsset* asset = m_Container.Find(path);
	if (asset) {
		return asset;
	}

	File file;
	if (!file.Open(path, "rb")) {
		return NULL;
	}

	const Char* encodedData = STACK_ALLOC(Char, file.Length());

	file.Read(encodedData, file.Length());

	asset = new TAsset();
	json_value* root = json_parse(encodedData, file.Length());
	if (!asset->ReadFrom(root)) {
		DeletePtr(asset);
		return NULL;
	}

	json_value_free(root);
	root = NULL;

	file.Close();

	m_Container.Add(path, asset);
	return asset;

}

void MeshManager::ReloadAll() {
	for (TContainer::TIterator iter = m_Container.Begin(); iter != m_Container.End(); ++iter) {
		iter->second->ReadFrom(iter->first);
	}
}