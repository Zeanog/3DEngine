#include "ModelLoader.h"
#include "rapidjson\document.h"
#include "System/File.h"
#include "Rendering/ModelLoaders/MeshManager.h"
#include "System/Reflector.h"
#include "System/JsonLoader.h"
#include "Shaders/ShaderProgramManager.h"

void	ModelLoader::ParseMesh(const rapidjson::Value& value) {
	Parse(value, m_MeshFilePath);
}

void ModelLoader::ParseInvertedNormals(const rapidjson::Value& value) {
	Parse(value, m_InvertNormals);
}

void ModelLoader::ParseProgram(const rapidjson::Value& value, ShaderProgram_GLSL& inoutProgram) {
	assert(value.IsObject());

	static StaticString s_Vert("vert");
	static StaticString s_Frag("frag");
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
	m_ShaderProgram->EnumerateUniforms(m_RequiredChannels, 1U, GL_SAMPLER_2D);
	//May need to manually add some channels here later
}

void	ModelLoader::ParseShadowProgram(const rapidjson::Value& value) {
	if (!m_ShadowProgram) {
		m_ShadowProgram = new ShaderProgram_GLSL();
	}
	ParseProgram(value, *m_ShadowProgram);

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
	m_MeshFilePath = "";
	m_InvertNormals = false;
	m_ShaderProgram = nullptr;
	m_RequiredChannels.Clear();

	m_ShadowProgram = nullptr;
}

Neo::Mesh* ModelLoader::Mesh() const {
	return Singleton<MeshManager>::GetInstance()->Get(m_MeshFilePath);
}