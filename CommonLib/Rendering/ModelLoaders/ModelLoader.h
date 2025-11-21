#pragma once

#include "System/StaticString.h"
#include "System/Map.h"
#include "System/Functors/Functor.h"
#include "Rendering/Mesh.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include "System/Reflector.h"

class IModelLoader {
	CLASS_TYPEDEFS(IModelLoader)

public:
	virtual Bool	Load(const Char* fileName) = 0;
	virtual	void	Clear() = 0;
};

class ModelLoader : public IModelLoader {
	INHERITEDCLASS_TYPEDEFS(ModelLoader, IModelLoader)

protected:
	DEFINE_MEMBER_EX(Bool, InvertNormals)

	StaticString		m_MeshFilePath;

	ShaderProgram_GLSL* m_ShaderProgram{};
	List<StaticString>	m_RequiredChannels;//These are the textures to be provided to the shader

	ShaderProgram_GLSL* m_ShadowProgram{};

	typedef Functor<void, TYPELIST_1(const rapidjson::Value&)> TFieldParser;
	static Map<StaticString, TFieldParser>	m_FieldParsers;

protected:
	void				ParseMesh(const rapidjson::Value& value);
	void				ParseInvertedNormals(const rapidjson::Value& value);
	void				ParseProgram(const rapidjson::Value& value, ShaderProgram_GLSL& inoutProgram);
	void				ParseShaderProgram(const rapidjson::Value& value);
	void				ParseShadowProgram(const rapidjson::Value& value);

	template<typename TData>
	void				Parse(const rapidjson::Value& value, TData& outData) {
		Singleton<ValueParser<TData>>::GetInstance()->Get(value, outData);
	}

	void			BuildFieldParsers();

public:
	ModelLoader();

	Neo::Mesh*		Mesh() const;
	const ShaderProgram_GLSL* GetShaderProgram() {
		return m_ShaderProgram;
	}
	const ShaderProgram_GLSL* GetShadowPrograms() {
		return m_ShadowProgram;
	}
	List<StaticString>&& GetRequiredChannels() {
		return std::move(m_RequiredChannels);
	}

	virtual Bool	Load(const Char* fileName);
	virtual	void	Clear();
};