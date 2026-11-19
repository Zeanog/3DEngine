#include "FBXNodeParser_Mesh.h"
#include "Images/ImageManager.h"
#include "System\DataStructureLibrary.h"

using namespace fbxsdk;

Bool FBXNodeParser_Mesh::GetSurfaceMaterial(const fbxsdk::FbxSurfaceMaterial* material, const Char* propertyName, const Char* factorPropertyName, Neo::Mesh::AMaterial* mat, const StaticString& channelName) {
	const fbxsdk::FbxProperty lProperty = material->FindProperty(propertyName);
	const fbxsdk::FbxProperty lFactorProperty = material->FindProperty(factorPropertyName);
	if (lProperty.IsValid() && lFactorProperty.IsValid())
	{
		auto lResult = lProperty.Get<fbxsdk::FbxDouble3>();
		double lFactor = lFactorProperty.Get<fbxsdk::FbxDouble>();
		if (lFactor != 1.0)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}

		//TODO: We are assuming that we are using 'mult' in our blending.
		//if (lResult[0] != 1.0 || lResult[1] != 1.0 || lResult[2] != 1.0) {//Don't add a non-useful value
			mat->UpdateChannel(channelName, Color<Float32>{ (Float32)lResult[0], (Float32)lResult[1], (Float32)lResult[2]});
		//}
	}

	if (lProperty.IsValid())
	{
		auto propName = lProperty.GetNameAsCStr();
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		if (lTextureCount > 0)
		{
			const FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>();
			auto filePath = lTexture->GetFileName();
			auto image = Singleton<ImageManager>::GetInstance()->Get(filePath);
			if (!image) {//If path fails then try to rebuild it to our data path
				auto fullDataPath = File::RebuildFullDataPath(filePath);
				image = Singleton<ImageManager>::GetInstance()->Get(fullDataPath);
			}

			if (image) {
				mat->UpdateChannel(channelName, image);
			}
		}
	}

	return true;
}

