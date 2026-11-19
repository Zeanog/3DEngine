#include "MeshLoader_FBX.h"
#include "FBXNodeParser_Mesh.h"
#include "FBXSceneParser_Animation.h"
#include "FBXNodeParser_SkeletalJoint.h"

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
	auto importRes = importer->Import(m_Scene);
	importer->Destroy();
	importer = nullptr;

	if (!importRes)
	{
		return false;
	}

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

	verify(lGeomConverter.Triangulate(m_Scene, true, false));

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

#include "Images/ImageManager.h"
Bool MeshLoader_FBX::LoadTextures(FbxScene* scene) {
	assert(scene);

	// Load the textures into GPU, only for file texture now
	const int lTextureCount = scene->GetTextureCount();
	for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
	{
		FbxTexture* lTexture = scene->GetTexture(lTextureIndex);
		FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);
		if (lFileTexture && !lFileTexture->GetUserDataPtr())
		{
			// Try to load the texture from absolute path
			auto fileName = lFileTexture->GetFileName();

			GLuint lTextureObject = 0;

			auto image = Singleton<ImageManager>::GetInstance()->Get(fileName);
			if (!image) {//If path fails then try to rebuild it to our data path
				auto fullDataPath = File::RebuildFullDataPath(fileName);
				image = Singleton<ImageManager>::GetInstance()->Get(fullDataPath);
			}
			assert(image);
			//bool lStatus = //LoadTextureFromFile(lFileName, lTextureObject);

			//const FbxString lAbsFbxFileName = FbxPathUtils::Resolve(pFbxFileName);
			//const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);
			//if (!lStatus)
			//{
			//	// Load texture from relative file name (relative to FBX file)
			//	const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lFileTexture->GetRelativeFileName());
			//	lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
			//}

			//if (!lStatus)
			//{
			//	// Load texture from file name only (relative to FBX file)
			//	const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
			//	const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
			//	lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
			//}

			//if (!lStatus)
			//{
			//	FBXSDK_printf("Failed to load texture file: %s\n", lFileName.Buffer());
			//	continue;
			//}

			//if (lStatus)
			//{
			//	//GLuint* lTextureName = new GLuint(lTextureObject);
			//	//lFileTexture->SetUserDataPtr(lTextureName);
			//}
		}
	}

	return true;
}

Bool MeshLoader_FBX::ParseMesh(const TNodeTypeToNodeListMap& nodeMap, Neo::Mesh& outMesh) {
	auto& meshNodeList = nodeMap[fbxsdk::FbxNodeAttribute::eMesh];

	for (UInt32 ix = 0; ix < meshNodeList.Length(); ++ix) {
		auto node = meshNodeList[ix];
		if (!Singleton<FBXNodeParser_Mesh>::GetInstance()->Parse(node, outMesh)) {
			return false;
		}
	}

	//outMesh.VertexBuffer().UploadData();
	//outMesh.IndexBuffer().UploadData();

	return true;
}

Bool MeshLoader_FBX::ParseSkeleton(const TNodeTypeToNodeListMap& nodeMap, const StaticString& jointPrefix, Neo::Skeleton& outSkeleton) {
	auto& nodeList = nodeMap[fbxsdk::FbxNodeAttribute::eSkeleton];

	Singleton<FBXNodeParser_SkeletalJoint>::GetInstance()->Clear();

	for (UInt32 ix = 0; ix < nodeList.Length(); ++ix) {
		auto node = nodeList[ix];

		Joint joint;
		if (!Singleton<FBXNodeParser_SkeletalJoint>::GetInstance()->Parse(node, jointPrefix, joint)) {
			return false;
		}
		outSkeleton.AddJoint(joint);
	}

	return true;
}

