#include "pch.h"
#include "MeshCollider.h"

physx::PxGeometryHolder MeshCollider::CreateGeometry()
{
    return CreateTriangleMeshGeometry();
}

MeshCollider::MeshCollider()
{
}

MeshCollider::~MeshCollider()
{
}

void MeshCollider::Init(RigidBody* body)
{
    auto phys = PhysDevice::GetInstance()->GetPhysics();
    auto cooking = PhysDevice::GetInstance()->GetCooking();

    std::vector<physx::PxVec3> vertices = {
        // Base vertices
        {-1, 0, -1}, {1, 0, -1}, {1, 0, 1}, {-1, 0, 1},
        // Apex
        {0, 2, 0},
    };
    std::vector<uint32_t> indices = {
        // Base
        0, 1, 2, 0, 2, 3,
        // Sides
        0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4,
    };

    physx::PxConvexMeshDesc  desc;
    desc.points.count = vertices.size();
    desc.points.stride = sizeof(physx::PxVec3);
    desc.points.data = vertices.data();

    desc.indices.count = indices.size(); //numface
    desc.indices.stride = sizeof(physx::PxVec3);
    desc.indices.data = indices.data();

    desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

    physx::PxDefaultMemoryOutputStream writeBuffer;
    physx::PxConvexMeshCookingResult::Enum result;
    bool cookingTriangleStatus = cooking->cookConvexMesh(desc, writeBuffer, &result);
    assert(cookingTriangleStatus);

    physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

    m_mesh = phys->createConvexMesh(readBuffer);

    body->SetKinematic(true);
    body->GetBody()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, false);

    Collider::Init(body);
}

physx::PxConvexMeshGeometry MeshCollider::CreateTriangleMeshGeometry()
{
    physx::PxMeshScale scale;
    scale.rotation = physx::PxQuat(physx::PxIdentity);
    scale.scale = physx::PxVec3(10.f, 10.f, 10.f);
    return physx::PxConvexMeshGeometry(m_mesh, scale);
}
