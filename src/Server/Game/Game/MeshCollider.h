#pragma once

#include "physx_define.h"
#include "Collider.h"

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



