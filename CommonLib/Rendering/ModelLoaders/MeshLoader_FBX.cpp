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
	m_Importer = NULL;
	m_Scene = NULL;
	m_CurrentAnimLayer = NULL;

	//m_Root = NULL;
}

MeshLoader_FBX::~MeshLoader_FBX() {
	if (m_Importer) {
		m_Importer->Destroy();
		m_Importer = NULL;
	}

	//Destroy(m_AnimationClips);
}

void MeshLoader_FBX::LoadComponents(const fbxsdk::FbxScene* pScene, FbxAnimLayer* pAnimLayer, const char* pFbxFileName, bool pSupportVBO)
{
	LoadComponents(pScene->GetRootNode(), pAnimLayer, pSupportVBO);
}

Bool MeshLoader_FBX::LoadComponents(FbxNode * pNode, FbxAnimLayer * pAnimLayer, bool pSupportVBO)
{
	const char* type = NULL;
	FbxAMatrix mat;

	const char* name = pNode->GetName();

	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			VertexBuffer vb;
			IndexBuffer ib;
			List<Neo::Mesh::AMaterialSlot*>	slots;
			LoadComponents(pNode, vb, m_VertexBuffer.NumVerts(), ib, slots);

			m_VertexBuffer += vb;
			m_IndexBuffer += ib;
			m_MaterialSlots += slots;
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

template<typename TTo, typename TFrom>
void	Convert(TTo& to, const TFrom& from);

template<>
void Convert<glm::mat4, FbxMatrix>(glm::mat4& to, const FbxMatrix& from) {
	for (Int32 iy = 0; iy < 4; ++iy) {
		for (Int32 ix = 0; ix < 4; ++ix) {
			to[iy][ix] = (Float32)from.Get(iy, ix);
		}
	}
}

template<>
void Convert<glm::mat4, FbxAMatrix>(glm::mat4& to, const FbxAMatrix& from) {
	for (Int32 iy = 0; iy < 4; ++iy) {
		for (Int32 ix = 0; ix < 4; ++ix) {
			to[iy][ix] = (Float32)from.Get(iy, ix);
		}
	}
}

template<>
void	Convert<glm::vec4, FbxVector4>(glm::vec4& to, const FbxVector4& from) {
	for (int ix = 0; ix < 4; ++ix) {
		to[ix] = (Float32)from[ix];
	}
}

template<>
void	Convert<Vector<4>, FbxVector4>(Vector<4>& to, const FbxVector4& from) {
	for (int ix = 0; ix < 4; ++ix) {
		to[ix] = (Float32)from[ix];
	}
}

template<>
void	Convert<Vector<3>, FbxVector4>(Vector<3>& to, const FbxVector4& from) {
	for (int ix = 0; ix < 3; ++ix) {
		to[ix] = (Float32)from[ix];
	}
}

template<>
void	Convert<Vector<2>, FbxVector2>(Vector<2>& to, const FbxVector2& from) {
	for (int ix = 0; ix < 2; ++ix) {
		to[ix] = (Float32)from[ix];
	}
}

void MeshLoader_FBX::LoadSkeletonHierarchy(FbxNode * rootnode)
{
	for (int childindex = 0; childindex < rootnode->GetChildCount(); ++childindex)
	{
		FbxNode *node = rootnode->GetChild(childindex);
		LoadSkeletonHierarchyre(node, 0, 0, -1);
	}
}

StaticString JointPrefix("HIKCharacterNode1_");

void MeshLoader_FBX::LoadSkeletonHierarchyre(FbxNode * node, int depth, int index, int parentindex)
{
	if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		JointInfo joint;
		joint.ParentIndex = parentindex;
		joint.Name = node->GetName() + JointPrefix.Length();
		m_JointNodes.Add(node);
		m_Joints.Add(joint);
	}
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		LoadSkeletonHierarchyre(node->GetChild(i), depth + 1, m_Joints.Length(), index);
	}
}

