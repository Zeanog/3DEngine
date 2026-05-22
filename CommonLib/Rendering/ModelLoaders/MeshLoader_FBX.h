#pragma once

#include "System/Typedefs.h"
#include "System/StaticString.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "System/List.h"
#include "System/Map.h"
#include "Rendering/Mesh.h"
#include "System/Functors/Functor.h"

class AMeshLoader {
public:
	struct JointInfo {
		StaticString	Name;
		Int32			ParentIndex;
	};

protected:
	VertexBuffer	m_VertexBuffer;
	IndexBuffer		m_IndexBuffer;
	List<Neo::Mesh::AMaterial*>	m_Materials;
	Map<StaticString, Neo::Mesh::AMaterial* >	m_MaterialMap;

	List<JointInfo>			m_Joints;

	List<StaticString>		m_AnimNames;
	Map<StaticString, AnimationClip*>	m_AnimationClips;

public:
	virtual Bool	Load(const StaticString& fileName) = 0;
	virtual	void	Clear() = 0;

	const VertexBuffer& VB() const {
		return m_VertexBuffer;
	}

	const IndexBuffer& IB() const {
		return m_IndexBuffer;
	}

	const List<StaticString>& AnimNames() const {
		return m_AnimNames;
	}

	const Map<StaticString, AnimationClip*>& AnimationClips() const {
		return m_AnimationClips;
	}

	const List<Neo::Mesh::AMaterial*>& Materials() const {
		return m_Materials;
	}

	const List<JointInfo>& Joints() const {
		return m_Joints;
	}

	virtual glm::mat4	GetJointLocalTransform(const StaticString& jointName, Float32 time) const = 0;

	/*virtual void	CopyComponentsTo(List<GeometricComponent*>& comps) const {
		comps.Clear();
		for (int ix = 0; ix < m_Components.Length(); ++ix) {
			comps.Add(m_Components[ix]);
		}
	}*/
};

#include "System/Functors/Functor.h"
#include <fbxsdk.h>

class MeshLoader_FBX : public AMeshLoader {
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
	static FbxManager* g_SdkManager;
	FbxScene* m_Scene;
	FbxAnimLayer* m_CurrentAnimLayer;

	Map<StaticString, FbxNode*>	m_JointNameNodeMap;

	struct MaterialPropertyParsingInfo {
		const Char* ChannelName{};
		Functor<Bool, TYPELIST_3(const fbxsdk::FbxProperty&, Neo::Mesh::AMaterial*, const Char*)>	Parser;

		Bool	Parse(const FbxProperty& property, Neo::Mesh::AMaterial* mat) const {
			return Parser(property, mat, ChannelName);
		}
	};
	Map<StaticString, MaterialPropertyParsingInfo> m_MaterialPropertyParsers;

protected:
	void	LoadComponents(const fbxsdk::FbxScene* pScene, FbxAnimLayer* pAnimLayer, bool pSupportVBO);
	Bool	LoadComponents(FbxNode* pNode, FbxAnimLayer* pAnimLayer, bool pSupportVBO);
	void	LoadMesh(FbxNode* pNode, VertexBuffer& vb, UInt32 appendingOffset, IndexBuffer& ib, Map<StaticString, Neo::Mesh::AMaterial* >& matMap, List<Neo::Mesh::AMaterial*>& mats);
	void	LoadAnimations(fbxsdk::FbxScene* pScene);
	void	LoadSkeletonHierarchy(FbxNode* rootnode);
	void	LoadSkeletonHierarchyre(FbxNode* node, int depth, int index, int parentindex);
	void	LoadGeometry(FbxMesh* pMesh, VertexBuffer& vb, IndexBuffer& ib, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats);
	void	LoadMaterials(FbxMesh* pMesh, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats);

public:
	static void	CreateGlobals();
	static void	DestroyGlobals();

	MeshLoader_FBX();
	virtual ~MeshLoader_FBX();
	virtual Bool	Load(const StaticString& fileName) override;
	virtual	void	Clear() override {
	}

	virtual glm::mat4	GetJointLocalTransform(const StaticString& jointName, Float32 time) const;
};