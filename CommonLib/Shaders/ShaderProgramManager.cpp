#include "ShaderProgramManager.h"

#include "../System/Json.h"

ShaderProgramManager::ShaderProgramManager() {
}

ShaderProgramManager::~ShaderProgramManager() {
}

const ShaderProgramManager::TAsset*	ShaderProgramManager::Get(const StaticString& name) {
	TAsset* asset = m_Container.Find(name);
	/*if (asset) {
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

	m_Container.Add(path, asset);*/
	return asset;

}