void MeshLoader_FBX::LoadAnimations(fbxsdk::FbxScene* pScene) {
	int numStacks = pScene->GetSrcObjectCount<FbxAnimStack>();
	//prop.GetSrcObject<FbxLayeredTexture>(j)
	//pScene->GetSrcObject<FbxAnimStack>(0)
	FbxAnimStack* pAnimStack = FbxCast<FbxAnimStack>(pScene->GetSrcObject<FbxAnimStack>(1));
	if (!pAnimStack) {
		return;
	}

	//FbxAnimStack* pAnimStack = pScene->GetCurrentAnimationStack();

	FbxAnimEvaluator* pAnimEvaluator = pScene->GetAnimationEvaluator();

	//We are ignoring that we may not be at frame 1
	FbxTimeSpan animTimeSpan = pAnimStack->GetLocalTimeSpan();
	FbxTime animDuration = animTimeSpan.GetDuration();
	FbxLongLong frameCount = animDuration.GetFrameCount(FbxTime::EMode::eFrames24);
	animDuration.SetFrame(frameCount, FbxTime::EMode::eFrames24);

	AnimationClip* animClip = new AnimationClip();
	m_AnimationClips.Add(animClip);

	animClip->Duration(animDuration.GetSecondDouble());
	animClip->FrameRate((Float32)animDuration.GetFrameRate(FbxTime::EMode::eFrames24));

	AnimKeyFrame* pKeyFrame = NULL;
	glm::mat4 mat;
	for (Int32 frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
		pKeyFrame = new	AnimKeyFrame();
		animClip->AddFrame(pKeyFrame);
		pKeyFrame->Time((float)frameIndex / animClip->FrameRate());

		FbxTime time;
		time.SetSecondDouble(pKeyFrame->Time());
		for (UInt32 ix = 0; ix < m_Joints.Length(); ++ix) {
			FbxAMatrix nodeTransform = pAnimEvaluator->GetNodeLocalTransform(m_JointNodes[ix], time, FbxNode::eSourcePivot);

			Convert(mat, nodeTransform);

			pKeyFrame->SetLocalTransform(ix, mat);
		}
	}
}

glm::mat4 MeshLoader_FBX::GetJointLocalTransform(const StaticString& jointName, Float32 time) const {
	glm::mat4 localMat;
	for (UInt32 ix = 0; ix < m_Joints.Length(); ++ix) {
		if (m_Joints[ix].Name == jointName) {
			FbxTime t;
			t.SetSecondDouble(time);
			Convert(localMat, m_JointNodes[ix]->EvaluateLocalTransform(t, FbxNode::eSourcePivot));
			return localMat;
		}
	}
	return glm::identity<glm::mat4>();
}

Bool GetMaterialProperty(const FbxSurfaceMaterial * pMaterial, const char * pPropertyName, const char * pFactorPropertyName, Neo::Mesh::AMaterialSlot*& outMaterial) {
	Neo::Mesh::MaterialSlot_ColorChannel* matSlot = NULL;
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
	if (!lProperty.IsValid())
	{
		return false;
	}

	String dataDirPath = FbxGetCurrentWorkPath().Buffer();
	dataDirPath += "\\data\\";

	const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
	matSlot = new Neo::Mesh::MaterialSlot_ColorChannel();
	for (int j = 0; j < lTextureCount; j++)
	{
		FbxFileTexture* texture = FbxCast<FbxFileTexture>(lProperty.GetSrcObject<FbxFileTexture>(j));
		// Then, you can get all the properties of the texture, include its name
		const char* textureName = texture->GetName();

		String fileName(texture->GetFileName());
		if (fileName.Length() > 0) {
			Int32 index = fileName.FindIndexOf(dataDirPath);
			if (index < 0) {
				index = fileName.FindLastOf('/');
				if (index >= 0) {
					//fileName = (&fileName[index + 1]);
					fileName.Replace(0, index, dataDirPath.CStr());
				}
				else {
					index = fileName.FindLastOf('\\');
					if (index >= 0) {
						fileName.Replace(0, index, dataDirPath.CStr());
					}
				}
			}
			matSlot->Texture = Singleton<ImageManager>::GetInstance()->Get(fileName.CStr());
		}
		else if (texture && texture->GetUserDataPtr())
		{
			matSlot->Texture = new Neo::Image(*(static_cast<UInt32*>(texture->GetUserDataPtr())), GL_RGBA);
		}
	}

	outMaterial = matSlot;

	if (lFactorProperty.IsValid())
	{
		FbxDouble3 lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if (lFactor != 1)
		{
			if (!outMaterial) {
				matSlot = new Neo::Mesh::MaterialSlot_ColorChannel();
				outMaterial = matSlot;
			}

			matSlot->Diffuse[0] = (float)lFactor;
			matSlot->Diffuse[1] = (float)lFactor;
			matSlot->Diffuse[2] = (float)lFactor;
			matSlot->Diffuse[3] = 1.0f;
			return true;
		}
	}
	return outMaterial != NULL;
}

