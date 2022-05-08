#include "ModelLoader.h"
#include "System/json.h"
#include "System/File.h"
#include "Rendering/ModelLoaders/MeshManager.h"

/*
{
	"mesh":"data/Cicada.fbx",
	"invertNormals": false,
	"shaderProgram": {
		"vert": "data/deferredShading.vert",
		"frag": "data/deferredShading.frag"
	}
}
*/
void	ModelLoader::ParseMesh(json_value* node) {
	assert(node->type == json_type::json_string);
	m_MeshFilePath = node->u.string.ptr;
}

void ModelLoader::ParseInvertedNormals(json_value* node) {
	assert(node->type == json_type::json_boolean);
	m_InvertNormals = node->u.boolean;
}

void	ModelLoader::ParseShaderProgram(json_value* node) {
	assert(node->type == json_type::json_object);
	for (unsigned int ix = 0; ix < node->u.object.length; ++ix) {
		if (String::StrICmp(node->u.object.values[ix].name, "vert") == 0 ) {
			m_VertProgamFilePath = node->u.object.values[ix].value->u.string.ptr;
		}

		if (String::StrICmp(node->u.object.values[ix].name, "frag") == 0) {
			m_FragProgamFilePath = node->u.object.values[ix].value->u.string.ptr;
		}
	}
}

void	ModelLoader::ParseShadowProgram(json_value* node) {
	assert(node->type == json_type::json_object);
	for (unsigned int ix = 0; ix < node->u.object.length; ++ix) {
		if (String::StrICmp(node->u.object.values[ix].name, "vert") == 0) {
			m_VertShadowProgamFilePath = node->u.object.values[ix].value->u.string.ptr;
		}

		if (String::StrICmp(node->u.object.values[ix].name, "frag") == 0) {
			m_FragShadowProgamFilePath = node->u.object.values[ix].value->u.string.ptr;
		}
	}
}

ModelLoader::ModelLoader() {
	TFieldParser pm;
	pm.AddListener(this, &ModelLoader::ParseMesh);
	m_FieldParsers.Add("mesh", pm);

	TFieldParser pin;
	pin.AddListener(this, &TSelf::ParseInvertedNormals);
	m_FieldParsers.Add("invertNormals", pin);

	TFieldParser psp;
	psp.AddListener(this, &TSelf::ParseShaderProgram);
	m_FieldParsers.Add("shaderProgram", psp);

	TFieldParser ssp;
	ssp.AddListener(this, &TSelf::ParseShadowProgram);
	m_FieldParsers.Add("shadowProgram", ssp);
}

Bool	ModelLoader::Load(const StaticString& fileName) {
	File file;
	if (!file.Open(fileName, "rb")) {
		return false;
	}

	char* buffer = new char[file.Length()];
	if (!file.Read(buffer, file.Length())) {
		return false;
	}

	json_value* root = json_parse(buffer, file.Length());
	assert(root->type == json_type::json_object);
	for (unsigned int ix = 0; ix < root->u.object.length; ++ix) {
		m_FieldParsers[root->u.object.values[ix].name]( root->u.object.values[ix].value );
	}

	DeleteArray(buffer);

	if (!Singleton<MeshManager>::GetInstance()->IsLoaded(m_MeshFilePath)) {
		Singleton<MeshManager>::GetInstance()->Load(m_MeshFilePath, m_InvertNormals);//This shouldnt be here. Need a mesh meta file
	}

	return true;
}

void	ModelLoader::Clear() {
}

Neo::Mesh* ModelLoader::Mesh() const {
	return Singleton<MeshManager>::GetInstance()->Get(m_MeshFilePath);
}

bool ModelLoader::GetShaderPrograms(StaticString& vertProgPath, StaticString& fragProgPath) const {
	vertProgPath = m_VertProgamFilePath;
	fragProgPath = m_FragProgamFilePath;
	return vertProgPath.Length() > 0 && fragProgPath.Length();
}

bool ModelLoader::GetShadowPrograms(StaticString& vertProgPath, StaticString& fragProgPath) const {
	vertProgPath = m_VertShadowProgamFilePath;
	fragProgPath = m_FragShadowProgamFilePath;
	return vertProgPath.Length() > 0 && fragProgPath.Length();
}