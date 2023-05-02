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
        ExtractObjectInfoFromFBXAscii(_strFilePath);


  //      // 추출한 파일을 가지고 .bin 파일 생성
		//hFile = FBXLoader::CreateFileWrite(binaryPath);

  //      // 임시 이름, 위치 정보 저장
  //      uint32 size = m_mapDeployedObjectInfo.size();
  //      saveInt(hFile, size);
  //      for (auto& info : m_mapDeployedObjectInfo)
  //      {
  //          saveString(hFile, info.first);
  //          saveVecData<objectLocationInfo>(hFile, info.second);
  //      }

  //      // 임시 이름, 진짜 메쉬 이름 저장
  //      size = m_objectStaticMesh.size();
  //      saveInt(hFile, size);
  //      for (auto& info : m_objectStaticMesh)
  //      {
  //          saveString(hFile, info.first);
  //          saveString(hFile, info.second);
  //      }

  //      CloseHandle(hFile);

		//wstring str{ L"FBXMapLoader::ExtractMapInfo - Failed to CreateFile From : " };
		//str += _strFilePath;
		//MSG_BOX(str.c_str());
		//return;
	}
    else
    {
        uint32 count = loadInt(hFile);

        for (uint32 i = 0; i < count; ++i)
        {
            std::wstring strName = loadString(hFile);
            std::vector<objectLocationInfo> v = loadVecData<objectLocationInfo>(hFile);

            m_mapDeployedObjectInfo[strName] = v;
        }


       count = loadInt(hFile);

        for (uint32 i = 0; i < count; ++i)
        {
            std::wstring strName1 = loadString(hFile);
            std::wstring strName2 = loadString(hFile);

            m_objectStaticMesh[strName1] = strName2;
        }

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
            // 위치 넣어주기
            /*
                ue
                정면 x
                우측 y
                상단 z

                directx
                정면 z
                우측 x
                상단 y

                x -> z, y -> x, z -> y
            */
            objectLocationInfo locationInfo;

            locationInfo.Position = FBXVec4(objectInfo.Position.x, objectInfo.Position.z, -objectInfo.Position.y, 1.f);
            locationInfo.Rotation = FBXVec4(objectInfo.Rotation.y, objectInfo.Rotation.z, objectInfo.Rotation.x, 1.f);
            locationInfo.Scale = FBXVec4(objectInfo.Scale.x, objectInfo.Scale.z, objectInfo.Scale.y, 1.f);

            std::wstring str111 = m_objectStaticMesh[info.first];

            if (m_assetInfo.find(m_objectStaticMesh[info.first]) != m_assetInfo.end())
            {
                auto& fbxinfo = m_assetInfo[m_objectStaticMesh[info.first]];

                m_MapObjectInfo.push_back(std::make_pair(m_objectStaticMesh[info.first], locationInfo));
            }
        }
    }
}

const FBXMeshInfomation& FBXMapLoader::FindVertexIndicesInfo(std::wstring statisMeshName)
{
    // nullptr check 할 것
    return m_assetInfo[statisMeshName];
}

void FBXMapLoader::ExtractObjectInfoFromFBXAscii(std::wstring path)
{
	std::ifstream file(path);

	if (!file.is_open()) {
		//MSG_BOX(L"FBXMapLoader::ExtractObjectInfoFromTXT - Failed to open file.");
		return;
	}

    std::string line;
    std::string name;
    int translation[3];
    float scaling[3];
    float rotation[3];

    bool checkStart = false;
    uint32_t count = 0;

    while (getline(file, line)) {
        if (line.find("Model::") != std::string::npos) {

            translation[0] = 0;
            translation[1] = 0;
            translation[2] = 0;

            scaling[0] = 1.f;
            scaling[1] = 1.f;
            scaling[2] = 1.f;

            rotation[0] = 0.f;
            rotation[1] = 0.f;
            rotation[2] = 0.f;

            // "Mesh" 가 해당 라인에 존재할 경우
            if (line.find("Mesh") != std::string::npos)
            {
                checkStart = true;
            }

            size_t pos = line.find("::");
            if (pos != std::string::npos) {
                name = line.substr(pos + 2, line.find('"', pos + 2) - pos - 2);
            }
        }
        else if (line.find("Lcl Translation") != std::string::npos) {
            size_t pos = line.find("A\",");
            if (pos != std::string::npos) {
                sscanf_s(line.substr(pos + 3).c_str(), "%d,%d,%d", &translation[0], &translation[1], &translation[2]);
            }
        }
        else if (line.find("Lcl Scaling") != std::string::npos) {
            size_t pos = line.find("A\",");
            if (pos != std::string::npos) {
                sscanf_s(line.substr(pos + 3).c_str(), "%f,%f,%f", &scaling[0], &scaling[1], &scaling[2]);
            }
        }
        else if (line.find("Lcl Rotation") != std::string::npos) {
            size_t pos = line.find("A\",");
            if (pos != std::string::npos) {
                sscanf_s(line.substr(pos + 3).c_str(), "%f,%f,%f", &rotation[0], &rotation[1], &rotation[2]);
            }
        }
        else if (line.find("}") != std::string::npos) {

            if (checkStart)
            {
                objectLocationInfo info{};
                info.Position = FBXVec4(translation[0], -translation[1], translation[2], 0.f);
                info.Rotation = FBXVec4(rotation[0], rotation[1], -rotation[2], 0.f);
                info.Scale = FBXVec4(scaling[0], scaling[1], scaling[2], 1.f);

                std::wstring wstrName{ name.begin(), name.end() };
                m_mapDeployedObjectInfo[wstrName].push_back(info);

                checkStart = false;

                ++count;
            }
        }



        if (line.find(";Geometry::") != std::string::npos) { // check if the line starts with ";Geometry::"
            int pos1 = line.find("::") + 2; // get the position of the first data
            int pos2 = line.find(",", pos1); // get the position of the second data
            std::string data1 = line.substr(pos1, pos2 - pos1); // extract the first data
            std::string data2 = line.substr(pos2 + 9); // extract the second data

            std::wstring meshName{ data2.begin(), data2.end() };
            std::wstring staticMeshName{ data1.begin(), data1.end() }; 

            m_objectStaticMesh[meshName] = staticMeshName;
        }
    }
}