void MeshLoader_FBX::LoadTextures(FbxMesh* pMesh, List<Neo::Mesh::AMaterialSlot*>& slots) {
	FbxProperty lProperty;
	const Char* d = NULL;
	int lNbMat = pMesh->GetNode()->GetSrcObjectCount();

	for (int lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++)
	{
		FbxSurfaceMaterial *lMaterial = (FbxSurfaceMaterial *)pMesh->GetNode()->GetSrcObject(lMaterialIndex);
		Neo::Mesh::AMaterialSlot* matSlot = NULL;
		if (!lMaterial)
		{
			continue;
		}

		if (GetMaterialProperty(lMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, matSlot)) {
			matSlot->Index = 0;
			matSlot->PolyCount = pMesh->GetPolygonCount();
			slots.Add(matSlot);
		}
	}
}

void MeshLoader_FBX::LoadComponents(FbxNode* pNode, VertexBuffer& vb, UInt32 appendingOffset, IndexBuffer& ib, List<Neo::Mesh::AMaterialSlot*>& slots) {
	FbxMesh* pMesh = pNode->GetMesh();
	if (!pMesh->GetNode()) {
		return;
	}

	const int lPolygonCount = pMesh->GetPolygonCount();

	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
	FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
	if (pMesh->GetElementMaterial())
	{
		lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
		int lMaterialIndiceCount = lMaterialIndice->GetCount();
		lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
		//if (lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		//{
		//	assert(lMaterialIndiceCount == lPolygonCount);
		//	if (lMaterialIndiceCount == lPolygonCount)
		//	{
		//		// Count the faces of each material
		//		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
		//		{
		//			const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
		//			slots.EnsureSize(lMaterialIndex + 1);
		//			if (!slots[lMaterialIndex]) {
		//				slots[lMaterialIndex] = new Neo::Mesh::MaterialSlot_ColorChannel();
		//			}
		//			slots[lMaterialIndex]->PolyCount += 1;
		//		}

		//		// Record the offset (how many vertex)
		//		const int lMaterialCount = slots.Length();
		//		int lOffset = 0;
		//		for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
		//		{
		//			slots[lIndex]->Index = lOffset;
		//			lOffset += slots[lIndex]->PolyCount * TRIANGLE_VERTEX_COUNT;
		//			// This will be used as counter in the following procedures, reset to zero
		//			slots[lIndex]->PolyCount = 0;
		//		}
		//		FBX_ASSERT(lOffset == lPolygonCount * TRIANGLE_VERTEX_COUNT);
		//	}
		//}
	}

	int lPolygonVertexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;

	LoadTextures(pMesh, slots);

	/*if (!slots.Length()) {
		slots.Add(new Neo::Mesh::MaterialSlot_Texture(0, lPolygonCount));
	}*/

	FbxStringList lUVNames;
	pMesh->GetUVSetNames(lUVNames);
	const char * lUVName = NULL;
	//if (mHasUV && lUVNames.GetCount())
	//{
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
	lUVName = lUVNames[0];
	//}

	// Populate the array with vertex attribute, if by control point.
	const FbxVector4 * lControlPoints = pMesh->GetControlPoints();
	FbxVector4 lCurrentVertex;
	FbxVector4 lCurrentNormal;
	FbxVector2 lCurrentUV;

	vb.Resize(lPolygonVertexCount);
	Vector<3>	pos;
	Vector<3>	normal;
	Vector<2>	uv;

	fbxsdk::FbxLayerElement::EMappingMode lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();

	FbxAMatrix globalTMatrix = GetGlobalTransform(pNode);
	FbxAMatrix globalRMatrix = globalTMatrix;
	globalRMatrix.SetT(FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));

	int lVertexCount = 0;
	for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
	{
		// The material for current face.
		int lMaterialIndex = 0;
		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
		}

		// Where should I save the vertex attribute index, according to the material
		const int lIndexOffset = slots[lMaterialIndex]->Index + (slots[lMaterialIndex]->PolyCount - 1) * TRIANGLE_VERTEX_COUNT;

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

	for (UInt32 ix = 0; ix < slots.Length(); ++ix) {
		slots[ix]->Index += appendingOffset;
	}
}