FbxAMatrix	FBXNodeParser_Mesh::GetLocalTransform(fbxsdk::FbxNode* node) {
	const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

FbxAMatrix	FBXNodeParser_Mesh::GetGlobalTransform(fbxsdk::FbxNode* node) {
	FbxAMatrix globalMatrix = node->EvaluateGlobalTransform();
	return globalMatrix * GetLocalTransform(node);
}

FbxAMatrix	FBXNodeParser_Mesh::GetGlobalTransform(fbxsdk::FbxNode* node, const FbxTime& time) {
	FbxAMatrix globalMatrix = node->EvaluateGlobalTransform(time);

	return globalMatrix * GetLocalTransform(node);
}

Bool FBXNodeParser_Mesh::Parse(fbxsdk::FbxNode* node, Neo::Mesh& outMesh) {
	if (!CanParse(node)) {
		return false;
	}

	auto mesh = node->GetMesh();
	if(!mesh) {
		return false;
	}

	Neo::VertexBuffer vb;//TODO: Would be nice to avoid this extra copy
	Neo::IndexBuffer ib;//TODO: Would be nice to avoid this extra copy

	Bool hasNormals = false;
	Bool hasUVs = false;
	Bool useAllControlPoints = false;
	DetermineMeshProperties(mesh, hasNormals, hasUVs, useAllControlPoints);
	if (useAllControlPoints) {
		LoadByControlPoints(mesh, hasNormals, hasUVs, useAllControlPoints, vb, ib, outMesh.VertexBuffer().NumVerts(), m_MaterialMap, outMesh.Materials());
	}
	else {
		//Not Correct!!!
		LoadGeometry(mesh, hasNormals, hasUVs, useAllControlPoints, vb, ib, outMesh.VertexBuffer().NumVerts(), m_MaterialMap, outMesh.Materials());
	}

	outMesh.VertexBuffer() += vb;
	outMesh.IndexBuffer() += ib;

	return true;
}

Bool FBXNodeParser_Mesh::ExtractVertexNormal(fbxsdk::FbxMesh* pMesh, fbxsdk::FbxGeometryElementNormal* element, Int32 controlPointIndex, UInt32 vertexIndex, UInt32 polyIndex, Neo::VertexBuffer::TNormal* outNormal) {
	return ExtractVertexElement(pMesh, element, controlPointIndex, vertexIndex, polyIndex, outNormal);
}

Bool FBXNodeParser_Mesh::ExtractVertexTexCoord(fbxsdk::FbxMesh* pMesh, fbxsdk::FbxGeometryElementUV* element, Int32 controlPointIndex, UInt32 vertexIndex, UInt32 polyIndex, Neo::VertexBuffer::TTextureCoordinate* outTexCoord) {
	return ExtractVertexElement(pMesh, element, controlPointIndex, vertexIndex, polyIndex, outTexCoord);
}

void FBXNodeParser_Mesh::DetermineMeshProperties(fbxsdk::FbxMesh* pMesh, Bool& outHasNormals, Bool& outHasUVs, Bool& outUseAllControlPoints ) {
	outHasNormals = pMesh->GetElementNormalCount() > 0;
	outHasUVs = pMesh->GetElementUVCount() > 0;
	outUseAllControlPoints = true;

	FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
	FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
	if (outHasNormals)
	{
		lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
		if (lNormalMappingMode == FbxGeometryElement::eNone)
		{
			outHasNormals = false;
		}
		if (outHasNormals && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
		{
			outUseAllControlPoints = false;
		}
	}
	if (outHasUVs)
	{
		lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
		if (lUVMappingMode == FbxGeometryElement::eNone)
		{
			outHasUVs = false;
		}
		if (outHasUVs && lUVMappingMode != FbxGeometryElement::eByControlPoint)
		{
			outUseAllControlPoints = false;
		}
	}
}

void FBXNodeParser_Mesh::LoadByControlPoints(fbxsdk::FbxMesh* pMesh, Bool hasNormals, Bool hasUVs, Bool useAllControlPoints, Neo::VertexBuffer& vb, Neo::IndexBuffer& ib, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	const int lPolygonCount = pMesh->GetPolygonCount();
	int lVertexCount = pMesh->GetControlPointsCount();
	/*if (!useAllControlPoints)
	{
		lVertexCount = lPolygonCount * Neo::Mesh::TRIANGLE_VERTEX_COUNT;
	}*/
	
	//float* lVertices = new float[lPolygonVertexCount * VERTEX_STRIDE];
	//unsigned int* lIndices = new unsigned int[lPolygonCount * Neo::Mesh::TRIANGLE_VERTEX_COUNT];

	auto indices = pMesh->GetPolygonVertices();
	for (int ix = 0; ix < pMesh->GetPolygonVertexCount(); ++ix) {
		ib.Add(indices[ix] + appendingOffset);
	}
	
	if (hasNormals)
	{
		vb.ResizeNormals(lVertexCount);
	}
	float* lUVs = NULL;
	FbxStringList lUVNames;
	pMesh->GetUVSetNames(lUVNames);
	const char* lUVName = NULL;
	//if (hasUVs && lUVNames.GetCount())
	//{
	//	//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
	//	lUVName = lUVNames[0];
	//	//vb.AddTextureCoordinate("", VertexArrayObject<Neo::VertexBuffer::TTextureCoordinate>());
	//	vb.TextureCoordinate("").Resize(lVertexCount);
	//}

	

	FbxVector4 lCurrentVertex;
	FbxVector4 lCurrentNormal;
	FbxVector2 lCurrentUV;

	const FbxGeometryElementNormal* lNormalElement = NULL;
	const FbxGeometryElementUV* lUVElement = NULL;
	if (hasNormals)
	{
		lNormalElement = pMesh->GetElementNormal();
	}
	if (hasUVs)
	{
		lUVElement = pMesh->GetElementUV();
	}

	auto& texCoordVAO = vb.TextureCoordinate("");
	const FbxVector4* lControlPoints = pMesh->GetControlPoints();
	for (int lIndex = 0; lIndex < lVertexCount; ++lIndex)
	{
		// Save the vertex position.
		lCurrentVertex = lControlPoints[lIndex];
		glm::vec3 pos;
		FbxConversions::Convert(pos, lCurrentVertex);
		vb.Position(lIndex) = pos;

		if (hasNormals)
		{
			int lNormalIndex = lIndex;
			if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
			{
				lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
			}
			lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
			glm::vec3 normal;
			FbxConversions::Convert(normal, lCurrentNormal);
			vb.Normal(lIndex) = normal;
		}

		if (hasUVs)
		{
			int lUVIndex = lIndex;
			if (lUVElement->GetReferenceMode() == fbxsdk::FbxLayerElement::eIndexToDirect)
			{
				lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
			}
			lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
			FbxConversions::Convert(texCoordVAO[lIndex], lCurrentUV);
		}
	}
}

template< class TIndexArray >
Int32 DetermineIndex(TIndexArray& indexArray, fbxsdk::FbxLayerElement::EReferenceMode refMode, Int32 index ) {
	auto val = index;// refMode == FbxLayerElement::eDirect
	if (refMode == fbxsdk::FbxLayerElement::eIndexToDirect) {
		val = indexArray.GetAt(val);
	}

	return val;
}

void FBXNodeParser_Mesh::LoadGeometry(fbxsdk::FbxMesh* pMesh, Bool hasNormals, Bool hasUVs, Bool useAllControlPoints, Neo::VertexBuffer& vb, Neo::IndexBuffer& ib, UInt32 appendingOffset, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	//DetermineMaterialPolyCounts(pMesh, matMap, mats);
	
	FbxAMatrix globalTMatrix = GetGlobalTransform(pMesh->GetNode());
	FbxAMatrix globalRMatrix = globalTMatrix;
	globalRMatrix.SetT(FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));

	FbxStringList lUVNames;
	pMesh->GetUVSetNames(lUVNames);
	const char* lUVName = lUVNames[0];

	/*for (int ix = 0; ix < lUVNames.GetCount(); ++ix) {
		lUVName = lUVNames[ix];
		vb.AddTextureCoordinate(lUVName);
	}*/

	UInt32 lVertexCount = 0;
	auto numCtrlPoints = pMesh->GetControlPointsCount();
	const int lPolygonCount = pMesh->GetPolygonCount();
	vb.Resize(numCtrlPoints, lUVNames);

	auto normalElement = pMesh->GetElementNormal(0);
	assert(normalElement);

	auto uvElement = pMesh->GetElementUV(0);
	assert(uvElement);

	//const auto vertices = pMesh->GetControlPoints();
	const auto indices = pMesh->GetPolygonVertices();

	LoadMaterials(pMesh->GetNode(), matMap, mats);

	Int32 matIndex = -1;

	auto matElement = pMesh->GetElementMaterial();
	assert(matElement);
	auto& materialIndice = matElement->GetIndexArray();
	auto matMappingMode = matElement->GetMappingMode();
	auto matReferenceMode = matElement->GetReferenceMode();
	auto matIndiceCount = materialIndice.GetCount();

	//TODO: What happens if multiple meshes reference the same material????????

	if (matMappingMode == fbxsdk::FbxLayerElement::eByPolygon) {
		assert(0);
		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
			matIndex = DetermineIndex(materialIndice, matReferenceMode, lPolygonIndex);
		}
	}
	else if( matMappingMode == fbxsdk::FbxLayerElement::eByPolygonVertex ) {
		assert(0);
		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
			auto startIndex = pMesh->GetPolygonVertexIndex(lPolygonIndex);

			for (int polyVerticeIndex = 0; polyVerticeIndex < Neo::Mesh::TRIANGLE_VERTEX_COUNT; ++polyVerticeIndex) {
				matIndex = DetermineIndex(materialIndice, matReferenceMode, startIndex + polyVerticeIndex);
			}
		}
	}
	else if (matMappingMode == fbxsdk::FbxLayerElement::eByControlPoint) {
		assert(0);
		for (int ctrlIndex = 0; ctrlIndex < numCtrlPoints; ++ctrlIndex) {
			matIndex = DetermineIndex(materialIndice, matReferenceMode, ctrlIndex);
		}
	}
	else if (matMappingMode == fbxsdk::FbxLayerElement::eAllSame) {
		matIndex = mats.Length() - 1;//TODO: UNTESTED!!!!!
	}
	else {
		assert(0);
	}
	
	for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
	{	
		auto startIndex = pMesh->GetPolygonVertexIndex(lPolygonIndex);
		//TODO: test to make sure polys only have 3 edges

		for (int polyVerticeIndex = 0; polyVerticeIndex < Neo::Mesh::TRIANGLE_VERTEX_COUNT; ++polyVerticeIndex)
		{
			const int ibIndex = startIndex + polyVerticeIndex;
			const int vbIndex = pMesh->GetPolygonVertex(lPolygonIndex, polyVerticeIndex);
#if _DEBUG
			auto index = indices[ibIndex];
			assert(index == vbIndex);
			//assert(ib.Count() == ibIndex);
			auto offsetIndex = index + appendingOffset;
			ib.Add(offsetIndex);
			assert(ib[ibIndex] == offsetIndex);//Verify we are at the correct index
#else
			ib.Add(vbIndex + appendingOffset);
#endif
			auto&& lCurrentVertex = std::move(pMesh->GetControlPointAt(vbIndex));
			//lCurrentVertex = globalTMatrix.MultT(lCurrentVertex);
			glm::vec3	pos;
			FbxConversions::Convert(pos, lCurrentVertex);
			vb.Position(vbIndex) = pos;

			glm::vec3	normal;
			fbxsdk::FbxVector4	currentNormal;

			//ExtractVertexNormal(pMesh, normalElement, vbIndex, ibIndex, lPolygonIndex, &normal);

			verify(pMesh->GetPolygonVertexNormal(lPolygonIndex, polyVerticeIndex, currentNormal));
			FbxConversions::Convert(normal, currentNormal);
			vb.Normal(vbIndex) = normal;

			glm::vec2	uv;
			Bool unmappedUV = true;
			fbxsdk::FbxVector2	currentUV;
			assert(lUVNames.GetCount() == 1);
			for (int ix = 0; ix < lUVNames.GetCount(); ++ix) {//TODO: Not sure if this is correct
				lUVName = lUVNames[ix];
				auto& texCoords = vb.TextureCoordinate(lUVName);

				//ExtractVertexTexCoord(pMesh, uvElement, vbIndex, ibIndex, lPolygonIndex, &uv);

				verify(pMesh->GetPolygonVertexUV(lPolygonIndex, polyVerticeIndex, lUVName, currentUV, unmappedUV) && !unmappedUV);
				//assert(uv.x == currentUV[0] && uv.y == currentUV[1]);
				FbxConversions::Convert(uv, currentUV);
				texCoords[vbIndex] = uv;
			}

			++lVertexCount;
		}

		/*if (range.IndexCount < ib.Count()) {
			range.IndexCount = ib.Count();
		}*/
	}

	auto mat = mats[matIndex];
	auto& range = mat->Range;
	assert(range.StartIndex > 0 && range.IndexCount == 0);
	
	UInt32 startIndex = 0;
	if (matIndex > 0) {
		decltype(mat->Range)* prevRange = &mats[matIndex - 1]->Range;
		assert(prevRange->IndexCount > 0);
		startIndex = prevRange->StartIndex + prevRange->IndexCount;
	}

	range.StartIndex = startIndex;
	range.IndexCount = ib.Count();
}

