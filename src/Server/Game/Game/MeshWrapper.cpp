#include "pch.h"
#include "MeshWrapper.h"
#include "PhysDevice.h"
#include "FBXLoader.h"

using namespace physx;
using namespace std;

MeshWrapper::MeshWrapper(std::wstring path, bool isConvex):
	m_meshPath(path), m_isConvex(isConvex)
{
}

MeshWrapper::MeshWrapper(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, bool isConvex)
    : m_isConvex(isConvex)
{

}

MeshWrapper::~MeshWrapper()
{
}

void MeshWrapper::Init(Vec3 extent)
{
#pragma region vertices/indices 초기화
    vector<PxVec3> vertices;
    vector<uint32_t> indices;

    PrepareVerticesIndices(vertices, indices);
    CheckEmptyMesh(vertices, indices);
#pragma endregion

#pragma region pxTriangleMesh 혹은 pxConvexMesh 생성
    if (m_isConvex)
        CreatePxConvexMesh(vertices, indices, extent);
    else
        CreatePxTriangleMesh(vertices, indices, extent);
#pragma endregion
}

void MeshWrapper::Init(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent)
{
#pragma region pxTriangleMesh 혹은 pxConvexMesh 생성
    if (m_isConvex)
        CreatePxConvexMesh(vertices, indices, extent);
    else
        CreatePxTriangleMesh(vertices, indices, extent);
#pragma endregion
}

void MeshWrapper::AddReference()
{
    m_referenceCount += 1;
}

void MeshWrapper::RemoveReference()
{
    m_referenceCount -= 1;
}

bool MeshWrapper::Release()
{
    RemoveReference();

    if (m_referenceCount < 0)
    {
        if (m_isConvex)
        {
            PX_RELEASE(m_convexMesh);
        }
        else
        {
            PX_RELEASE(m_triangleMesh);
        }

        m_meshPath = L"";
        return true;                //메모리 해제
    }

    return false;                   //RefCnt 감소
}

void MeshWrapper::PrepareVerticesIndices(std::vector<physx::PxVec3>& vertices, std::vector<uint32_t>& indices)
{
    FBXLoader loader;
    loader.makeBinaryFilePath(m_meshPath);
    wstring binaryPath = loader.GetBinaryFilePath();

    // 읽기 파일 생성
    HANDLE hFile = FBXLoader::CreateFileRead(binaryPath);

    // 파일이 존재하지 않는다면
    if (INVALID_HANDLE_VALUE == hFile)
    {
        //throw std::runtime_error("Invalid Mesh File Name.");

        // bin파일 생성 및 저장
        hFile = FBXLoader::CreateFileWrite(binaryPath);

        loader.LoadFbx(m_meshPath);
        loader.SaveFbxToBinary(hFile);

        // 파일을 닫고
        CloseHandle(hFile);

        // 읽기 파일 생성
        hFile = FBXLoader::CreateFileRead(binaryPath);
    }

    // bin 파일로부터 데이터 읽어오기
    loader.LoadFbxFromBinary(hFile);

    // 메쉬 정보 가져오기
    auto& meshInfo = loader.GetMeshInfo();

    // 현재는 다수의 메쉬가 있더라도 하나의 정보만 가공된 형태
    for (auto& mesh : meshInfo)
    {
        for (auto& vertice : mesh.m_vertices)
        {
            vertices.push_back(physx::PxVec3(vertice.x, vertice.y, vertice.z));
        }
        for (auto& indice : mesh.m_indicies)
        {
            indices.push_back(uint32_t(indice));
        }
    }

    CloseHandle(hFile);
}

void MeshWrapper::CreatePxTriangleMesh(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent)
{
    auto phys = PhysDevice::GetInstance()->GetPhysics();
    auto cooking = PhysDevice::GetInstance()->GetCooking();

    // Apply scale to vertices
    std::vector<physx::PxVec3> scaledVertices(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        scaledVertices[i].x = (vertices[i].x) * PX_SCALE_FACTOR;
        scaledVertices[i].y = (vertices[i].y) * PX_SCALE_FACTOR;
        scaledVertices[i].z = (vertices[i].z) * PX_SCALE_FACTOR;      //0.01 unit interval between unreal and physx
    }

    // Create description for triangle mesh
    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = scaledVertices.size();
    meshDesc.points.stride = sizeof(physx::PxVec3);
    meshDesc.points.data = scaledVertices.data();

    meshDesc.triangles.count = indices.size() / 3;
    meshDesc.triangles.stride = 3 * sizeof(PxU32);
    meshDesc.triangles.data = indices.data();


    // Validate the description
    assert(meshDesc.isValid());

    // Cook the triangle mesh
    physx::PxDefaultMemoryOutputStream writeBuffer;
    if (!cooking->cookTriangleMesh(meshDesc, writeBuffer)) {
        // handle error
        std::cout << "triangleMeshWraperErr\n";
        return;
    }

    // Create the triangle mesh
    physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    m_triangleMesh = phys->createTriangleMesh(readBuffer);

    scaledVertices.clear();
}

void MeshWrapper::CreatePxConvexMesh(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent)
{
    auto phys = PhysDevice::GetInstance()->GetPhysics();
    auto cooking = PhysDevice::GetInstance()->GetCooking();

    // Apply scale to vertices
    std::vector<physx::PxVec3> scaledVertices(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        scaledVertices[i].x = (vertices[i].x) * PX_SCALE_FACTOR;
        scaledVertices[i].y = (vertices[i].y) * PX_SCALE_FACTOR;
        scaledVertices[i].z = (vertices[i].z) * PX_SCALE_FACTOR;
    }

    // Create description for convex mesh
    physx::PxConvexMeshDesc convexDesc;
    convexDesc.points.count = scaledVertices.size();
    convexDesc.points.stride = sizeof(physx::PxVec3);
    convexDesc.points.data = scaledVertices.data();

    // If you have indices and want the convex hull of a subset of the vertices, use this
    // Note: This is optional. If you want the convex hull of all vertices, you can skip this part
    convexDesc.indices.count = indices.size();
    convexDesc.indices.stride = sizeof(uint32_t);
    convexDesc.indices.data = indices.data();

    convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX; // Enables the convex hull computation



    // Validate the description
    assert(convexDesc.isValid());

    // Cook the convex mesh
    physx::PxDefaultMemoryOutputStream writeBuffer;
    if (!cooking->cookConvexMesh(convexDesc, writeBuffer))
    {
        // handle error
        std::cout << "convexMeshWraperErr\n";
        return;
    }

    // Create the convex mesh
    physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    m_convexMesh = phys->createConvexMesh(readBuffer);

    scaledVertices.clear();
}

void MeshWrapper::CheckEmptyMesh(std::vector<physx::PxVec3>& vertices, std::vector<uint32_t>& indices)
{
    if (vertices.empty() && indices.empty())
    {
        std::vector<physx::PxVec3> verticesExample = {
            // Base vertices
            {-1, 0, -1}, {1, 0, -1}, {1, 0, 1}, {-1, 0, 1},
            // Apex
            {0, 2, 0},
        };
        vertices = verticesExample;

        std::vector<uint32_t> indicesExample = {
            // Base
            0, 1, 2, 0, 2, 3,
            // Sides
            0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4,
        };
        indices = indicesExample;
    }
}

std::wstring MeshWrapper::GetCurrentPath()
{
    return m_meshPath;
}

physx::PxTriangleMesh* MeshWrapper::GetTriangleMesh()
{
	return m_triangleMesh;
}

physx::PxConvexMesh* MeshWrapper::GetConvexMesh()
{
    return m_convexMesh;
}