Bool MeshLoader_FBX::ValidateSkeleton(const TNodeTypeToNodeListMap& nodeMap, const StaticString& jointPrefix, const Neo::Skeleton& skeleton) {
	auto& nodeList = nodeMap[fbxsdk::FbxNodeAttribute::eSkeleton];

	assert(nodeList.Length() == skeleton.Joints().Length());

	Singleton<FBXNodeParser_SkeletalJoint>::GetInstance()->Clear();
	for (UInt32 ix = 0; ix < nodeList.Length(); ++ix) {
		auto node = nodeList[ix];
		Joint joint;

		assert(Singleton<FBXNodeParser_SkeletalJoint>::GetInstance()->CanParse(node));

		Singleton<FBXNodeParser_SkeletalJoint>::GetInstance()->Parse(node, jointPrefix, joint);

		if (!skeleton.FindJoint(joint.Name())) {
			return false;
		}
	}

	return true;
}

Bool MeshLoader_FBX::Load(const ModelDef& def, Neo::Mesh& outMesh) {
	if (!LoadScene(def.Mesh)) {
		return false;
	}

	TNodeTypeToNodeListMap	nodeMap;
	ExtractNodes(m_Scene->GetRootNode(), nodeMap);

	//LoadTextures(m_Scene);

	ParseMesh(nodeMap, outMesh);
	ParseSkeleton(nodeMap, def.SkeletalPrefix, outMesh.Skeleton());
	/*if (def.AnimationFiles.Contains(def.Mesh)) {
		if (!ParseAnimations(nodeMap, def.SkeletalPrefix, def.AnimationFiles[def.Mesh], outMesh)) {
			Singleton<DebugConsole>::GetInstance()->Write("MeshLoader_FBX::Load: Warning: Failed to load animations from file %s. Skipping.\n", def.Mesh.CStr());
		}
	}*/

	/*FOREACH(animFileIter, def.AnimationFiles) {
		if(animFileIter->first == def.Mesh) {
			continue;
		}

		if (!LoadScene(animFileIter->first)) {
			Singleton<DebugConsole>::GetInstance()->Write("MeshLoader_FBX::Load: Warning: Failed to load animation file %s for mesh %s. Skipping.\n", animFileIter->first.CStr(), def.Mesh.CStr());
			continue;
		}

		nodeMap.Clear();
		ExtractNodes(m_Scene->GetRootNode(), nodeMap);
		if (!ValidateSkeleton(nodeMap, def.SkeletalPrefix, outMesh.Skeleton())) {
			Singleton<DebugConsole>::GetInstance()->Write("MeshLoader_FBX::Load: Warning: Failed to validate skeleton for mesh %s. Skipping.\n", animFileIter->first.CStr(), def.Mesh.CStr());
			continue;
		}
		if (!ParseAnimations(nodeMap, def.SkeletalPrefix, animFileIter->second, outMesh)) {
			Singleton<DebugConsole>::GetInstance()->Write("MeshLoader_FBX::Load: Warning: Failed to parse animations from anim %s for mesh %s. Skipping.\n", animFileIter->first.CStr(), def.Mesh.CStr());
			continue;
		}
	}*/

	return true;
}

Bool MeshLoader_FBX::ExtractNodes(fbxsdk::FbxNode* node, TNodeTypeToNodeListMap& outNodeMap) {
	if (!node) {
		return false;
	}

	auto attr = node->GetNodeAttribute();
	if (attr) {
		auto attribType = attr->GetAttributeType();
		if (!outNodeMap.Contains(attribType)) {
			outNodeMap.Add(attribType, TNodeList());
		}
		TNodeList& nodeList = outNodeMap[attribType];
		nodeList.Add(node);
	}

	for (int ix = 0; ix < node->GetChildCount(); ++ix) {
		if (!ExtractNodes(node->GetChild(ix), outNodeMap)) {
			return false;
		}
	}

	return true;
}

Bool MeshLoader_FBX::ParseAnimations(const TNodeTypeToNodeListMap& nodeMap, const StaticString& jointPrefix, const ModelDef::TInFileAnimationMap& inFileAnimMap, Neo::Mesh& outMesh) {
	assert(m_Scene);
	return Singleton<FBXSceneParser_Animation>::GetInstance()->Parse(m_Scene, nodeMap, outMesh.Skeleton(), inFileAnimMap, outMesh.AnimationClips());
}