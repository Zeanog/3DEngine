#include "MeshLoader_FBX.h"
#include "System/File.h"
#include "System/List.h"
#include "Rendering/Mesh.h"
#include "Images/ImageManager.h"
#include "Rendering/Joint.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windows.h>
#include <gl/gl.h>

fbxsdk::FbxManager* MeshLoader_FBX::g_SdkManager;

Bool GetMaterialFactor(const FbxProperty& lProperty, Neo::Mesh::AMaterial* mat, const char* channelName) {
	if (!lProperty.IsValid())
	{
		return false;
	}
	//FbxDouble3 lResult = lProperty.Get<FbxDouble3>();
	double lResult = lProperty.Get<FbxDouble>();
	if ( lResult == 1.0f) {//TODO: Test to see if this is necessary
		return false;
	}
	return mat->UpdateChannel(channelName, Color<Float32>((float)lResult, (float)lResult, (float)lResult, 1.0f));
}

Bool GetMaterialTexture(const FbxProperty& lProperty, Neo::Mesh::AMaterial* mat, const char* channelName) {
	if (!lProperty.IsValid())
	{
		return false;
	}

	const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
	assert(lTextureCount <= 1);//We only support one texture per property for now
	if (lTextureCount <= 0) {
		return false;
	}

	for (int j = 0; j < lTextureCount; j++) {
		FbxFileTexture* texture = FbxCast<FbxFileTexture>(lProperty.GetSrcObject<FbxFileTexture>(j));
		if (!texture) {
			continue;
		}
		if (texture->GetUserDataPtr())
		{
			return mat->UpdateChannel(channelName, new Neo::Image(*(static_cast<UInt32*>(texture->GetUserDataPtr())), GL_RGBA));
		}

		//TODO: Possibly find a better way to resolve texture paths
		String fileName(texture->GetFileName());//TODO: Try and remove this allocation
		if (fileName.Length() <= 0) {
			continue;
		}

		auto image = Singleton<ImageManager>::GetInstance()->Get(fileName.CStr());
		if (!image) {//If path fails then try to rebuild it to our data path
			auto fullDataPath = File::RebuildFullDataPath(fileName);
			image = Singleton<ImageManager>::GetInstance()->Get(fullDataPath);
		}
		//TODO: Figure out an alternative if we can't find the image
		return mat->UpdateChannel(channelName, image);
	}
	return false;
}

FbxAMatrix	GetLocalTransform(FbxNode* node) {
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	if (node->GetNodeAttribute())
	{
		const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}

	return matrixGeo;
}

FbxAMatrix	GetGlobalTransform(FbxNode* node) {
	FbxAMatrix globalMatrix = node->EvaluateGlobalTransform();

	return globalMatrix * GetLocalTransform(node);
}

