#pragma once

#include "System/AssetManager.h"
#include "Rendering/Model.h"
#include "Rendering/ModelLoaders/ModelLoader_FBX.h"
#include "System/Singleton.h"

//class ModelManager {
//	SINGLETON_DECLARATIONS( ModelManager );
//
//public:
//	typedef AssetManager<Model>	TContainer;
//	typedef std::map<StaticString, AModelLoader*>	THandlerContainer;
//
//protected:
//	TContainer			m_Models;
//
//	THandlerContainer	m_Loaders;
//
//public:
//	~ModelManager();
//
//	void		Shutdown() {
//		m_Models.Shutdown();
//	}
//
//	Model*	Get( const Char* path ) {
//		return Get( StaticString(path) );
//	}
//
//	Model*	Get( const String& path ) {
//		return Get( path.CStr() );
//	}
//
//	Model*	Get( const StaticString& path );
//
//	void	ReloadAll();
//
//protected:
//	Bool	LoadModel( const StaticString& fp, Model* model ) {
//		assert(model);
//
//		THandlerContainer::iterator iter = m_Loaders.find( StaticString(FilePath::GetExtension(fp)) );
//		if( iter == m_Loaders.end() ) {
//			return false;
//		}
//
//		AMeshLoader* loader = iter->second;
//		if( !loader->Load(fp) ) {
//			return false;
//		}
//
//		if( !model->UploadData(*loader) ) {
//			return false;
//		}
//
//		loader->Clear();
//		return true;
//	}
//};