void FBXNodeParser_Mesh::LoadMaterials(fbxsdk::FbxNode* pNode, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	Int32 startMatIndex = mats.Length();//Allows us to put all materials in one list
	const int lMaterialCount = pNode->GetMaterialCount();
	for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
	{
		FbxSurfaceMaterial* lMaterial = pNode->GetMaterial(lMaterialIndex);
		LoadMaterial(lMaterial, startMatIndex + lMaterialIndex, matMap, mats);
	}
}

Neo::Mesh::AMaterial* FBXNodeParser_Mesh::LoadMaterial(FbxSurfaceMaterial* material, Int32 matIndex, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	if (!material)
	{
		assert(0);
		return nullptr;
	}

	auto name = material->GetName();
	if (!name || !name[0]) {
		assert(0);
		return nullptr;
	}

	Neo::Mesh::AMaterial* mat{};
	if (!matMap.Contains(name)) {
		mat = new Neo::Mesh::Material(name);
		mats.EnsureSize(matIndex + 1);//TODO: Could possibly just append????  Could remove the index argument
		mats[matIndex] = mat;
		matMap.Add(name, mat);

		GetSurfaceMaterial(material, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, mat, "Diffuse");
		//GetSurfaceMaterial(material, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, mat, "Emissive");
		//GetSurfaceMaterial(material, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, mat, "SpecularMap");
		//GetSurfaceMaterial(material, FbxSurfaceMaterial::sNormalMap, "", mat, "NormalMap");
	}
	else {
		mat = matMap[name];
	}

	return mat;
}

