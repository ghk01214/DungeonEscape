#pragma once
#include "Object.h"

class Material;
class StructuredBuffer;

struct IndexBufferInfo
{
	ComPtr<ID3D12Resource>		buffer;
	D3D12_INDEX_BUFFER_VIEW		bufferView;
	DXGI_FORMAT					format;
	uint32						count;
};

struct KeyFrameInfo
{
	double	time;
	int32	frame;
	Vec3	scale;
	Vec4	rotation;
	Vec3	translate;
};

struct BoneInfo
{
	wstring					boneName;
	int32					parentIdx;
	Matrix					matOffset;
};

struct AnimClipInfo
{
	wstring			animName;
	int32			frameCount;
	double			duration;
	vector<vector<KeyFrameInfo>>	keyFrames;
};

class Mesh : public Object
{
public:
	Mesh();
	virtual ~Mesh();

	void Create(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexbuffer);
	void Render(uint32 instanceCount = 1, uint32 idx = 0);
	void Render(shared_ptr<class InstancingBuffer>& buffer, uint32 idx = 0);

	static shared_ptr<Mesh> CreateFromFBX(const struct FbxMeshInfo* meshInfo, class FBXLoader& loader);

public:
	void SaveDataBinary(HANDLE hFile);
	void LoadDataBinary(HANDLE hFile, const FbxMeshInfo* meshInfo);

private:
	void CreateVertexBuffer(const vector<Vertex>& buffer);
	void CreateIndexBuffer(const vector<uint32>& buffer);
	void CreateBonesAndAnimations(class FBXLoader& loader);
	void CreateBonesAndAnimations(void);

	Matrix GetMatrix(FbxAMatrix& matrix);

public:
	uint32 GetSubsetCount() { return static_cast<uint32>(m_vecIndexInfo.size()); }
	const vector<BoneInfo>*		GetBones() { return &m_bones; }
	uint32						GetBoneCount() { return static_cast<uint32>(m_bones.size()); }
	const vector<AnimClipInfo>* GetAnimClip() { return &m_animClips; }

	bool							IsAnimMesh() { return !m_animClips.empty(); }
	shared_ptr<StructuredBuffer>	GetBoneFrameDataBuffer(int32 index = 0) { return m_frameBuffer[index]; } // 전체 본 프레임 정보
	shared_ptr<StructuredBuffer>	GetBoneOffsetBuffer() { return  m_offsetBuffer; }

private:
	ComPtr<ID3D12Resource>		m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView = {};
	uint32 m_vertexCount = 0;

	vector<IndexBufferInfo>		m_vecIndexInfo;

	// Animation
	vector<AnimClipInfo>			m_animClips;
	vector<BoneInfo>				m_bones;

	shared_ptr<StructuredBuffer>	m_offsetBuffer; // 각 뼈의 offset 행렬
	vector<shared_ptr<StructuredBuffer>> m_frameBuffer; // 전체 본 프레임 정보
};

