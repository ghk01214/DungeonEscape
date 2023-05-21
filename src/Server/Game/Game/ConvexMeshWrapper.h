#pragma once
class ConvexMeshWrapper
{
public:
	ConvexMeshWrapper(std::wstring path);
	ConvexMeshWrapper(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices);
	~ConvexMeshWrapper();

private:
	//Init에서 작동할 함수
	void PrepareVerticesIndices(std::vector<physx::PxVec3>& vertices, std::vector<uint32_t>& indices);

public:
	void Init(Vec3 extent);
	void Init(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent);
	void AddReference();
	void RemoveReference();
	bool Release();

private:
	void CreatePxConvexMesh(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent);
	void CheckEmptyMesh(std::vector<physx::PxVec3>& vertices, std::vector<uint32_t>& indices);

public:
	std::wstring GetCurrentPath();
	physx::PxTriangleMesh* GetConvexMesh();

private:
	physx::PxTriangleMesh* m_convexMesh = nullptr;
	std::wstring m_meshPath = L"";


	int m_referenceCount = 0;
};

