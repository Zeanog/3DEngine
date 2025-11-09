#pragma once

#include "System/AssetManager.h"
#include "Rendering/Model.h"
#include "Rendering/ModelLoaders/ModelLoader.h"
#include "System/Singleton.h"

class ModelManager {
	CLASS_TYPEDEFS(ModelManager)
	SINGLETON_DECLARATIONS( ModelManager );

public:
	typedef AssetManager<Model>	TContainer;
	typedef Map<StaticString, ModelLoader*>	THandlerContainer;

protected:
	TContainer			m_Models;

	THandlerContainer	m_Loaders;

public:
	~ModelManager();

	void		Shutdown() {
		m_Models.Shutdown();
	}

	Model*	Get( const Char* path ) {
		return Get( StaticString(path) );
	}

	Model*	Get( const String& path ) {
		return Get( path.CStr() );
	}

	Model*	Get( const StaticString& path );

	void	ReloadAll();

protected:
	Bool	LoadModel( const Char* fp, Model* model ) {
		assert(model);

		THandlerContainer::TValue	loader{};
		if (!m_Loaders.Find(FilePath::GetExtension(fp), loader)) {
			return false;
		}
		if( !loader->Load(fp) ) {
			return false;
		}

		if( !model->UploadData(*loader) ) {
			return false;
		}

		loader->Clear();
		return true;
	}
};