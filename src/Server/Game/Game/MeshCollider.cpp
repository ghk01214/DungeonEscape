﻿#include "pch.h"
#include "MeshCollider.h"
#include "PhysDevice.h"
#include "RigidBody.h"

using namespace physx;

PxGeometryHolder MeshCollider::CreateGeometry()
{
    return CreateTriangleMeshGeometry();
}

MeshCollider::MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body)
    : Collider(ownerGameObject, ownerComponent, body)
{
}

MeshCollider::~MeshCollider()
{
}

void MeshCollider::Init()
{
    auto phys = PhysDevice::GetInstance()->GetPhysics();
    auto cooking = PhysDevice::GetInstance()->GetCooking();

    std::vector<PxVec3> vertices = {
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

    PxConvexMeshDesc  desc;
    desc.points.count = vertices.size();
    desc.points.stride = sizeof(PxVec3);
    desc.points.data = vertices.data();

    desc.indices.count = indices.size(); //numface
    desc.indices.stride = sizeof(PxVec3);
    desc.indices.data = indices.data();

    desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    PxDefaultMemoryOutputStream writeBuffer;
    PxConvexMeshCookingResult::Enum result;
    bool cookingTriangleStatus = cooking->cookConvexMesh(desc, writeBuffer, &result);
    assert(cookingTriangleStatus);

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

    m_convexMesh = phys->createConvexMesh(readBuffer);

    m_attachedRigidBody->SetKinematic(true);
    m_attachedRigidBody->GetBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);

    Collider::Init();
}

void MeshCollider::Release()
{
    PX_RELEASE(m_convexMesh);
    Collider::Release();
}

PxConvexMeshGeometry MeshCollider::CreateTriangleMeshGeometry()
{
    PxMeshScale scale;
    scale.rotation = PxQuat(PxIdentity);
    scale.scale = PxVec3(10.f, 10.f, 10.f);
    return PxConvexMeshGeometry(m_convexMesh, scale);
}
