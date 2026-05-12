#pragma once

#include "System/StaticString.h"
#include "System/Map.h"
#include "System/Functors/Functor.h"
#include "Rendering/Mesh.h"
#include "Shaders/ShaderProgram_GLSL.h"
#include "Rendering/ModelLoaders/ModelDef.h"

class IModelLoader {
	CLASS_TYPEDEFS(IModelLoader)

public:
	virtual Bool	Load(const Char* fileName) = 0;
	virtual	void	Clear() = 0;
};

class ModelLoader : public IModelLoader {
	INHERITED_CLASS_TYPEDEFS(ModelLoader, IModelLoader)

protected:
	ModelDef			m_Def;

	DEFINE_MEMBER_EX(Bool, InvertNormals)

	ShaderProgram_GLSL* m_ShaderProgram{};

	ShaderProgram_GLSL* m_ShadowProgram{};

	typedef Functor<void, TYPELIST_1(const rapidjson::Value&)> TFieldParser;
	static Map<StaticString, TFieldParser>	m_FieldParsers;

protected:
	void			ParseDef(const rapidjson::Value& value);
	void			ParseInvertedNormals(const rapidjson::Value& value);
	void			ParseProgram(const rapidjson::Value& value, ShaderProgram_GLSL& inoutProgram);
	void			ParseShaderProgram(const rapidjson::Value& value);
	void			ParseShadowProgram(const rapidjson::Value& value);

	template<typename TData>
	void			Parse(const rapidjson::Value& value, TData& outData) {
		Singleton<ValueParser<TData>>::GetInstance()->Get(value, outData);
	}

	void			BuildFieldParsers();

public:
	ModelLoader();

	Neo::Mesh*		Mesh() const;
	const ShaderProgram_GLSL* GetShaderProgram() const {
		return m_ShaderProgram;
	}
	const ShaderProgram_GLSL* GetShadowPrograms() const {
		return m_ShadowProgram;
	}

	virtual Bool	Load(const Char* fileName);
	virtual	void	Clear();
};