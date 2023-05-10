#include "pch.h"
#include "FBXMapLoader.h"
#include "Transform.h"
#include "MapObject.h"

FBXMapLoader::FBXMapLoader()
{
}

FBXMapLoader::~FBXMapLoader()
{
}

void FBXMapLoader::AddBasicObject(std::wstring path)
{
    FBXLoader loader;
    loader.makeBinaryFilePath(path);
    std::wstring binaryPath = loader.GetBinaryFilePath();

    loader.LoadFbx(path);

    std::vector<FBXVec3> vertices;
    std::vector<uint32> indicies;

    for (int32 i = 0; i < loader.GetMeshCount(); i++)
    {
        const FbxMeshInfo& mesh = loader.GetMesh(i);

        // 존재 여부 확인, 이미 존재하면 넘어가기
        if (m_assetInfo.find(mesh.name) != m_assetInfo.end())
            continue;

        // 버텍스 정보 로드
        vertices.clear();
        for (auto& vertex : mesh.vertices)
        {
            vertices.push_back(vertex.pos);
        }

        // 인디시즈 정보 로드
        indicies.clear();
        for (auto& indices : mesh.indices)
        {
            for (auto& index : indices)
            {
                indicies.push_back(index);
            }
        }

        FBXMeshInfomation info;
        info.m_vertices = vertices;
        info.m_indicies = indicies;
        m_assetInfo[mesh.name] = info;
    }
}

void FBXMapLoader::ExtractMapInfo(std::wstring _strFilePath)
{
	// 해당 경로를 가진 파일의 bin 형식이 존재하는지 확인
	FBXLoader loader;
	loader.makeBinaryFilePath(_strFilePath);
	std::wstring binaryPath = loader.GetBinaryFilePath();

	// .bin 파일 존재여부 확인
	HANDLE hFile = FBXLoader::CreateFileRead(binaryPath);

	// 파일이 잘 생성되었는지 검사
	if (INVALID_HANDLE_VALUE == hFile)
	{
        // 없다면 파일이 존재하지 않거나 오류가 생겼다는 의미. 다시 만든다.
        CloseHandle(hFile);

        // 파일을 추출
        ExtractObjectInfoFromFBX(_strFilePath);
	}
    else
    {
        CloseHandle(hFile);
    }

    // 이제 가지고 있는 파일들을 사용해 메쉬 데이터를 만든다.
    MakeMap();
}

void FBXMapLoader::MakeMap(void)
{
    m_MapObjectInfo.clear();

    for (auto& info : m_mapDeployedObjectInfo)
    {
        for (auto& objectInfo : info.second)
        {
            objectLocationInfo locationInfo;

            locationInfo.Position = FBXVec4(objectInfo.Position.x, objectInfo.Position.z, objectInfo.Position.y, 1.f);
            locationInfo.Rotation = FBXVec4(objectInfo.Rotation.y, -objectInfo.Rotation.z, objectInfo.Rotation.x, 1.f);
            locationInfo.Scale = FBXVec4(objectInfo.Scale.x, objectInfo.Scale.z, objectInfo.Scale.y, 1.f);
            locationInfo.qLocal = objectInfo.qLocal;
            locationInfo.qWorld = objectInfo.qWorld;

            if (m_assetInfo.find(info.first) != m_assetInfo.end())
            {
                auto& fbxinfo = m_assetInfo[info.first];

                m_MapObjectInfo.push_back(std::make_pair(info.first, locationInfo));
            }
        }
    }
}

const FBXMeshInfomation& FBXMapLoader::FindVertexIndicesInfo(std::wstring statisMeshName)
{
    // nullptr check 할 것
    return m_assetInfo[statisMeshName];
}

DirectX::XMFLOAT4 FBXMapLoader::ConvertFbxToDirectQuaternion(const FbxQuaternion& q)
{
    return DirectX::XMFLOAT4(static_cast<float>(q[0]), static_cast<float>(q[1]), static_cast<float>(q[2]), static_cast<float>(q[3]));
}

Vec3 FBXMapLoader::ConvertFbxDouble3ToVector3(const FbxDouble3& v)
{
    return Vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));
}

void FBXMapLoader::ExtractObjectInfoFromFBX(std::wstring path)
{
    FBXLoader loader;
    loader.LoadFbx(path);

    for (uint32 i = 0; i < loader.GetMeshCount(); ++i)
    {
        const std::wstring& wstrName = loader.GetMesh(i).name;
        objectLocationInfo locationInfo{};
        locationInfo.Position = ConvertFbxDouble3ToVector3(loader.GetMesh(i).transform.translation);
        locationInfo.Rotation = ConvertFbxDouble3ToVector3(loader.GetMesh(i).transform.rotation);
        locationInfo.Scale = ConvertFbxDouble3ToVector3(loader.GetMesh(i).transform.scaling);
        locationInfo.qLocal = loader.GetMesh(i).transform.qLocal;
        locationInfo.qWorld = loader.GetMesh(i).transform.qWorld;

        m_mapDeployedObjectInfo[wstrName].push_back(locationInfo);
    }
}