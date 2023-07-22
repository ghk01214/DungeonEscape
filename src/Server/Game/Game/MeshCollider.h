#pragma once

#include "physx_define.h"
#include "Collider.h"

	

class MeshWrapper;

class MeshCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent, std::wstring meshName, std::wstring path = L"", bool isConvex = false);
	MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent, std::wstring meshName, const FBXMeshInfomation& locationInfo, bool isConvex = false);
	~MeshCollider();

public:
	void Init() override;
	void Release() override;

public:
	static std::unordered_map<std::wstring, MeshWrapper*>& GetMeshWrapperContainder(void) { return m_meshWrapperContainer; }

protected:
	physx::PxTriangleMeshGeometry CreateTriangleMeshGeometry();
	physx::PxConvexMeshGeometry CreateConvexMeshGeometry();

private:
	MeshWrapper* GetMyMeshWrapper();

private:
	std::wstring m_meshName = L"";
	static std::unordered_map<std::wstring, MeshWrapper*> m_meshWrapperContainer;
	static std::unordered_map<std::wstring, MeshWrapper*> m_staticMeshContainer;

	bool m_isConvex = false;
};



