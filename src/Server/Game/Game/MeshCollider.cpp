#include "pch.h"
#include "MeshCollider.h"
#include "PhysDevice.h"
#include "RigidBody.h"
#include "FBXLoader.h"
#include "ConvexMeshWrapper.h"

using namespace physx;

std::unordered_map<std::wstring, ConvexMeshWrapper*> MeshCollider::m_convexContainer;

PxGeometryHolder MeshCollider::CreateGeometry()
{
    return CreateTriangleMeshGeometry();
}

MeshCollider::MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent, std::wstring meshName, std::wstring path)
    : Collider(ownerGameObject, ownerComponent, body), m_meshName(meshName)
{
    ConvexMeshWrapper* convexwrapper = GetMyConvexWrapper();
    if (convexwrapper)
    {
        convexwrapper->AddReference();                          //container에 해당 mesh가 존재한다면 reference만 증가
        return;
    }
    else
    {
        convexwrapper = new ConvexMeshWrapper(path);            //container에 해당 mesh가 없다면 
        convexwrapper->Init();                                  //wrapper를 생성, 초기화, reference 증가 후 container에 보관
        convexwrapper->AddReference();
        m_convexContainer.emplace(meshName, convexwrapper);
    }
}

MeshCollider::MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent, std::wstring meshName, const FBXMeshInfomation& info)
    : Collider(ownerGameObject, ownerComponent, body), m_meshName(meshName)
{
    ConvexMeshWrapper* convexwrapper = GetMyConvexWrapper();
    if (convexwrapper)
    {
        convexwrapper->AddReference();                          //container에 해당 mesh가 존재한다면 reference만 증가
        return;
    }
    else
    {
        std::vector<PxVec3> vertices;

        for (const auto& vertice : info.m_vertices)
        {
            vertices.push_back(PxVec3(vertice.x, vertice.y, vertice.z));
        }

        convexwrapper = new ConvexMeshWrapper(vertices, info.m_indicies);
        convexwrapper->Init(vertices, info.m_indicies);                                  //wrapper를 생성, 초기화, reference 증가 후 container에 보관
        convexwrapper->AddReference();
        m_convexContainer.emplace(meshName, convexwrapper);
    }
}

MeshCollider::~MeshCollider()
{
}

void MeshCollider::Init()
{
    m_attachedRigidBody->SetKinematic(true);
    m_attachedRigidBody->GetBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);

    Collider::Init();
}

void MeshCollider::Release()
{
    auto it = m_convexContainer.find(m_meshName);
    if (it != m_convexContainer.end())
    {
        ConvexMeshWrapper* meshWrapperPtr = it->second;
        if (meshWrapperPtr->Release())
        {
            m_convexContainer.erase(it);
            delete meshWrapperPtr;
        }
    }

    Collider::Release();
}

PxConvexMeshGeometry MeshCollider::CreateTriangleMeshGeometry()
{
    PxMeshScale scale;
    scale.rotation = PxQuat(PxIdentity);
    scale.scale = PxVec3(10.f, 10.f, 10.f);
    return PxConvexMeshGeometry(GetMyConvexWrapper()->GetConvexMesh(), scale);
}

ConvexMeshWrapper* MeshCollider::GetMyConvexWrapper()
{
    auto it = m_convexContainer.find(m_meshName);
    if (it != m_convexContainer.end())
    {
        ConvexMeshWrapper* meshWrapperPtr = it->second;
        return meshWrapperPtr;
    }
    else
    {
        return nullptr;
    }
}
