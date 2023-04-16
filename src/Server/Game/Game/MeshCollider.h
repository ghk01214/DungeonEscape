#pragma once

#include "physx_define.h"
#include "Collider.h"

class MeshCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body);
	~MeshCollider();
public:
	void Init() override;
	void Release() override;

protected:
	physx::PxConvexMeshGeometry CreateTriangleMeshGeometry();
private:
	physx::PxConvexMesh* m_convexMesh = nullptr;
};



