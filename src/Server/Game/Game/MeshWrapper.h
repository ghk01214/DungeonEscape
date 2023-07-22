#pragma once

class MeshWrapper
{
public:
	MeshWrapper(std::wstring path, bool isConvex = false);
	MeshWrapper(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, bool isConvex = false);
	~MeshWrapper();

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
	void CreatePxTriangleMesh(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent);
	void CreatePxConvexMesh(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent);
	void CheckEmptyMesh(std::vector<physx::PxVec3>& vertices, std::vector<uint32_t>& indices);

public:
	std::wstring GetCurrentPath();
	physx::PxTriangleMesh* GetTriangleMesh();
	physx::PxConvexMesh* GetConvexMesh();

private:
	physx::PxTriangleMesh* m_triangleMesh = nullptr;
	physx::PxConvexMesh* m_convexMesh = nullptr;
	std::wstring m_meshPath = L"";


	int m_referenceCount = 0;


	bool m_isConvex = false;
};

