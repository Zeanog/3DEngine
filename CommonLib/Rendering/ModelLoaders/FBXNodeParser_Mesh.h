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

		//m_MaterialPropertyParsers.Add("Diffuse", { "Color", GetMaterialColor });

		//m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sDiffuse, { "Diffuse", GetSurfaceMaterial });

		//m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sSpecular, { "Specular", GetMaterialTexture });
		//m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sSpecularFactor, { "Specular", GetMaterialFactor });

		////m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sEmissive, { "Emissive", GetMaterialTexture });
		////m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sEmissiveFactor, { "Emissive", GetMaterialFactor });

		//m_MaterialPropertyParsers.Add(FbxSurfaceMaterial::sNormalMap, { "NormalMap", GetMaterialTexture });
	}

public:
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
	//struct MaterialPropertyParsingInfo {
	//	const Char* ChannelName{};
	//	Functor<Bool, TYPELIST_5(const FbxSurfaceMaterial*, const Char*, const Char*, Neo::Mesh::AMaterial*, const Char*)>	Parser;

	//	Bool	Parse(const FbxSurfaceMaterial* material, const Char* propertyName, const Char* factorPropertyName, Neo::Mesh::AMaterial* mat) const {
	//		return Parser(material, propertyName, factorPropertyName, mat, ChannelName);
	//	}

	//	static Bool    ContainsTexture(const FbxProperty& property) {
	//		const int lTextureCount = property.GetSrcObjectCount<FbxFileTexture>();
	//		assert(lTextureCount <= 1);//We only support one texture per property for now
	//		return lTextureCount > 0;
	//	}
	//};
	//Map<StaticString, MaterialPropertyParsingInfo> m_MaterialPropertyParsers;

	Map<StaticString, Neo::Mesh::AMaterial* >	m_MaterialMap;

protected:
	static Bool			GetSurfaceMaterial(const FbxSurfaceMaterial* material, const Char* propertyName, const Char* factorPropertyName, Neo::Mesh::AMaterial* mat, const StaticString& channelName);
	static FbxAMatrix	GetLocalTransform(fbxsdk::FbxNode* node);
	static FbxAMatrix	GetGlobalTransform(fbxsdk::FbxNode* node);
	static FbxAMatrix	GetGlobalTransform(fbxsdk::FbxNode* node, const FbxTime& time);

	Bool ExtractVertexNormal(FbxMesh* pMesh, fbxsdk::FbxGeometryElementNormal* element, Int32 controlPointIndex, UInt32 vertexIndex, UInt32 polyIndex, Neo::VertexBuffer::TNormal* outNormal);
	Bool ExtractVertexTexCoord(FbxMesh* pMesh, fbxsdk::FbxGeometryElementUV* element, Int32 controlPointIndex, UInt32 vertexIndex, UInt32 polyIndex, Neo::VertexBuffer::TTextureCoordinate* outTexCoord);

	void				LoadByControlPoints(fbxsdk::FbxMesh* pMesh, Bool hasNormals, Bool hasUVs, Bool useAllControlPoints, Neo::VertexBuffer& vb, Neo::IndexBuffer& ib, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats);

	static constexpr Int32 DetermineIndexFromMappingMode(fbxsdk::FbxLayerElement::EMappingMode mappingMode, const int controlPointIndex, UInt32 vertexIndex, UInt32 polyIndex) {
		switch (mappingMode)
		{
		case FbxGeometryElement::eByControlPoint:
			return controlPointIndex;

		case FbxGeometryElement::eByPolygonVertex:
			return vertexIndex;

		case FbxGeometryElement::eByPolygon:
			return polyIndex;

		case FbxGeometryElement::eAllSame:
			return 0;

		default:
			return -1;
		}
	}

	Int32 DetermineMaterialIndex(fbxsdk::FbxLayerElementMaterial* element, Int32 polyIndex) {
		fbxsdk::FbxLayerElement::EMappingMode mappingMode = element->GetMappingMode();
		fbxsdk::FbxLayerElement::EReferenceMode referenceMode = element->GetReferenceMode();
		const auto& indexArray = element->GetIndexArray();

		Int32 matIndex = -1;
		// Resolve material index based on mapping mode
		if (mappingMode == FbxLayerElement::eByPolygon) {
			if (referenceMode == FbxLayerElement::eIndexToDirect) {
				matIndex = indexArray.GetAt(polyIndex);
			}
			else {
				matIndex = polyIndex; // Direct reference
			}
		}
		else if (mappingMode == FbxLayerElement::eAllSame) {
			matIndex = indexArray.GetAt(0);
		}
		// Add other mapping modes if necessary (e.g., eByControlPoint)

		return matIndex;
	}

	static void DetermineMeshProperties(fbxsdk::FbxMesh* pMesh, Bool& outHasNormals, Bool& outHasUVs, Bool& outUseAllControlPoints);

	template<typename TElement, typename TOut>
	static Bool ExtractVertexElement(FbxMesh* pMesh, TElement* element, Int32 controlPointIndex, UInt32 vertexIndex, UInt32 polyIndex, TOut* out) {
		if (!element || !out) {
			return false;
		}

		FbxLayerElement::EMappingMode mappingMode = element->GetMappingMode();
		FbxLayerElement::EReferenceMode refMode = element->GetReferenceMode();

		auto mapIndex = DetermineIndexFromMappingMode(mappingMode, controlPointIndex, vertexIndex, polyIndex);
		if (mapIndex < 0) {
			return false;
		}

		auto mappedIndex = mapIndex;// refMode == FbxLayerElement::eDirect
		if (refMode == FbxLayerElement::eIndexToDirect) {
			mappedIndex = element->GetIndexArray().GetAt(mapIndex);
		}

		auto mappedVal = element->GetDirectArray().GetAt(mappedIndex);
		FbxConversions::Convert(*out, mappedVal);
		return true;
	}

	void LoadGeometry(FbxMesh* pMesh, Bool hasNormals, Bool hasUVs, Bool useAllControlPoints, Neo::VertexBuffer& vb, Neo::IndexBuffer& ib, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats);
	void LoadMaterials(fbxsdk::FbxNode* pNode, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats);
	Neo::Mesh::AMaterial* LoadMaterial(FbxSurfaceMaterial* material, Int32 matIndex, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats);
	void DetermineMaterialPolyCounts(FbxMesh* pMesh, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats);

public:
	virtual Bool CanParse(fbxsdk::FbxNode* node) const override {
		if (!node) {
			return false;
		}
		auto attr = node->GetNodeAttribute();
		if (!attr) {
			auto mesh = node->GetMesh();
			return false;
		}
		return attr->GetAttributeType() == NodeType();
	}

	virtual Bool	Parse(fbxsdk::FbxNode* node, Neo::Mesh& outMesh) override;
	virtual void	Clear() override {
		m_MaterialMap.Clear();
	}
};
