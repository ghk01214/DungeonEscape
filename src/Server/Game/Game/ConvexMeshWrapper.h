#pragma once
class ConvexMeshWrapper
{
public:
	ConvexMeshWrapper(std::wstring path);
	~ConvexMeshWrapper();

private:
	//Init에서 작동할 함수
	void PrepareVerticesIndices(std::vector<physx::PxVec3>& vertices, std::vector<uint32_t>& indices);

public:
	void Init();
	void AddReference();
	void RemoveReference();
	bool Release();

public:
	std::wstring GetCurrentPath();
	physx::PxConvexMesh* GetConvexMesh();

private:
	physx::PxConvexMesh* m_convexMesh = nullptr;
	std::wstring m_meshPath = L"";


	int m_referenceCount = 0;
};

