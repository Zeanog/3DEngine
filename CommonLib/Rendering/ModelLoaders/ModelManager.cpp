#include "ModelManager.h"
#include "ModelLoader_FBX.h"

//ModelManager::ModelManager() {
//	m_Loaders[ StaticString(".fbx") ] = new MeshLoader_FBX();
//	m_Loaders[ StaticString(".model") ] = new MeshLoader_Simple();
//}
//
//ModelManager::~ModelManager() {
//	Destroy( m_Loaders );
//}
//
//Model* ModelManager::Get( const StaticString& path ) {
//	//STACK_STRING(fullPath, 256 + relativePath.Length());
//	//
//	//	Int32 fullPathLength = GetCurrentDirectory(fullPath.Length(), fullPath.CStr());
//	//	String::StrCpy(fullPath.CStr() + fullPathLength, fullPath.Length(), "/");
//	//	++fullPathLength;
//	//	String::StrCpy(fullPath.CStr() + fullPathLength, fullPath.Length(), relativePath.CStr());
//
//	Model* model = m_Models.Find( path );
//	if( model ) {
//		return model;
//	}
//
//	model = new Model();
//	if( !LoadModel(path, model) ) {
//		DeletePtr(model);
//		return NULL;
//	}
//
//	m_Models.Add( path, model );
//	return model;
//}
//
//void ModelManager::ReloadAll() {
//	FOREACH(iter, m_Models) {
//		LoadModel(iter->first, iter->second);
//	}
//}