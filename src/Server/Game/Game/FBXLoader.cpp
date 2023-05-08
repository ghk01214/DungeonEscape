#include "pch.h"
#include "FBXLoader.h"

using namespace std;

FBXLoader::FBXLoader()
{

}

FBXLoader::~FBXLoader()
{
	if (m_scene)
		m_scene->Destroy();
	if (m_manager)
		m_manager->Destroy();
}

void FBXLoader::LoadFbx(const std::wstring& path)
{
	// 파일 데이터 로드
	Import(path);

	// Animation	
	LoadBones(m_scene->GetRootNode());
	// LoadAnimationInfo(); // -> 나중에 추가

	// 로드된 데이터 파싱 (Mesh/Material/Skin)
	ParseNode(m_scene->GetRootNode());
}
void FBXLoader::LoadFbxFromBinary(const HANDLE& hFile)
{
	uint32 size = 0;

	// 1. vector<FbxMeshInfo>의 사이즈 불러오기
	size = loadInt(hFile);

	m_meshInfo.resize(size);
	for (auto& mesh : m_meshInfo)
	{
		uint32 num = 0;
		mesh.m_vertices = loadVecData<FBXVec3>(hFile);
		mesh.m_indicies = loadVecData<uint32>(hFile);

		// FBXTransformInfo 위치 정보 저장
		FBXTransformInfo& transform = mesh.m_transformInfo;

		transform.translation[0] = loadDouble(hFile);
		transform.translation[1] = loadDouble(hFile);
		transform.translation[2] = loadDouble(hFile);
		transform.translation[3] = loadDouble(hFile);

		transform.scaling[0] = loadDouble(hFile);
		transform.scaling[1] = loadDouble(hFile);
		transform.scaling[2] = loadDouble(hFile);
		transform.scaling[3] = loadDouble(hFile);

		transform.rotation[0] = loadDouble(hFile);
		transform.rotation[1] = loadDouble(hFile);
		transform.rotation[2] = loadDouble(hFile);
		transform.rotation[3] = loadDouble(hFile);

		transform.qLocal[0] = loadDouble(hFile);
		transform.qLocal[1] = loadDouble(hFile);
		transform.qLocal[2] = loadDouble(hFile);
		transform.qLocal[3] = loadDouble(hFile);

		transform.qWorld[0] = loadDouble(hFile);
		transform.qWorld[1] = loadDouble(hFile);
		transform.qWorld[2] = loadDouble(hFile);
		transform.qWorld[3] = loadDouble(hFile);
	}
}

void FBXLoader::SaveFbxToBinary(const HANDLE& hFile)
{
	// 버티시즈, 인디시즈 정보 저장
	uint32 num = 0;

	// 1. vector<FbxMeshInfo>의 사이즈 저장
	num = m_meshes.size();
	saveInt(hFile, num);

	vector<FBXVec3> vertices;
	vector<uint32> indicies;
	for (auto& iter : m_meshes)
	{
		// 버텍스 정보 저장
		for (auto& vertex : iter.vertices)
		{
			vertices.push_back(vertex.pos);
		}
		saveVecData(hFile, vertices);


		// 인디시즈 정보 저장
		indicies.clear();
		for (auto& indices : iter.indices)
		{
			for (auto& index : indices)
			{
				indicies.push_back(index);
			}
		}
		saveVecData(hFile, indicies);


		// FBXTransformInfo 위치 정보 저장
		saveDouble(hFile, iter.transform.translation[0]);
		saveDouble(hFile, iter.transform.translation[1]);
		saveDouble(hFile, iter.transform.translation[2]);
		saveDouble(hFile, iter.transform.translation[3]);

		saveDouble(hFile, iter.transform.scaling[0]);
		saveDouble(hFile, iter.transform.scaling[1]);
		saveDouble(hFile, iter.transform.scaling[2]);
		saveDouble(hFile, iter.transform.scaling[3]);

		saveDouble(hFile, iter.transform.rotation[0]);
		saveDouble(hFile, iter.transform.rotation[1]);
		saveDouble(hFile, iter.transform.rotation[2]);
		saveDouble(hFile, iter.transform.rotation[3]);

		saveDouble(hFile, iter.transform.qLocal[0]);
		saveDouble(hFile, iter.transform.qLocal[1]);
		saveDouble(hFile, iter.transform.qLocal[2]);
		saveDouble(hFile, iter.transform.qLocal[3]);

		saveDouble(hFile, iter.transform.qWorld[0]);
		saveDouble(hFile, iter.transform.qWorld[1]);
		saveDouble(hFile, iter.transform.qWorld[2]);
		saveDouble(hFile, iter.transform.qWorld[3]);
	}
}

