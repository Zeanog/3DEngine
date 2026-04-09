#pragma once

#include "AFBXNodeParser.h"
#include "Rendering/Mesh.h"
#include "Rendering/VertexBuffer.h"

//This Loads materials also
class FBXNodeParser_Mesh : public AFBXNodeParser<TYPELIST_1(Neo::Mesh)> {
	INHERITED_CLASS_TYPEDEFS(FBXNodeParser_Mesh, AFBXNodeParser)
	SINGLETON_DECLARATIONS(FBXNodeParser_Mesh) {
		m_NodeType = fbxsdk::FbxNodeAttribute::eMesh;

		//m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sAmbient, { "Ambient", GetMaterialTexture });
		//m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sAmbientFactor, { "Ambient", GetMaterialFactor });

		m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sDiffuse, { "Diffuse", GetMaterialTexture });
		m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sDiffuseFactor, { "Diffuse", GetMaterialFactor });

		m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sSpecular, { "Specular", GetMaterialTexture });
		m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sSpecularFactor, { "Specular", GetMaterialFactor });

		//m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sEmissive, { "Emissive", GetMaterialTexture });
		//m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sEmissiveFactor, { "Emissive", GetMaterialFactor });

		m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sNormalMap, { "NormalMap", GetMaterialTexture });
	}

public:
	const int TRIANGLE_VERTEX_COUNT = 3;

	struct IndexRange {
		Int32	EndPolyIndex = -1;
		Int32	StartPolyIndex = 0;

		UInt32	PolyCount() const {
			return UInt32(EndPolyIndex - StartPolyIndex + 1);
		}

		void	AddPolyIndex(Int32 index) {
			if (index < StartPolyIndex) {
				StartPolyIndex = index;
			}

			if (index > EndPolyIndex) {
				EndPolyIndex = index;
			}
		}
	};

protected:
	struct MaterialPropertyParsingInfo {
		const Char* ChannelName{};
		Functor<Bool, TYPELIST_3(const fbxsdk::FbxProperty&, Neo::Mesh::AMaterial*, const Char*)>	Parser;

		Bool	Parse(const FbxProperty& property, Neo::Mesh::AMaterial* mat) const {
			return Parser(property, mat, ChannelName);
		}
	};
	Map<StaticString, MaterialPropertyParsingInfo> m_MaterialPropertyParsers;

	Map<StaticString, Neo::Mesh::AMaterial* >	m_MaterialMap;

protected:
	static Bool			GetMaterialFactor(const FbxProperty& lProperty, Neo::Mesh::AMaterial* mat, const char* channelName);
	static Bool			GetMaterialTexture(const FbxProperty& lProperty, Neo::Mesh::AMaterial* mat, const char* channelName);
	static FbxAMatrix	GetLocalTransform(FbxNode* node);
	static FbxAMatrix	GetGlobalTransform(FbxNode* node);
	static FbxAMatrix	GetGlobalTransform(FbxNode* node, const FbxTime& time);

	void LoadGeometry(FbxMesh* pMesh, Neo::VertexBuffer& vb, Neo::IndexBuffer& ib, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats);
	void LoadMaterials(FbxMesh* pMesh, UInt32 appendingOffset, const Map<Int32, IndexRange>& materialIndices, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
		assert(pMesh->GetNode()->GetMaterialCount() == materialIndices.Size());

		FOREACH(matRangeIter, materialIndices) {
			auto pMaterial = (fbxsdk::FbxSurfaceMaterial*)pMesh->GetNode()->GetSrcObject(matRangeIter->first);
			if (!pMaterial)
			{
				continue;
			}

			auto name = pMaterial->GetName();
			assert(name && name[0]);
			Neo::Mesh::AMaterial* mat{};
			if (!matMap.Contains(name)) {
				mat = new Neo::Mesh::Material(name);
				mats.Add(mat);
				matMap.Add(name, mat);
			}
			else {
				mat = matMap[name];
			}

			mat->Ranges.Add({ matRangeIter->second.StartPolyIndex + (Int32)appendingOffset, (UInt32)matRangeIter->second.PolyCount() });

#if _DEBUG
			List<StaticString>	propertyNames;//To allow us to see all the of the properties in the debugger
#endif
			for (auto&& prop = pMaterial->GetFirstProperty(); prop.IsValid(); prop = pMaterial->GetNextProperty(prop)) {
				StaticString propName(prop.GetNameAsCStr());

#if _DEBUG
				propertyNames.Add(propName);
#endif

				if (!m_MaterialPropertyParsers.Contains(propName)) {
					continue;
				}
				const auto& parser = m_MaterialPropertyParsers[propName];
				parser.Parse(prop, mat);
			}
		}
	}

public:
	virtual Bool	Parse(fbxsdk::FbxNode* node, Neo::Mesh& outMesh) override;
	virtual void	Clear() override {
		m_MaterialMap.Clear();
	}
};
