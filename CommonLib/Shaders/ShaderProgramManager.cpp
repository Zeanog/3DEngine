#include "ShaderProgramManager.h"

#include "rapidjson/rapidjson.h"
#include "ShaderManager_Vertex.h"

ShaderProgramManager::ShaderProgramManager() {
}

ShaderProgramManager::~ShaderProgramManager() {
}

const ShaderProgramManager::TAsset*	ShaderProgramManager::Get(const StaticString& vertPath, const StaticString& fragPath) {
	try {
		TAsset* asset{};
		//TODO: Please find a better way of doing this!!!!!
		UInt32 pathLength = vertPath.Length() + fragPath.Length() + 1;
		STACK_STRING(uniqueName, pathLength);
		strcat_s(uniqueName.Str(), pathLength, vertPath.CStr());
		strcat_s(uniqueName.Str(), pathLength, fragPath.CStr());
		//TODO: Please find a better way of doing this!!!!!

		if (m_Container.Find(uniqueName.CStr(), asset)) {
			return asset;
		}

		asset = new TAsset();
		Load(uniqueName.CStr(), vertPath, fragPath, asset);
		return asset;
	}
	catch(...){
		return nullptr;
	}
}

void ShaderProgramManager::ReloadAll() {
}