void FBXLoader::makeBinaryFilePath(const wstring& path)
{
	// fbx파일이 존재하는 폴더의 경로를 생성
	std::wstring binaryFilePath = fs::path(path).parent_path().wstring();

	// 생성된 폴더 안에 [ 파일이름.bin ] 의 binary파일 추가
	wstring fileName = fs::path(path).filename().stem().wstring() + L".bin";
	binaryFilePath += (wstring(L"\\") + fileName);

	m_binaryFilePath = binaryFilePath;

	// 나중에 Texture 경로 계산할 때 쓸 것
	m_resourceDirectory = fs::path(path).parent_path().wstring();
}

void FBXLoader::Import(const wstring& path)
{
	// FBX SDK 관리자 객체 생성
	m_manager = FbxManager::Create();

	// IOSettings 객체 생성 및 설정
	FbxIOSettings* settings = FbxIOSettings::Create(m_manager, IOSROOT);
	m_manager->SetIOSettings(settings);

	// FbxImporter 객체 생성
	m_scene = FbxScene::Create(m_manager, "");

	// 나중에 Texture 경로 계산할 때 쓸 것
	m_resourceDirectory = fs::path(path).parent_path().wstring();// 
	
	
	L"\\" + fs::path(path).filename().stem().wstring() + L".fbm";

	m_importer = FbxImporter::Create(m_manager, "");

	string strPath{ path.begin(), path.end() };
	m_importer->Initialize(strPath.c_str(), -1, m_manager->GetIOSettings());

	m_importer->Import(m_scene);

	m_scene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::DirectX);

	// 씬 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다.
	FbxGeometryConverter geometryConverter(m_manager);
	geometryConverter.Triangulate(m_scene, true);

	m_importer->Destroy();
}

void FBXLoader::ParseNode(FbxNode* node)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute)
	{
		switch (attribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			LoadMesh(node->GetMesh());

			// 노드의 위치 정보 추출
			LoadPosition(node);
			break;
		}
	}

	// Tree 구조 재귀 호출
	const int32 childCount = node->GetChildCount();
	for (int32 i = 0; i < childCount; ++i)
		ParseNode(node->GetChild(i));
}

void FBXLoader::LoadMesh(FbxMesh* mesh)
{
	m_meshes.push_back(FbxMeshInfo());
	FbxMeshInfo& meshInfo = m_meshes.back();

	meshInfo.name = s2ws(mesh->GetName());

	const int32 vertexCount = mesh->GetControlPointsCount();
	meshInfo.vertices.resize(vertexCount);
	meshInfo.boneWeights.resize(vertexCount);

	// Position
	FbxVector4* controlPoints = mesh->GetControlPoints();
	for (int32 i = 0; i < vertexCount; ++i)
	{
		meshInfo.vertices[i].pos.x = static_cast<float>(controlPoints[i].mData[0]);
		meshInfo.vertices[i].pos.y = static_cast<float>(controlPoints[i].mData[2]);
		meshInfo.vertices[i].pos.z = static_cast<float>(controlPoints[i].mData[1]);
	}

	const int32 materialCount = mesh->GetNode()->GetMaterialCount();
	meshInfo.indices.resize(materialCount);

	FbxGeometryElementMaterial* geometryElementMaterial = mesh->GetElementMaterial();

	const int32 polygonSize = mesh->GetPolygonSize(0);
	assert(polygonSize == 3);

	uint32 arrIdx[3];
	uint32 vertexCounter = 0; // 정점의 개수

	const int32 triCount = mesh->GetPolygonCount(); // 메쉬의 삼각형 개수를 가져온다
	for (int32 i = 0; i < triCount; i++) // 삼각형의 개수
	{
		for (int32 j = 0; j < 3; j++) // 삼각형은 세 개의 정점으로 구성
		{
			int32 controlPointIndex = mesh->GetPolygonVertex(i, j); // 제어점의 인덱스 추출
			arrIdx[j] = controlPointIndex;

			GetNormal(mesh, &meshInfo, controlPointIndex, vertexCounter);
			GetTangent(mesh, &meshInfo, controlPointIndex, vertexCounter);
			GetUV(mesh, &meshInfo, controlPointIndex, mesh->GetTextureUVIndex(i, j));

			vertexCounter++;
		}

		const uint32 subsetIdx = geometryElementMaterial->GetIndexArray().GetAt(i);
		meshInfo.indices[subsetIdx].push_back(arrIdx[0]);
		meshInfo.indices[subsetIdx].push_back(arrIdx[2]);
		meshInfo.indices[subsetIdx].push_back(arrIdx[1]);
	}

	// Animation
	LoadAnimationData(mesh, &meshInfo);
}

