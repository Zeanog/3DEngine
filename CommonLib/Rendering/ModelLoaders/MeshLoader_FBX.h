#pragma once

#include "Rendering/ModelLoaders/IMeshLoader.h"
#include "Rendering/ModelLoaders/AFBXNodeParser.h"

class MeshLoader_FBX : public IMeshLoader {
	INHERITED_CLASS_TYPEDEFS(MeshLoader_FBX, IMeshLoader)

public:
	typedef Map<StaticString, fbxsdk::FbxNode*>		TJointNameToNodeMap;

protected:
	static FbxManager*	g_SdkManager;
	FbxScene*			m_Scene = nullptr;

protected:
	void	VisitNode( fbxsdk::FbxNode* node, const StaticString& jointPrefix, Neo::Mesh& outMesh);

public:
	static void	CreateGlobals();
	static void	DestroyGlobals();

	MeshLoader_FBX();
	
	virtual Bool Load(const ModelDef& def, Neo::Mesh& outMesh) override;
	virtual void Clear() override;

	virtual Bool LoadScene(const StaticString& filePath) override;
	virtual Bool ParseMesh(const StaticString& jointPrefix, Neo::Mesh& outMesh) override;
	virtual Bool ParseAnimations(const StaticString& jointPrefix, const ModelDef::TInFileAnimationMap& inFileAnimMap, Neo::Mesh& outMesh) override;
};