Bool MeshLoader_FBX::Load(const StaticString& fileName) {
	if (!g_SdkManager) {
		CreateGlobals();
	}

	int lFileFormat = -1;
	m_Importer = FbxImporter::Create(g_SdkManager, "");
	if (!g_SdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(fileName.CStr(), lFileFormat))
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		lFileFormat = g_SdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
	}

	// Initialize the importer by providing a filename.
	m_Importer->Initialize(fileName.CStr(), lFileFormat);

	m_Scene = FbxScene::Create(g_SdkManager, fileName.CStr());
	if (!m_Importer->Import(m_Scene))
	{
		return false;
	}

	// Set the scene status flag to refresh 
		// the scene in the first timer callback.

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

	// Get the list of all the animation stack.
	m_Scene->FillAnimStackNameArray(m_AnimStackNameArray);

	// Get the list of all the cameras in the scene.
	// FillCameraArray(mScene, mCameraArray);

	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter lGeomConverter(g_SdkManager);

	lGeomConverter.Triangulate(m_Scene, true, true);//node //<<------------------------------------------timing more..

	// Split meshes per material, so that we only have one material per mesh (for VBO support)
	lGeomConverter.SplitMeshesPerMaterial(m_Scene, /*replace*/true);

	// Bake the scene for one frame
	LoadComponents(m_Scene, m_CurrentAnimLayer, fileName.CStr(), /*SupportVBO*/true);

	LoadSkeletonHierarchy(m_Scene->GetRootNode());

	// Initialize the frame period.
	//mFrameTime.SetTime(0, 0, 0, 1, 0, m_Scene->GetGlobalSettings().GetTimeMode());

	LoadAnimations(m_Scene);

	assert(m_MaterialSlots[0] != NULL);
	for (UInt32 ix = 1; ix < m_MaterialSlots.Length(); ++ix) {
		if (!m_MaterialSlots[ix]->Texture) {
			m_MaterialSlots[ix]->Texture = m_MaterialSlots[0]->Texture;
		}
	}

	//lResult = true;

	return true;
}

Bool MeshLoader_Simple::Load(const StaticString& path) {
	File file;

	if (!file.Open(path, "rb")) {
		return false;
	}

	UInt32 fileLength = file.Length();
	const Char* encodedJson = STACK_ALLOC(Char, fileLength);
	file.Read(encodedJson, fileLength);

	json_value* root = json_parse(encodedJson, fileLength);
	Bool validRead = Load(root);
	json_value_free(root);
	root = NULL;
	return validRead;
}

Bool MeshLoader_Simple::Load(json_value* root) {
	if (root->type != json_type::json_object) {
		return false;
	}

	json_value* obj = root;

	if (root->type == json_type::json_string) {
		File file;

		file.Open(root->u.string.ptr, "rb");

		UInt32 dataLength = file.Length();
		Char* encodedJson = STACK_ALLOC(Char, dataLength + 1);
		file.Read(encodedJson, dataLength);
		file.Close();

		obj = json_parse(encodedJson, dataLength);
	}

	if (obj->type != json_type::json_object) {
		if (obj != root) {
			json_value_free(obj);
		}
		return false;
	}

	IndexBuffer* ib = NULL;
	for (UInt32 ix = 0; ix < obj->u.object.length; ++ix) {
		if (!String::StrICmp(obj->u.object.values[ix].name, "vertexBuffer")) {
			if (!m_VertexBuffer.ReadFrom(obj->u.object.values[ix].value)) {
				if (obj != root) {
					json_value_free(obj);
				}
				return false;
			}
			continue;
		}

		if (!String::StrICmp(obj->u.object.values[ix].name, "indexBuffer")) {
			if (!m_IndexBuffer.ReadFrom(obj->u.object.values[ix].value)) {
				if (obj != root) {
					json_value_free(obj);
				}
				return false;
			}
			continue;
		}

		//if (!String::StrICmp(obj->u.object.values[ix].name, "parts")) {
		//	if (!JsonSerializer<List<Neo::Mesh::MaterialSlot_Texture*>>::ReadFrom(obj->u.object.values[ix].value, m_MaterialSlots)) {
		//		if (obj != root) {
		//			json_value_free(obj);
		//		}
		//		return false;
		//	}

		//	//Validate Material Data
		//	for (int im = 0; im < m_MaterialSlots.Length(); ++im) {
		//		assert(m_MaterialSlots[im].Index >= 0 && m_MaterialSlots[im].EndIndex() < m_IndexBuffer.Count());
		//	}
		//	continue;
		//}
	}

	if (obj != root) {
		json_value_free(obj);
	}
	return true;
}

glm::mat4 MeshLoader_Simple::GetJointLocalTransform(const StaticString& jointName, Float32 time) const {
	return glm::identity<glm::mat4>();
}