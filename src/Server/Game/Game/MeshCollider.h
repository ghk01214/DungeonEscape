#pragma once

#include "physx_define.h"
#include "Collider.h"

	

class ConvexMeshWrapper;

class MeshCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent, std::wstring meshName, std::wstring path = L"");
	MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent, std::wstring meshName, const FBXMeshInfomation& locationInfo);
	~MeshCollider();

public:
	void Init() override;
	void Release() override;

public:
	static std::unordered_map<std::wstring, ConvexMeshWrapper*>& GetConvexContainder(void) { return m_convexContainer; }

protected:
	physx::PxTriangleMeshGeometry CreateTriangleMeshGeometry();

private:
	ConvexMeshWrapper* GetMyConvexWrapper();

private:
	std::wstring m_meshName = L"";
	static std::unordered_map<std::wstring, ConvexMeshWrapper*> m_convexContainer;

	static std::unordered_map<std::wstring, ConvexMeshWrapper*> m_staticMeshContainer;
};



