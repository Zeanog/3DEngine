#include "ModelLoader.h"
#include "rapidjson\document.h"
#include "System/File.h"
#include "Rendering/ModelLoaders/MeshManager.h"
#include "System/Reflector.h"
#include "System/JsonValueParsers.h"
#include "Shaders/ShaderProgramManager.h"

void	ModelLoader::ParseDef(const rapidjson::Value& value) {
	Singleton<ValueParser<ModelDef>>::GetInstance()->Get(value, m_Def);
}

void ModelLoader::ParseInvertedNormals(const rapidjson::Value& value) {
	Parse(value, m_InvertNormals);
}

void ModelLoader::ParseProgram(const rapidjson::Value& value, ShaderProgram_GLSL& inoutProgram) {
	assert(value.IsObject());

	static const StaticString s_Vert("vert");
	static const StaticString s_Frag("frag");
	StaticString vertProgFilePath;
	StaticString fragProgFilePath;

	FOREACH_MEMBER(memberIter, value) {
		StaticString memberName(memberIter->name.GetString());

		//TODO: Change this to be a 'Map' so we can avoid the if/else chain
		if (memberName == s_Vert) {
			Parse(memberIter->value, vertProgFilePath);
		}
		else if (memberName == s_Frag) {
			Parse(memberIter->value, fragProgFilePath);
		}
	}

	verify(inoutProgram.Create(vertProgFilePath, fragProgFilePath, NULL));
}

void	ModelLoader::ParseShaderProgram(const rapidjson::Value& value) {
	if (!m_ShaderProgram) {
		m_ShaderProgram = new ShaderProgram_GLSL();
	}
	ParseProgram(value, *m_ShaderProgram);
}

void	ModelLoader::ParseShadowProgram(const rapidjson::Value& value) {
	if (!m_ShadowProgram) {
		m_ShadowProgram = new ShaderProgram_GLSL();
	}
	ParseProgram(value, *m_ShadowProgram);
}

Map<StaticString, ModelLoader::TFieldParser> ModelLoader::m_FieldParsers;

void ModelLoader::BuildFieldParsers() {
	if (m_FieldParsers.Size() > 0) {
		return;
	}

	TFieldParser pd;
	pd.AddListener(this, &TSelf::ParseDef);
	m_FieldParsers.Add("def", pd);

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

ModelLoader::ModelLoader() {
	BuildFieldParsers();
}

Bool	ModelLoader::Load(const Char* fileName) {
	rapidjson::Document	doc;
	verify(rapidjson::LoadFrom(fileName, doc));
	assert(doc.IsObject());

	FOREACH_MEMBER(memberIter, doc) {
		StaticString memberName(memberIter->name.GetString());
		if (!m_FieldParsers.Contains(memberName)) {
			continue;
		}
		auto& parser = m_FieldParsers[memberName];
		parser(memberIter->value);
	}

	return true;
}

void	ModelLoader::Clear() {
	//m_MeshFilePath = "";
	m_InvertNormals = false;
	m_ShaderProgram = nullptr;

	m_ShadowProgram = nullptr;
}

Neo::Mesh* ModelLoader::Mesh() const {
	Neo::Mesh* mesh = Singleton<MeshManager>::GetInstance()->Get(m_Def);
	return mesh;
}