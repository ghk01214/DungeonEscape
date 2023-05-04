﻿#pragma once

struct FbxMaterialInfo
{
	FBXVec4			diffuse;
	FBXVec4			ambient;
	FBXVec4			specular;
	std::wstring			name;
	std::wstring			diffuseTexName;
	std::wstring			normalTexName;
	std::wstring			specularTexName;
};

struct BoneWeight
{
	using Pair = std::pair<int32, double>;
	std::vector<Pair> boneWeights;

	void AddWeights(uint32 index, double weight)
	{
		if (weight <= 0.f)
			return;

		auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
			[=](const Pair& p) { return p.second < weight; });

		if (findIt != boneWeights.end())
			boneWeights.insert(findIt, Pair(index, weight));
		else
			boneWeights.push_back(Pair(index, weight));

		// 가중치는 최대 4개
		if (boneWeights.size() > 4)
			boneWeights.pop_back();
	}

	void Normalize()
	{
		double sum = 0.f;
		std::for_each(boneWeights.begin(), boneWeights.end(), [&](Pair& p) { sum += p.second; });
		std::for_each(boneWeights.begin(), boneWeights.end(), [=](Pair& p) { p.second = p.second / sum; });
	}
};

struct FbxMeshInfo
{
	std::wstring						name;
	std::vector<Vertex>					vertices;
	std::vector<std::vector<uint32>>	indices;
	std::vector<FbxMaterialInfo>		materials;
	std::vector<BoneWeight>				boneWeights; // 뼈 가중치
	bool								hasAnimation;
};

struct FbxKeyFrameInfo
{
	FbxAMatrix  matTransform;
	double		time;
};

struct FbxBoneInfo
{
	std::wstring			boneName;
	int32					parentIndex;
	FbxAMatrix				matOffset;
};

struct FbxAnimClipInfo
{
	std::wstring			name;
	FbxTime			startTime;
	FbxTime			endTime;
	FbxTime::EMode	mode;
	std::vector<std::vector<FbxKeyFrameInfo>>	keyFrames;
};

class FBXLoader
{
public:
	FBXLoader();
	~FBXLoader();

public:
	void LoadFbx(const std::wstring& path);
	void LoadFbxFromBinary(const HANDLE& hFile);
	void SaveFbxToBinary(const HANDLE& hFile);

public:
	const std::wstring& GetBinaryFilePath(void) { return m_binaryFilePath; }
	void makeBinaryFilePath(const std::wstring& path);

public:
	int32 GetMeshCount() { return static_cast<int32>(m_meshes.size()); }
	const FbxMeshInfo& GetMesh(int32 idx) { return m_meshes[idx]; }
	std::vector<std::shared_ptr<FbxBoneInfo>>& GetBones() { return m_bones; }
	std::vector<std::shared_ptr<FbxAnimClipInfo>>& GetAnimClip() { return m_animClips; }
private:
	void Import(const std::wstring& path);

	void ParseNode(FbxNode* root);
	void LoadMesh(FbxMesh* mesh);
	void LoadMaterial(FbxSurfaceMaterial* surfaceMaterial);

	void		GetNormal(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	void		GetTangent(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	void		GetUV(FbxMesh* mesh, FbxMeshInfo* container, int32 idx, int32 vertexCounter);
	FBXVec4		GetMaterialData(FbxSurfaceMaterial* surface, const char* materialName, const char* factorName);
	std::wstring		GetTextureRelativeName(FbxSurfaceMaterial* surface, const char* materialProperty);

	// Animation
	void LoadBones(FbxNode* node) { LoadBones(node, 0, -1); }
	void LoadBones(FbxNode* node, int32 idx, int32 parentIdx);
	void LoadAnimationInfo();

	void LoadAnimationData(FbxMesh* mesh, FbxMeshInfo* meshInfo);
	void LoadBoneWeight(FbxCluster* cluster, int32 boneIdx, FbxMeshInfo* meshInfo);
	void LoadOffsetMatrix(FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* meshInfo);
	void LoadKeyframe(int32 animIndex, FbxNode* node, FbxCluster* cluster, const FbxAMatrix& matNodeTransform, int32 boneIdx, FbxMeshInfo* container);

	int32 FindBoneIndex(std::string name);
	FbxAMatrix GetTransform(FbxNode* node);

	void FillBoneWeight(FbxMesh* mesh, FbxMeshInfo* meshInfo);

public:
	static HANDLE CreateFileWrite(const std::wstring& path);
	static HANDLE CreateFileRead(const std::wstring& path);
	const std::vector<FBXMeshInfomation>& GetMeshInfo(void) { return m_meshInfo; }

private:
	FbxManager* m_manager = nullptr;
	FbxScene* m_scene = nullptr;
	FbxImporter* m_importer = nullptr;
	std::wstring			m_resourceDirectory;

	std::vector<FbxMeshInfo>						m_meshes;
	std::vector<std::shared_ptr<FbxBoneInfo>>		m_bones;
	std::vector<std::shared_ptr<FbxAnimClipInfo>>	m_animClips;
	FbxArray<FbxString*>				m_animNames;

	std::wstring			m_binaryFilePath;

	std::vector<FBXMeshInfomation> m_meshInfo;
};