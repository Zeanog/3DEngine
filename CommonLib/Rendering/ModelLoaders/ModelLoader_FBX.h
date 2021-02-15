#pragma once

#include "System/Typedefs.h"
#include "System/StaticString.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "System/List.h"
#include "Rendering/Mesh.h"
#include "fbxsdk.h"

class AMeshLoader {
public:
	struct JointInfo {
		StaticString	Name;
		Int32			ParentIndex;
	};

protected:
	VertexBuffer	m_VertexBuffer;
	IndexBuffer		m_IndexBuffer;
	List<Neo::Mesh::AMaterialSlot*>	m_MaterialSlots;

	List<JointInfo>			m_Joints;
	List<AnimationClip*>	m_AnimationClips;

public:
	virtual Bool	Load(const StaticString& fileName) = 0;
	virtual	void	Clear() = 0;

	const VertexBuffer&	VB() const {
		return m_VertexBuffer;
	}

	const IndexBuffer&	IB() const {
		return m_IndexBuffer;
	}

	const List<AnimationClip*>&	AnimationClips() const {
		return m_AnimationClips;
	}

	const List<Neo::Mesh::AMaterialSlot*>&	MaterialSlots() const {
		return m_MaterialSlots;
	}

	const List<JointInfo>&	Joints() const {
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

class MeshLoader_FBX : public AMeshLoader {
public:
	const int TRIANGLE_VERTEX_COUNT = 3;

protected:
	static FbxManager* g_SdkManager;
	FbxScene* m_Scene;
	FbxImporter* m_Importer;
	FbxAnimLayer * m_CurrentAnimLayer;
	FbxArray<FbxString*> m_AnimStackNameArray;

	List<FbxNode*>		m_JointNodes;

protected:
	void	LoadComponents(const fbxsdk::FbxScene * pScene, FbxAnimLayer * pAnimLayer, const char * pFbxFileName, bool pSupportVBO);
	Bool	LoadComponents(FbxNode * pNode, FbxAnimLayer * pAnimLayer, bool pSupportVBO);
	void	LoadComponents(FbxNode * pNode, VertexBuffer& vb, UInt32 appendingOffset, IndexBuffer& ib, List<Neo::Mesh::AMaterialSlot*>& slots);
	void	LoadAnimations(fbxsdk::FbxScene* pScene);
	void	LoadSkeletonHierarchy(FbxNode * rootnode);
	void	LoadSkeletonHierarchyre(FbxNode * node, int depth, int index, int parentindex);
	void	LoadTextures(FbxMesh* pMesh, List<Neo::Mesh::AMaterialSlot*>& slots);

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


class MeshLoader_Simple : public AMeshLoader {
public:

protected:
	Bool	Load(json_value* root);
	//Bool	LoadComponents(json_value* rootObj);

public:
	virtual Bool	Load(const StaticString& fileName) override;
	virtual	void	Clear() override {
	}

	virtual glm::mat4	GetJointLocalTransform(const StaticString& jointName, Float32 time) const;
};