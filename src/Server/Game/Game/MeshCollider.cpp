#include "pch.h"
#include "MeshCollider.h"
#include "PhysDevice.h"
#include "RigidBody.h"
#include "FBXLoader.h"
#include "MeshWrapper.h"
#include "GameObject.h"
#include "Transform.h"

using namespace physx;

std::unordered_map<std::wstring, MeshWrapper*> MeshCollider::m_meshWrapperContainer;

PxGeometryHolder MeshCollider::CreateGeometry()
{
    if (m_isConvex)
        return CreateConvexMeshGeometry();
    else
        return CreateTriangleMeshGeometry();
}

MeshCollider::MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent, std::wstring meshName, std::wstring path, bool isConvex)
    : Collider(ownerGameObject, ownerComponent, body), m_meshName(meshName), m_isConvex(isConvex)
{
    MeshWrapper* meshWrapper = GetMyMeshWrapper();
    if (meshWrapper)
    {
        meshWrapper->AddReference();                          //container에 해당 mesh가 존재한다면 reference만 증가
        return;
    }
    else
    {
        meshWrapper = new MeshWrapper(path, isConvex);            //container에 해당 mesh가 없다면 
        meshWrapper->Init(extent);                                  //wrapper를 생성, 초기화, reference 증가 후 container에 보관
        meshWrapper->AddReference();
        m_meshWrapperContainer.emplace(meshName, meshWrapper);
    }
}

MeshCollider::MeshCollider(GameObject* ownerGameObject, Component* ownerComponent, RigidBody* body, Vec3 extent, std::wstring meshName, const FBXMeshInfomation& info, bool isConvex)
    : Collider(ownerGameObject, ownerComponent, body), m_meshName(meshName), m_isConvex(isConvex)
{
	std::vector<PxVec3> vertices;

	for (const auto& vertice : info.m_vertices)
	{
		vertices.push_back(PxVec3(vertice.x, vertice.y, vertice.z));
	}

	MeshWrapper* meshWrapper = new MeshWrapper(vertices, info.m_indicies, isConvex);
	meshWrapper->Init(vertices, info.m_indicies, extent);                                  //wrapper를 생성, 초기화, reference 증가 후 container에 보관
	meshWrapper->AddReference();
	m_meshWrapperContainer.emplace(meshName, meshWrapper);
}

MeshCollider::~MeshCollider()
{
}

void MeshCollider::Init()
{
    if (!m_isConvex)
    {
        m_attachedRigidBody->SetKinematic(true);
        m_attachedRigidBody->GetBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
    }

    Collider::Init();
}

void MeshCollider::Release()
{
    auto it = m_meshWrapperContainer.find(m_meshName);
    if (it != m_meshWrapperContainer.end())
    {
        MeshWrapper* meshWrapperPtr = it->second;
        if (meshWrapperPtr->Release())
        {
            m_meshWrapperContainer.erase(it);
            delete meshWrapperPtr;
        }
    }

    Collider::Release();
}

PxTriangleMeshGeometry MeshCollider::CreateTriangleMeshGeometry()
{
    PxMeshScale scale;

    auto trans = m_ownerGameObject->GetTransform();
    auto size = trans->GetScale();
    scale.rotation = PxQuat(PxIdentity);
    scale.scale = PxVec3(size.x, size.y, size.z);
    return PxTriangleMeshGeometry(GetMyMeshWrapper()->GetTriangleMesh(), scale);
}

physx::PxConvexMeshGeometry MeshCollider::CreateConvexMeshGeometry()
{
    PxMeshScale scale;

    auto trans = m_ownerGameObject->GetTransform();
    auto size = trans->GetScale();
    scale.rotation = PxQuat(PxIdentity);
    scale.scale = PxVec3(size.x, size.y, size.z);
    return PxConvexMeshGeometry(GetMyMeshWrapper()->GetConvexMesh(), scale);
}

MeshWrapper* MeshCollider::GetMyMeshWrapper()
{
    auto it = m_meshWrapperContainer.find(m_meshName);
    if (it != m_meshWrapperContainer.end())
    {
        MeshWrapper* meshWrapperPtr = it->second;
        return meshWrapperPtr;
    }
    else
    {
        return nullptr;
    }
}