void MeshLoader_FBX::CreateGlobals() {
	g_SdkManager = FbxManager::Create();
	if (!g_SdkManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", g_SdkManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(g_SdkManager, IOSROOT);
	g_SdkManager->SetIOSettings(ios);

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

MeshLoader_FBX::MeshLoader_FBX() {
	m_Scene = NULL;
	m_CurrentAnimLayer = NULL;

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

MeshLoader_FBX::~MeshLoader_FBX() {
	//Destroy(m_AnimationClips);
}

void MeshLoader_FBX::LoadComponents(const fbxsdk::FbxScene* pScene, FbxAnimLayer* pAnimLayer, bool pSupportVBO)
{
	LoadComponents(pScene->GetRootNode(), pAnimLayer, pSupportVBO);
}

Bool MeshLoader_FBX::LoadComponents(FbxNode* pNode, FbxAnimLayer* pAnimLayer, bool pSupportVBO)
{
	const char* type = NULL;
	FbxAMatrix mat;

	const char* name = pNode->GetName();

	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if (lNodeAttribute)
	{
		// Bake mesh as VBO(vertex buffer object) into GPU.
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			VertexBuffer vb;//TODO: Would be nice to avoid this extra copy
			IndexBuffer ib;//TODO: Would be nice to avoid this extra copy

			LoadMesh(pNode, vb, m_VertexBuffer.NumVerts(), ib, m_MaterialMap, m_Materials);

			m_VertexBuffer += vb;
			m_IndexBuffer += ib;
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
			/*FbxSkeleton* lSkeleton = (FbxSkeleton*)lNodeAttribute;

			int nbMetaData = lSkeleton->GetSrcObjectCount<FbxObjectMetaData>();
			for (int i = 0; i < nbMetaData; i++)
			{
				FbxObjectMetaData* metaData = lSkeleton->GetSrcObject<FbxObjectMetaData>(i);
				OutputDebugString((char*)metaData->GetName());
			}

			const char* lSkeletonTypes[] = { "Root", "Limb", "Limb Node", "Effector" };
			type = lSkeletonTypes[lSkeleton->GetSkeletonType()];

			mat = GetLocalTransform(pNode);*/

			/*DisplayString("    Type: ", lSkeletonTypes[lSkeleton->GetSkeletonType()]);

			if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimb)
			{
				DisplayDouble("    Limb Length: ", lSkeleton->LimbLength.Get());
			}
			else if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode)
			{
				DisplayDouble("    Limb Node Size: ", lSkeleton->Size.Get());
			}
			else if (lSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
			{
				DisplayDouble("    Limb Root Size: ", lSkeleton->Size.Get());
			}*/
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		Bool stop = LoadComponents(pNode->GetChild(lChildIndex), pAnimLayer, pSupportVBO);
		if (stop) {
			return stop;
		}
	}

	return false;
}

glm::mat4& Convert(glm::mat4& to, const FbxMatrix& from) {
	for (Int32 iy = 0; iy < 4; ++iy) {
		for (Int32 ix = 0; ix < 4; ++ix) {
			to[iy][ix] = (Float32)from.Get(iy, ix);
		}
	}

	return to;
}

glm::mat4& Convert(glm::mat4& to, const FbxAMatrix& from) {
	for (Int32 iy = 0; iy < 4; ++iy) {
		for (Int32 ix = 0; ix < 4; ++ix) {
			to[iy][ix] = (Float32)from.Get(iy, ix);
		}
	}

	return to;
}

glm::vec4& Convert(glm::vec4& to, const FbxVector4& from) {
	for (int ix = 0; ix < 4; ++ix) {
		to[ix] = (Float32)from[ix];
	}
	return to;
}

Vector<4>& Convert(Vector<4>& to, const FbxVector4& from) {
	for (int ix = 0; ix < 4; ++ix) {
		to[ix] = (Float32)from[ix];
	}
	return to;
}

Vector<3>& Convert(Vector<3>& to, const FbxVector4& from) {
	for (int ix = 0; ix < 3; ++ix) {
		to[ix] = (Float32)from[ix];
	}
	return to;
}

Vector<2>& Convert(Vector<2>& to, const FbxVector2& from) {
	for (int ix = 0; ix < 2; ++ix) {
		to[ix] = (Float32)from[ix];
	}
	return to;
}

void MeshLoader_FBX::LoadSkeletonHierarchy(FbxNode* rootnode)
{
	for (int childindex = 0; childindex < rootnode->GetChildCount(); ++childindex)
	{
		FbxNode* node = rootnode->GetChild(childindex);
		LoadSkeletonHierarchyre(node, 0, 0, -1);
	}
}

StaticString JointPrefix("HIKCharacterNode1_");//TODO: Find a better place for this

void MeshLoader_FBX::LoadSkeletonHierarchyre(FbxNode* node, int depth, int index, int parentindex)
{
	if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		JointInfo joint;
		joint.ParentIndex = parentindex;
		joint.Name = node->GetName() + JointPrefix.Length();
		m_JointNameNodeMap.Add(joint.Name, node);
		m_Joints.Add(joint);
	}
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		LoadSkeletonHierarchyre(node->GetChild(i), depth + 1, m_Joints.Length(), index);
	}
}

void MeshLoader_FBX::LoadAnimations(fbxsdk::FbxScene* pScene) {
	Int32 numStacks = pScene->GetSrcObjectCount<FbxAnimStack>();
	FOR(Int32, ix, 0, numStacks, 1) {
		FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(ix);
		if (!pAnimStack) {
			continue;
		}

		//We are ignoring that we may not be at frame 1
		FbxTimeSpan animTimeSpan = pAnimStack->GetLocalTimeSpan();
		FbxTime animDuration = animTimeSpan.GetDuration();
		FbxLongLong frameCount = animDuration.GetFrameCount(FbxTime::EMode::eFrames24);
		animDuration.SetFrame(frameCount, FbxTime::EMode::eFrames24);

		AnimationClip* animClip = new AnimationClip();
		m_AnimNames.Add(pAnimStack->GetName());
		m_AnimationClips.Add(pAnimStack->GetName(), animClip);

		animClip->Duration(animDuration.GetSecondDouble());
		animClip->FrameRate((Float32)animDuration.GetFrameRate(FbxTime::EMode::eFrames24));

		FbxAnimEvaluator* pAnimEvaluator = pScene->GetAnimationEvaluator();

		AnimKeyFrame* pKeyFrame = NULL;
		glm::mat4 mat;
		Float64 fFrameCount = (Float64)frameCount;//Using float to avoid casting int to float in the loop
		FOR(Float64, fFrameIndex, 0.0f, fFrameCount, 1.0f) {
			pKeyFrame = new	AnimKeyFrame();
			animClip->AddFrame(pKeyFrame);
			pKeyFrame->Time(fFrameIndex / (Float64)animClip->FrameRate());

			UInt32	index = 0;
			FbxTime time;
			time.SetSecondDouble(pKeyFrame->Time());
			FOREACH(iter, m_JointNameNodeMap) {
				FbxAMatrix nodeTransform = pAnimEvaluator->GetNodeLocalTransform(iter->second, time, FbxNode::eSourcePivot);
				Convert(mat, nodeTransform);
				pKeyFrame->SetLocalTransform(index++, mat);
			}
		}
	}
}

glm::mat4 MeshLoader_FBX::GetJointLocalTransform(const StaticString& jointName, Float32 time) const {
	assert(m_JointNameNodeMap.Contains(jointName));

	FbxTime t;
	t.SetSecondDouble(time);
	auto fxNode = m_JointNameNodeMap[jointName];

	glm::mat4 localMat;
	return Convert(localMat, fxNode->EvaluateLocalTransform(t, FbxNode::eSourcePivot));
}

void MeshLoader_FBX::LoadGeometry(FbxMesh* pMesh, VertexBuffer& vb, IndexBuffer& ib, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
	FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
	if (pMesh->GetElementMaterial())
	{
		lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
		int lMaterialIndiceCount = lMaterialIndice->GetCount();
		lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
	}

	FbxStringList lUVNames;
	pMesh->GetUVSetNames(lUVNames);
	const char* lUVName = NULL;
	//if (mHasUV && lUVNames.GetCount())
	//{
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
	lUVName = lUVNames[0];
	//}

	const FbxVector4* lControlPoints = pMesh->GetControlPoints();
	FbxVector4 lCurrentVertex;
	FbxVector4 lCurrentNormal;
	FbxVector2 lCurrentUV;

	Vector<3>	pos;
	Vector<3>	normal;
	Vector<2>	uv;

	FbxAMatrix globalTMatrix = GetGlobalTransform(pMesh->GetNode());
	FbxAMatrix globalRMatrix = globalTMatrix;
	globalRMatrix.SetT(FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));

	int lVertexCount = 0;
	const int lPolygonCount = pMesh->GetPolygonCount();
	vb.Resize(lPolygonCount * TRIANGLE_VERTEX_COUNT);
	for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
	{
		for (int lVerticeIndex = 0; lVerticeIndex < TRIANGLE_VERTEX_COUNT; ++lVerticeIndex)
		{
			const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

			ib.AddIndex(lPolygonIndex * TRIANGLE_VERTEX_COUNT + lVerticeIndex + appendingOffset);

			lCurrentVertex = lControlPoints[lControlPointIndex];
			lCurrentVertex = globalTMatrix.MultT(lCurrentVertex);
			Convert(pos, lCurrentVertex);
			vb.Position(lVertexCount) = pos * 0.1f;

			verify(pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal));
			lCurrentNormal = globalRMatrix.MultT(lCurrentNormal);
			Convert(normal, lCurrentNormal);
			vb.Normal(lVertexCount) = normal;

			bool lUnmappedUV;
			verify(pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV));
			Convert(uv, lCurrentUV);
			vb.TextureCoordinate(lVertexCount) = uv;

			++lVertexCount;
		}
	}

	LoadMaterials(pMesh, appendingOffset, matMap, mats);
}

