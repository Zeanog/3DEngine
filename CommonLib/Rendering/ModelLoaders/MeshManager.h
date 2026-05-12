#pragma once

#include "System/AssetManager.h"
#include "Rendering/Mesh.h"
#include "Rendering/ModelLoaders/MeshLoader_FBX.h"
#include "System/Singleton.h"
#include "ModelLoader.h"

class MeshManager {
	CLASS_TYPEDEFS(MeshManager)
	SINGLETON_DECLARATIONS(MeshManager);

public:
	typedef AssetManager<Neo::Mesh>	TContainer;
	typedef Map<StaticString, IMeshLoader*>	THandlerContainer;

protected:
	TContainer			m_Assets;

	THandlerContainer	m_Loaders;

public:
	~MeshManager();

	void		Shutdown() {
		m_Assets.Shutdown();
	}

	Neo::Mesh*	Get(const ModelDef& def);

	void	ReloadAll();

protected:
	Bool	LoadMesh(const ModelDef& def, Neo::Mesh* asset) {
		assert(asset);

		IMeshLoader* loader{};
		if(!m_Loaders.Find(FilePath::GetExtension(def.Mesh), loader)) {
			return false;
		}

		if (!loader->Load(def, *asset)) {
			return false;
		}

		loader->Clear();
		return true;
	}
};