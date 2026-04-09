#include "FBXNodeParser_Mesh.h"

Bool FBXNodeParser_Mesh::GetMaterialFactor(const FbxProperty& lProperty, Neo::Mesh::AMaterial* mat, const char* channelName) {
	if (!lProperty.IsValid())
	{
		return false;
	}
	//FbxDouble3 lResult = lProperty.Get<FbxDouble3>();
	double lResult = lProperty.Get<FbxDouble>();
	if (lResult == 1.0f) {//TODO: Test to see if this is necessary
		return false;
	}
	return mat->UpdateChannel(channelName, Color<Float32>((float)lResult, (float)lResult, (float)lResult, 1.0f));
}

#include "Images/ImageManager.h"
Bool FBXNodeParser_Mesh::GetMaterialTexture(const FbxProperty& lProperty, Neo::Mesh::AMaterial* mat, const char* channelName) {
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

FbxAMatrix	FBXNodeParser_Mesh::GetLocalTransform(FbxNode* node) {
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

FbxAMatrix	FBXNodeParser_Mesh::GetGlobalTransform(FbxNode* node) {
	FbxAMatrix globalMatrix = node->EvaluateGlobalTransform();

	return globalMatrix * GetLocalTransform(node);
}

FbxAMatrix	FBXNodeParser_Mesh::GetGlobalTransform(FbxNode* node, const FbxTime& time) {
	FbxAMatrix globalMatrix = node->EvaluateGlobalTransform(time);

	return globalMatrix * GetLocalTransform(node);
}

Bool FBXNodeParser_Mesh::Parse(fbxsdk::FbxNode* node, Neo::Mesh& outMesh) {
	if (!CanParse(node)) {
		return false;
	}

	FbxMesh* mesh = node->GetMesh();
	if(!mesh) {
		return false;
	}

	Neo::VertexBuffer vb;//TODO: Would be nice to avoid this extra copy
	Neo::IndexBuffer ib;//TODO: Would be nice to avoid this extra copy

	LoadGeometry(mesh, vb, ib, outMesh.VertexBuffer().NumVerts(), m_MaterialMap, outMesh.Materials());

	outMesh.VertexBuffer() += vb;
	outMesh.IndexBuffer() += ib;

	return true;
}

void FBXNodeParser_Mesh::LoadGeometry(FbxMesh* pMesh, Neo::VertexBuffer& vb, Neo::IndexBuffer& ib, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
	FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
	if (pMesh->GetElementMaterial())
	{
		lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
		int lMaterialIndiceCount = lMaterialIndice->GetCount();
		lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
	}

	const FbxVector4* lControlPoints = pMesh->GetControlPoints();
	FbxVector4 lCurrentVertex;
	FbxVector4 lCurrentNormal;
	FbxVector2 lCurrentUV;

	glm::vec3	pos;
	glm::vec3	normal;
	glm::vec2	uv;

	FbxAMatrix globalTMatrix = GetGlobalTransform(pMesh->GetNode());
	FbxAMatrix globalRMatrix = globalTMatrix;
	globalRMatrix.SetT(FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));

	static Map<Int32, IndexRange>	materialInfos;//To minimize reallocations.  This makes this function non-thread-safe.
	materialInfos.Clear();

	int lVertexCount = 0;
	const int lPolygonCount = pMesh->GetPolygonCount();
	vb.Resize(lPolygonCount * TRIANGLE_VERTEX_COUNT);
	FbxStringList lUVNames;
	pMesh->GetUVSetNames(lUVNames);
	const char* lUVName = NULL;

	auto& texCoordsMap = vb.TextureCoordinates();
	for (int ix = 0; ix < lUVNames.GetCount(); ++ix) {
		lUVName = lUVNames[ix];
		assert(!texCoordsMap.Contains(lUVName));
		texCoordsMap.Add(lUVName, VertexArrayObject<Neo::VertexBuffer::TTextureCoordinate>());
		texCoordsMap[lUVName].Resize(vb.NumVerts());
	}

	for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
	{
		// The material for current face.
		int lMaterialIndex = 0;
		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
		}

		if (!materialInfos.Contains(lMaterialIndex)) {
			materialInfos.Add(lMaterialIndex, IndexRange());
		}
		auto& info = materialInfos[lMaterialIndex];
		info.AddPolyIndex(lPolygonIndex);

		for (int lVerticeIndex = 0; lVerticeIndex < TRIANGLE_VERTEX_COUNT; ++lVerticeIndex)
		{
			const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

			ib.AddIndex(lPolygonIndex * TRIANGLE_VERTEX_COUNT + lVerticeIndex + appendingOffset);

			lCurrentVertex = lControlPoints[lControlPointIndex];
			lCurrentVertex = globalTMatrix.MultT(lCurrentVertex);
			FbxConversions::Convert(pos, lCurrentVertex);
			vb.Position(lVertexCount) = pos;

			verify(pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal));
			lCurrentNormal = globalRMatrix.MultT(lCurrentNormal);
			FbxConversions::Convert(normal, lCurrentNormal);
			vb.Normal(lVertexCount) = normal;

			for (int ix = 0; ix < lUVNames.GetCount(); ++ix) {
				lUVName = lUVNames[ix];
				auto& texCoords = texCoordsMap[lUVName];

				bool lUnmappedUV;
				verify(pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV));
				FbxConversions::Convert(uv, lCurrentUV);
				texCoords[lVertexCount] = uv;
			}

			++lVertexCount;
		}
	}

	LoadMaterials(pMesh, appendingOffset, materialInfos, matMap, mats);
}