void FBXLoader::LoadPosition(FbxNode* pNode)
{
	FBXTransformInfo& transform = m_meshes.back().transform;

	transform.qLocal = pNode->EvaluateLocalTransform().GetQ();
	transform.qWorld = pNode->EvaluateGlobalTransform().GetQ();

	transform.translation = pNode->LclTranslation.Get();
	transform.rotation = pNode->LclRotation.Get();
	transform.scaling = pNode->LclScaling.Get();
}

void FBXLoader::LoadMaterial(FbxSurfaceMaterial* surfaceMaterial)
{
	FbxMaterialInfo material{};

	material.name = s2ws(surfaceMaterial->GetName());

	material.diffuse = GetMaterialData(surfaceMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	material.ambient = GetMaterialData(surfaceMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	material.specular = GetMaterialData(surfaceMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);

	material.diffuseTexName = GetTextureRelativeName(surfaceMaterial, FbxSurfaceMaterial::sDiffuse);
	material.normalTexName = GetTextureRelativeName(surfaceMaterial, FbxSurfaceMaterial::sNormalMap);
	material.specularTexName = GetTextureRelativeName(surfaceMaterial, FbxSurfaceMaterial::sSpecular);

	m_meshes.back().materials.push_back(material);
}

void FBXLoader::GetNormal(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter)
{
	if (mesh->GetElementNormalCount() == 0)
		return;

	FbxGeometryElementNormal* normal = mesh->GetElementNormal();
	uint32 normalIdx = 0;

	if (normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (normal->GetReferenceMode() == FbxGeometryElement::eDirect)
			normalIdx = vertexCounter;
		else
			normalIdx = normal->GetIndexArray().GetAt(vertexCounter);
	}
	else if (normal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (normal->GetReferenceMode() == FbxGeometryElement::eDirect)
			normalIdx = idx;
		else
			normalIdx = normal->GetIndexArray().GetAt(idx);
	}

	FbxVector4 vec = normal->GetDirectArray().GetAt(normalIdx);
	container->vertices[idx].normal.x = static_cast<float>(vec.mData[0]);
	container->vertices[idx].normal.y = static_cast<float>(vec.mData[2]);
	container->vertices[idx].normal.z = static_cast<float>(vec.mData[1]);
}

void FBXLoader::GetTangent(FbxMesh* mesh, FbxMeshInfo* meshInfo, int32 idx, int32 vertexCounter)
{
	if (mesh->GetElementTangentCount() == 0)
	{
		// TEMP : 원래는 이런 저런 알고리즘으로 Tangent 만들어줘야 함
		meshInfo->vertices[idx].tangent.x = 1.f;
		meshInfo->vertices[idx].tangent.y = 0.f;
		meshInfo->vertices[idx].tangent.z = 0.f;
		return;
	}

	FbxGeometryElementTangent* tangent = mesh->GetElementTangent();
	uint32 tangentIdx = 0;

	if (tangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (tangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			tangentIdx = vertexCounter;
		else
			tangentIdx = tangent->GetIndexArray().GetAt(vertexCounter);
	}
	else if (tangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (tangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			tangentIdx = idx;
		else
			tangentIdx = tangent->GetIndexArray().GetAt(idx);
	}

	FbxVector4 vec = tangent->GetDirectArray().GetAt(tangentIdx);
	meshInfo->vertices[idx].tangent.x = static_cast<float>(vec.mData[0]);
	meshInfo->vertices[idx].tangent.y = static_cast<float>(vec.mData[2]);
	meshInfo->vertices[idx].tangent.z = static_cast<float>(vec.mData[1]);
}

void FBXLoader::GetUV(FbxMesh* mesh, FbxMeshInfo* meshInfo, int32 idx, int32 uvIndex)
{
	FbxVector2 uv = mesh->GetElementUV()->GetDirectArray().GetAt(uvIndex);
	meshInfo->vertices[idx].uv.x = static_cast<float>(uv.mData[0]);
	meshInfo->vertices[idx].uv.y = 1.f - static_cast<float>(uv.mData[1]);
}

FBXVec4 FBXLoader::GetMaterialData(FbxSurfaceMaterial* surface, const char* materialName, const char* factorName)
{
	FbxDouble3  material;
	FbxDouble	factor = 0.f;

	FbxProperty materialProperty = surface->FindProperty(materialName);
	FbxProperty factorProperty = surface->FindProperty(factorName);

	if (materialProperty.IsValid() && factorProperty.IsValid())
	{
		material = materialProperty.Get<FbxDouble3>();
		factor = factorProperty.Get<FbxDouble>();
	}

	FBXVec4 ret = FBXVec4(
		static_cast<float>(material.mData[0] * factor),
		static_cast<float>(material.mData[1] * factor),
		static_cast<float>(material.mData[2] * factor),
		static_cast<float>(factor));

	return ret;
}

wstring FBXLoader::GetTextureRelativeName(FbxSurfaceMaterial* surface, const char* materialProperty)
{
	string name;

	FbxProperty textureProperty = surface->FindProperty(materialProperty);
	if (textureProperty.IsValid())
	{
		uint32 count = textureProperty.GetSrcObjectCount();

		if (1 <= count)
		{
			FbxFileTexture* texture = textureProperty.GetSrcObject<FbxFileTexture>(0);
			if (texture)
				name = texture->GetRelativeFileName();
		}
	}

	return s2ws(name);
}

void FBXLoader::LoadBones(FbxNode* node, int32 idx, int32 parentIdx)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		shared_ptr<FbxBoneInfo> bone = make_shared<FbxBoneInfo>();
		bone->boneName = s2ws(node->GetName());
		bone->parentIndex = parentIdx;
		m_bones.push_back(bone);
	}

	const int32 childCount = node->GetChildCount();
	for (int32 i = 0; i < childCount; i++)
		LoadBones(node->GetChild(i), static_cast<int32>(m_bones.size()), idx);
}

void FBXLoader::LoadAnimationInfo()
{
	m_scene->FillAnimStackNameArray(OUT m_animNames);

	const int32 animCount = m_animNames.GetCount();
	for (int32 i = 0; i < animCount; i++)
	{
		FbxAnimStack* animStack = m_scene->FindMember<FbxAnimStack>(m_animNames[i]->Buffer());
		if (animStack == nullptr)
			continue;

		shared_ptr<FbxAnimClipInfo> animClip = make_shared<FbxAnimClipInfo>();
		animClip->name = s2ws(animStack->GetName());
		animClip->keyFrames.resize(m_bones.size()); // 키프레임은 본의 개수만큼

		FbxTakeInfo* takeInfo = m_scene->GetTakeInfo(animStack->GetName());
		animClip->startTime = takeInfo->mLocalTimeSpan.GetStart();
		animClip->endTime = takeInfo->mLocalTimeSpan.GetStop();
		animClip->mode = m_scene->GetGlobalSettings().GetTimeMode();

		m_animClips.push_back(animClip);
	}
}

void FBXLoader::LoadAnimationData(FbxMesh* mesh, FbxMeshInfo* meshInfo)
{
	const int32 skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount <= 0 || m_animClips.empty())
		return;

	meshInfo->hasAnimation = true;

	for (int32 i = 0; i < skinCount; i++)
	{
		FbxSkin* fbxSkin = static_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));

		if (fbxSkin)
		{
			FbxSkin::EType type = fbxSkin->GetSkinningType();
			if (FbxSkin::eRigid == type || FbxSkin::eLinear)
			{
				const int32 clusterCount = fbxSkin->GetClusterCount();
				for (int32 j = 0; j < clusterCount; j++)
				{
					FbxCluster* cluster = fbxSkin->GetCluster(j);
					if (cluster->GetLink() == nullptr)
						continue;

					int32 boneIdx = FindBoneIndex(cluster->GetLink()->GetName());
					assert(boneIdx >= 0);

					FbxAMatrix matNodeTransform = GetTransform(mesh->GetNode());
					LoadBoneWeight(cluster, boneIdx, meshInfo);
					LoadOffsetMatrix(cluster, matNodeTransform, boneIdx, meshInfo);

					const int32 animCount = m_animNames.Size();
					for (int32 k = 0; k < animCount; k++)
						LoadKeyframe(k, mesh->GetNode(), cluster, matNodeTransform, boneIdx, meshInfo);
				}
			}
		}
	}

	FillBoneWeight(mesh, meshInfo);
}


