#pragma once
#include "physx_utils.h"

class Collider;

class MeshCollider : public Collider
{
	virtual physx::PxGeometryHolder CreateGeometry() override;

public:
	MeshCollider();
	~MeshCollider();
public:
	void Init(RigidBody* body);

protected:
	physx::PxConvexMeshGeometry CreateTriangleMeshGeometry();
private:
	physx::PxConvexMesh* m_mesh;
};



