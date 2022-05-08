#pragma once

#include "System/StaticString.h"
#include "System/Map.h"
#include "System/Functors/Functor.h"
#include "Rendering/Mesh.h"

class AModelLoader {
	CLASS_TYPEDEFS(AModelLoader);

public:
	virtual Bool	Load(const StaticString& fileName) = 0;
	virtual	void	Clear() = 0;
};

class ModelLoader : public AModelLoader {
	INHERITEDCLASS_TYPEDEFS(ModelLoader, AModelLoader);

protected:
	typedef Functor<void, TYPELIST_1(json_value*)> TFieldParser;

	Map < StaticString, TFieldParser>	m_FieldParsers;

	StaticString		m_MeshFilePath;
	Bool				m_InvertNormals;
	StaticString		m_FragProgamFilePath;
	StaticString		m_VertProgamFilePath;

	StaticString		m_FragShadowProgamFilePath;
	StaticString		m_VertShadowProgamFilePath;

protected:
	void				ParseMesh(json_value* node);
	void				ParseInvertedNormals(json_value* node);
	void				ParseShaderProgram(json_value* node);
	void				ParseShadowProgram(json_value* node);

public:
	ModelLoader();

	Neo::Mesh* Mesh() const;
	bool			GetShaderPrograms(StaticString& vertProgPath, StaticString& fragProgPath) const;
	bool			GetShadowPrograms(StaticString& vertProgPath, StaticString& fragProgPath) const;

	virtual Bool	Load(const StaticString& fileName);
	virtual	void	Clear();
};