void MeshLoader_FBX::LoadMaterials(FbxMesh* pMesh, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	Int32 lNbMat = pMesh->GetNode()->GetSrcObjectCount();

	FOR(Int32, lMaterialIndex, 0, lNbMat, 1) {
		auto pMaterial = (fbxsdk::FbxSurfaceMaterial*)pMesh->GetNode()->GetSrcObject(lMaterialIndex);

		if (!pMaterial)
		{
			continue;
		}
		auto name = pMaterial->GetName();
		if (!name || !name[0]) {
			continue;
		}

		Neo::Mesh::AMaterial* mat = new Neo::Mesh::Material(name);

		mats.Add(mat);
		matMap.Add(pMaterial->GetName(), mat);

		//mat->Index = 0;
		//mat->PolyCount = pMesh->GetPolygonCount();
		mat->Ranges.Add({ (Int32)appendingOffset, (UInt32)pMesh->GetPolygonCount() });

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

void MeshLoader_FBX::LoadMesh(FbxNode* pNode, VertexBuffer& vb, UInt32 appendingOffset, IndexBuffer& ib, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	FbxMesh* pMesh = pNode->GetMesh();
	assert(pNode == pMesh->GetNode());

	LoadGeometry(pMesh, vb, ib, appendingOffset, matMap, mats);
}

Bool MeshLoader_FBX::Load(const StaticString& fileName) {
	if (!g_SdkManager) {
		CreateGlobals();
	}

	int lFileFormat = -1;
	if (g_SdkManager && !g_SdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(fileName.CStr(), lFileFormat))
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		lFileFormat = g_SdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
	}

	auto importer = FbxImporter::Create(g_SdkManager, "");
	importer->Initialize(fileName.CStr(), lFileFormat);

	m_Scene = FbxScene::Create(g_SdkManager, fileName.CStr());
	if (!importer->Import(m_Scene))
	{
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

	verify( lGeomConverter.Triangulate(m_Scene, true, true) );

	// Split meshes per material, so that we only have one material per mesh (for VBO support)
	verify( lGeomConverter.SplitMeshesPerMaterial(m_Scene, /*replace*/false) );

	// Bake the scene for one frame
	LoadComponents(m_Scene, m_CurrentAnimLayer, /*SupportVBO*/true);

	LoadSkeletonHierarchy(m_Scene->GetRootNode());

	LoadAnimations(m_Scene);

	static StaticString diffuseChannel("Diffuse");

	//TODO: Not sure if this is the best way to handle missing textures
	assert(m_Materials[0] != NULL);
	assert(m_Materials[0]->ChannelMap.Contains(diffuseChannel));
	auto defaultChannel = m_Materials[0]->ChannelMap[diffuseChannel];
	for (UInt32 ix = 1; ix < m_Materials.Length(); ++ix) {
		auto material = m_Materials[ix];
		if (!material->ChannelMap.Contains(diffuseChannel)) {
			material->UpdateChannel(diffuseChannel, defaultChannel->Texture);
			material->UpdateChannel(diffuseChannel, defaultChannel->Color);
			continue;
		}

		auto channel = material->ChannelMap[diffuseChannel];
		if (channel && !channel->Texture) {
			channel->Texture = defaultChannel->Texture;
			channel->Color = defaultChannel->Color;
		}
	}

	return true;
}