void FBXNodeParser_Mesh::DetermineMaterialPolyCounts(fbxsdk::FbxMesh* pMesh, Map<StaticString, Neo::Mesh::AMaterial*>& matMap, List<Neo::Mesh::AMaterial*>& mats) {
	auto& matPolyCounts = *Library<List<UInt32>>::GetInstance()->CheckOut();

	__try {
		matPolyCounts.Clear();

		if (!pMesh->GetElementMaterial())
		{
			__leave;
		}

		const int lPolygonCount = pMesh->GetPolygonCount();
		auto lMaterialElement = pMesh->GetElementMaterial();
		auto lMaterialIndice = &lMaterialElement->GetIndexArray();
		FbxGeometryElement::EMappingMode lMaterialMappingMode = lMaterialElement->GetMappingMode();
		if (!lMaterialIndice || lMaterialMappingMode != FbxGeometryElement::eByPolygon)
		{
			matPolyCounts.EnsureSize(1, lPolygonCount);
		}
		else {

			assert(lMaterialIndice->GetCount() == lPolygonCount);
			/*if (lMaterialIndice->GetCount() != lPolygonCount)
			{
				return;
			}*/

			// Count the faces of each material
			for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
			{
				const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
				matPolyCounts.EnsureSize(lMaterialIndex + 1, 0);
				matPolyCounts[lMaterialIndex] += 1;
			}
		}

#if _DEBUG
		UInt32 currentOffset = 0;
		for (UInt32 ix = 0; ix < matPolyCounts.Length(); ++ix) {
			assert(matPolyCounts[ix] > 0);

			//LoadMaterial(pMesh->GetNode()->GetMaterial(ix), ix, currentOffset, matMap, mats);
			currentOffset += matPolyCounts[ix];
		}
#endif
	}
	__finally {
		Library<List<UInt32>>::GetInstance()->Return(&matPolyCounts);
	}
}