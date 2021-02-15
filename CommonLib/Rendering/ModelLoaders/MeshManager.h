#pragma once

#pragma once

#include "System/AssetManager.h"
#include "Rendering/Mesh.h"
#include "Rendering/ModelLoaders/ModelLoader_FBX.h"
#include "System/Singleton.h"

class MeshManager {
	SINGLETON_DECLARATIONS(MeshManager);

public:
	typedef AssetManager<Neo::Mesh>	TContainer;
	typedef std::map<StaticString, AMeshLoader*>	THandlerContainer;

protected:
	TContainer			m_Assets;

	THandlerContainer	m_Loaders;

public:
	~MeshManager();

	void		Shutdown() {
		m_Assets.Shutdown();
	}

	Neo::Mesh*	Get(const Char* path) {
		return Get(StaticString(path));
	}

	Neo::Mesh*	Get(const String& path) {
		return Get(path.CStr());
	}

	Neo::Mesh*	Get(const StaticString& path);

	void	ReloadAll();

protected:
	Bool	LoadModel(const StaticString& fp, Neo::Mesh* asset) {
		assert(asset);

		THandlerContainer::iterator iter = m_Loaders.find(StaticString(FilePath::GetExtension(fp)));
		if (iter == m_Loaders.end()) {
			return false;
		}

		AMeshLoader* loader = iter->second;
		if( !loader->Load(fp) ) {
			return false;
		}

		if( !asset->UploadData(*loader) ) {
			return false;
		}

		loader->Clear();
		return true;
	}
};