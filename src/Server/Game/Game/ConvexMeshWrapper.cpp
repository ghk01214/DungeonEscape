﻿#include "pch.h"
#include "ConvexMeshWrapper.h"
#include "PhysDevice.h"
#include "FBXLoader.h"

using namespace physx;
using namespace std;

ConvexMeshWrapper::ConvexMeshWrapper(std::wstring path):
	m_meshPath(path)
{
}

ConvexMeshWrapper::ConvexMeshWrapper(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices)
{

}

ConvexMeshWrapper::~ConvexMeshWrapper()
{
}

void ConvexMeshWrapper::PrepareVerticesIndices(std::vector<physx::PxVec3>& vertices, std::vector<uint32_t>& indices)
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

void ConvexMeshWrapper::Init(Vec3 extent)
{
#pragma region vertices/indices 초기화
    vector<PxVec3> vertices;
    vector<uint32_t> indices;

    PrepareVerticesIndices(vertices, indices);
    CheckEmptyMesh(vertices, indices);
#pragma endregion

#pragma region pxConvexMesh 생성
    CreatePxConvexMesh(vertices, indices, extent);
#pragma endregion
}

void ConvexMeshWrapper::Init(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent)
{
#pragma region pxConvexMesh 생성
    CreatePxConvexMesh(vertices, indices, extent);
#pragma endregion
}

void ConvexMeshWrapper::AddReference()
{
    m_referenceCount += 1;
}

void ConvexMeshWrapper::RemoveReference()
{
    m_referenceCount -= 1;
}

bool ConvexMeshWrapper::Release()
{
    RemoveReference();

    if (m_referenceCount < 0)
    {
        PX_RELEASE(m_convexMesh);
        m_meshPath = L"";
        return true;                //메모리 해제
    }

    return false;                   //RefCnt 감소
}

void ConvexMeshWrapper::CreatePxConvexMesh(const std::vector<physx::PxVec3>& vertices, const std::vector<uint32_t>& indices, Vec3 extent)
{
    auto phys = PhysDevice::GetInstance()->GetPhysics();
    auto cooking = PhysDevice::GetInstance()->GetCooking();


    // Apply scale to vertices
    float scaleFactor = 0.01f; // 1/100
    std::vector<physx::PxVec3> scaledVertices(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        scaledVertices[i].x = vertices[i].x * extent.x * 0.01f;
        scaledVertices[i].y = vertices[i].y * extent.y * 0.01f;
        scaledVertices[i].z = vertices[i].z * extent.z * 0.01f;      //0.01 unit interval between unreal and physx
    }

    PxConvexMeshDesc  desc;
    desc.points.count = scaledVertices.size();
    desc.points.stride = sizeof(PxVec3);
    desc.points.data = scaledVertices.data();

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
}

void ConvexMeshWrapper::CheckEmptyMesh(std::vector<physx::PxVec3>& vertices, std::vector<uint32_t>& indices)
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

std::wstring ConvexMeshWrapper::GetCurrentPath()
{
    return m_meshPath;
}

physx::PxConvexMesh* ConvexMeshWrapper::GetConvexMesh()
{
	return m_convexMesh;
}