void FBXLoader::FillBoneWeight(FbxMesh* mesh, FbxMeshInfo* meshInfo)
{
	const int32 size = static_cast<int32>(meshInfo->boneWeights.size());
	for (int32 v = 0; v < size; v++)
	{
		BoneWeight& boneWeight = meshInfo->boneWeights[v];
		boneWeight.Normalize();

		float animBoneIndex[4] = {};
		float animBoneWeight[4] = {};

		const int32 weightCount = static_cast<int32>(boneWeight.boneWeights.size());
		for (int32 w = 0; w < weightCount; w++)
		{
			animBoneIndex[w] = static_cast<float>(boneWeight.boneWeights[w].first);
			animBoneWeight[w] = static_cast<float>(boneWeight.boneWeights[w].second);
		}

		memcpy(&meshInfo->vertices[v].indices, animBoneIndex, sizeof(FBXVec4));
		memcpy(&meshInfo->vertices[v].weights, animBoneWeight, sizeof(FBXVec4));
	}
}

HANDLE FBXLoader::CreateFileWrite(const wstring& path)
{
	return CreateFile(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

HANDLE FBXLoader::CreateFileRead(const wstring& path)
{
	return CreateFile(path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

void FBXLoader::LoadBoneWeight(FbxCluster* cluster, int32 boneIdx, FbxMeshInfo* meshInfo)
{
	const int32 indicesCount = cluster->GetControlPointIndicesCount();
	for (int32 i = 0; i < indicesCount; i++)
	{
		double weight = cluster->GetControlPointWeights()[i];
		int32 vtxIdx = cluster->GetControlPointIndices()[i];
		meshInfo->boneWeights[vtxIdx].AddWeights(boneIdx, weight);
	}
}

void FBXLoader::LoadOffsetMatrix(FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* meshInfo)
{
	FbxAMatrix matClusterTrans;
	FbxAMatrix matClusterLinkTrans;
	// The transformation of the mesh at binding time 
	cluster->GetTransformMatrix(matClusterTrans);
	// The transformation of the cluster(joint) at binding time from joint space to world space 
	cluster->GetTransformLinkMatrix(matClusterLinkTrans);

	FbxVector4 V0 = { 1, 0, 0, 0 };
	FbxVector4 V1 = { 0, 0, 1, 0 };
	FbxVector4 V2 = { 0, 1, 0, 0 };
	FbxVector4 V3 = { 0, 0, 0, 1 };

	FbxAMatrix matReflect;
	matReflect[0] = V0;
	matReflect[1] = V1;
	matReflect[2] = V2;
	matReflect[3] = V3;

	FbxAMatrix matOffset;
	matOffset = matClusterLinkTrans.Inverse() * matClusterTrans;
	matOffset = matReflect * matOffset * matReflect;

	m_bones[boneIdx]->matOffset = matOffset.Transpose();
}

void FBXLoader::LoadKeyframe(int32 animIndex, FbxNode* node, FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* meshInfo)
{
	if (m_animClips.empty())
		return;

	FbxVector4	v1 = { 1, 0, 0, 0 };
	FbxVector4	v2 = { 0, 0, 1, 0 };
	FbxVector4	v3 = { 0, 1, 0, 0 };
	FbxVector4	v4 = { 0, 0, 0, 1 };
	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	FbxTime::EMode timeMode = m_scene->GetGlobalSettings().GetTimeMode();

	// 애니메이션 골라줌
	FbxAnimStack* animStack = m_scene->FindMember<FbxAnimStack>(m_animNames[animIndex]->Buffer());
	m_scene->SetCurrentAnimationStack(OUT animStack);

	FbxLongLong startFrame = m_animClips[animIndex]->startTime.GetFrameCount(timeMode);
	FbxLongLong endFrame = m_animClips[animIndex]->endTime.GetFrameCount(timeMode);

	for (FbxLongLong frame = startFrame; frame < endFrame; frame++)
	{
		FbxKeyFrameInfo keyFrameInfo = {};
		FbxTime fbxTime = 0;

		fbxTime.SetFrame(frame, timeMode);

		FbxAMatrix matFromNode = node->EvaluateGlobalTransform(fbxTime);
		FbxAMatrix matTransform = matFromNode.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(fbxTime);
		matTransform = matReflect * matTransform * matReflect;

		keyFrameInfo.time = fbxTime.GetSecondDouble();
		keyFrameInfo.matTransform = matTransform;

		m_animClips[animIndex]->keyFrames[boneIdx].push_back(keyFrameInfo);
	}
}

int32 FBXLoader::FindBoneIndex(string name)
{
	wstring boneName = wstring(name.begin(), name.end());

	for (UINT i = 0; i < m_bones.size(); ++i)
	{
		if (m_bones[i]->boneName == boneName)
			return i;
	}

	return -1;
}

FbxAMatrix FBXLoader::GetTransform(FbxNode* node)
{
	const FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 scaling = node->GetGeometricScaling(FbxNode::eSourcePivot);
	return FbxAMatrix(translation, rotation, scaling);
}
