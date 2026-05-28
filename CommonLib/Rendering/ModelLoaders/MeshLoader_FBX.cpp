#include "MeshLoader_FBX.h"
#include "FBXNodeParser_Mesh.h"
#include "FBXHierarchyParser_Skeleton.h"
#include "FBXSceneParser_Animation.h"

MeshLoader_FBX::MeshLoader_FBX() {
	//AFBXNodeParser* parser = Singleton<FBXNodeParser_Mesh>::GetInstance();
	//m_NodeParsers.Add(parser->NodeType(), parser);

	//m_SceneParsers.Add(Singleton<FBXHierarchyParser_Skeleton>::GetInstance());//This must be before animation parser
	//m_SceneParsers.Add(Singleton<FBXSceneParser_Animation>::GetInstance());
}

void MeshLoader_FBX::Clear() {
	/*FOREACH(nodeParser, m_NodeParsers) {
		nodeParser->second->Clear();
	}

	for (UInt32 ix = 0; ix < m_SceneParsers.Length(); ++ix) {
		m_SceneParsers[ix]->Clear();
	}*/
}

#include "System/DebugConsole.h"
Bool MeshLoader_FBX::LoadScene(const StaticString& filePath) {
	if (!g_SdkManager) {
		CreateGlobals();
	}

	if(m_Scene) {
		m_Scene->Destroy();
		m_Scene = nullptr;
	}

	int lFileFormat = -1;
	if (g_SdkManager && !g_SdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(filePath.CStr(), lFileFormat))
	{
		Singleton<DebugConsole>::GetInstance()->Write("MeshLoader_FBX::LoadScene: Warning: Unrecognized file format for file %s. Defaulting to FBX binary format.\n", filePath);
		lFileFormat = g_SdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
	}

	auto importer = FbxImporter::Create(g_SdkManager, "");
	assert(importer);
	verify(importer->Initialize(filePath.CStr(), lFileFormat));

	m_Scene = FbxScene::Create(g_SdkManager, filePath.CStr());
	if (!importer->Import(m_Scene))
	{
		importer->Destroy();
		importer = nullptr;
		return false;
	}
	importer->Destroy();
	importer = nullptr;

	// Convert Axis System to what is used in this example, if needed
	FbxAxisSystem SceneAxisSystem = m_Scene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
	if (SceneAxisSystem != OurAxisSystem)
	{
		OurAxisSystem.ConvertScene(m_Scene);
	}

	// Convert Unit System to what is used in this example, if needed
	FbxSystemUnit SceneSystemUnit = m_Scene->GetGlobalSettings().GetSystemUnit();
	if (SceneSystemUnit.GetScaleFactor() != 1.0)
	{
		//The unit in this example is centimeter.
		FbxSystemUnit::cm.ConvertScene(m_Scene);
	}

	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter lGeomConverter(g_SdkManager);

	verify(lGeomConverter.Triangulate(m_Scene, true, true));

	// Split meshes per material, so that we only have one material per mesh (for VBO support)
	verify(lGeomConverter.SplitMeshesPerMaterial(m_Scene, /*replace*/true));
	return true;
}

fbxsdk::FbxManager* MeshLoader_FBX::g_SdkManager = nullptr;
void MeshLoader_FBX::CreateGlobals() {
	g_SdkManager = FbxManager::Create();
	if (!g_SdkManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", g_SdkManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ioSettings = FbxIOSettings::Create(g_SdkManager, IOSROOT);
	g_SdkManager->SetIOSettings(ioSettings);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	g_SdkManager->LoadPluginsDirectory(lPath.Buffer());
}

void MeshLoader_FBX::DestroyGlobals() {
	if (g_SdkManager) {
		g_SdkManager->Destroy();
		g_SdkManager = NULL;
	}
}

void MeshLoader_FBX::VisitNode(fbxsdk::FbxNode* node, const StaticString& jointPrefix, TJointNameToNodeMap& jointNameToNodeMap, Neo::Mesh& outMesh) {
	if (Singleton<FBXNodeParser_Mesh>::GetInstance()->CanParse(node)) {
		Singleton<FBXNodeParser_Mesh>::GetInstance()->Parse(node, outMesh);
	} 
	else if (Singleton<FBXNodeParser_Joint>::GetInstance()->CanParse(node)) {
		Joint joint;
		Singleton<FBXNodeParser_Joint>::GetInstance()->Parse(node, jointPrefix, joint);
		outMesh.Skeleton().Joints().Add(joint);
		jointNameToNodeMap.Add(joint.Name(), node);
	}

	for( int ix = 0; ix < node->GetChildCount(); ++ix ) {
		VisitNode(node->GetChild(ix), jointPrefix, jointNameToNodeMap, outMesh);
	}
}

Bool MeshLoader_FBX::Load(const ModelDef& def, Neo::Mesh& outMesh) {
	if (!LoadScene(def.Mesh)) {
		return false;
	}

	ParseMesh(def.SkeletonPrefix, outMesh);
	if(def.AnimationFiles.Contains(def.Mesh)) {
		if (!ParseAnimations(def.SkeletonPrefix, def.AnimationFiles[def.Mesh], outMesh)) {
			Singleton<DebugConsole>::GetInstance()->Write("MeshLoader_FBX::Load: Warning: Failed to load animations from file %s. Skipping.\n", def.Mesh.CStr());
		}
	}

	FOREACH(animFileIter, def.AnimationFiles) {
		if(animFileIter->first == def.Mesh) {
			continue;
		}

		if (!LoadScene(animFileIter->first)) {
			Singleton<DebugConsole>::GetInstance()->Write("MeshLoader_FBX::Load: Warning: Failed to load animation file %s for mesh %s. Skipping.\n", animFileIter->first.CStr(), def.Mesh.CStr());
			continue;
		}

		//TODO: Pull the nodes out for joints
		if (!Singleton<FBXHierarchyParser_Skeleton>::GetInstance()->Validate(m_Scene, def.SkeletonPrefix, outMesh.Skeleton())) {
			continue;
		}
		if (!ParseAnimations(def.SkeletonPrefix, animFileIter->second, outMesh)) {
			Singleton<DebugConsole>::GetInstance()->Write("MeshLoader_FBX::Load: Warning: Failed to parse animations from anim %s for mesh %s. Skipping.\n", animFileIter->first.CStr(), def.Mesh.CStr());
			continue;
		}
	}
	return true;
}

Bool MeshLoader_FBX::ParseMesh(const StaticString& jointPrefix, Neo::Mesh& outMesh) {
	assert(m_Scene);
	VisitNode(m_Scene->GetRootNode(), jointPrefix, m_JointNameToNodeMap, outMesh);
	return true;
}

Bool MeshLoader_FBX::ParseAnimations(const StaticString& jointPrefix, const ModelDef::TInFileAnimationMap& inFileAnimMap, Neo::Mesh& outMesh) {
	assert(m_Scene);
	return Singleton<FBXSceneParser_Animation>::GetInstance()->Parse(m_Scene, m_JointNameToNodeMap, inFileAnimMap, outMesh.AnimationClips());
}