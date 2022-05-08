#pragma once

#include "System/Typedefs.h"
#include "System/StaticString.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "System/List.h"
#include "Rendering/Mesh.h"

#include "fbxsdk.h"

#include "AMeshLoader.h"

class MeshLoader_FBX : public AMeshLoader {
public:
	const int TRIANGLE_VERTEX_COUNT = 3;

protected:
	static FbxManager*		g_SdkManager;
	FbxScene*				m_Scene;
	FbxImporter*			m_Importer;
	FbxAnimLayer *			m_CurrentAnimLayer;
	FbxArray<FbxString*>	m_AnimStackNameArray;

	List<FbxNode*>			m_JointNodes;

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