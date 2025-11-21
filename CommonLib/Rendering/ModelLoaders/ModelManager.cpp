#include "ModelManager.h"
#include "ModelLoader.h"

ModelManager::ModelManager() {
	m_Loaders.Add(StaticString(".neom"), new ModelLoader());
}

ModelManager::~ModelManager() {
	Destroy( m_Loaders );
}

Model* ModelManager::Get( const StaticString& path ) {
	Model* model{};
	if(m_Models.Find(path, model)) {
		return model;
	}

	model = new Model();
	if( !LoadModel(path.CStr(), model) ) {
		DeletePtr(model);
		return NULL;
	}

	m_Models.Add( path, model );
	return model;
}

void ModelManager::ReloadAll() {
	FOREACH(iter, m_Models) {
		LoadModel(iter->first.CStr(), iter->second);
	}
}