#include "pch.h"
#include "FBXMapLoader.h"
#include "MeshData.h"
#include "Resources.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"

FBXMapLoader::FBXMapLoader()
{
}

FBXMapLoader::~FBXMapLoader()
{
}

void FBXMapLoader::AddBasicObject(std::wstring path)
{
	// 경로로 부터 fbx파일 정보를 읽어온다.
    std::shared_ptr<MeshData> v = MeshData::LoadFromFBX(path);

    vector<MeshRenderInfo> renderInfo = v.get()->GetMeshRenderInfo();
    for (auto& info : renderInfo)
    {
        m_assetInfo[info.mesh->GetName()] = info;
    }
     
	// 데이터는 자동으로 Resource에 저장된다.
	// 이를 가져오려면 GET_SINGLE(Resources)->Get<Mesh>(fbxName); 이렇게 가져오면 된다.
}

void FBXMapLoader::ExtractMapInfo(std::wstring _strFilePath)
{
	// 해당 경로를 가진 파일의 bin 형식이 존재하는지 확인

	FBXLoader loader;
	loader.makeBinaryFilePath(_strFilePath);
	wstring binaryPath = loader.GetBinaryFilePath();

	// .bin 파일 존재여부 확인
	HANDLE hFile = MeshData::CreateFileRead(binaryPath);

	// 파일이 잘 생성되었는지 검사
	if (INVALID_HANDLE_VALUE == hFile)
	{
        // 없다면 파일이 존재하지 않거나 오류가 생겼다는 의미. 다시 만든다.
        CloseHandle(hFile);
        
        // 파일을 추출
        ExtractObjectInfoFromFBXAscii(_strFilePath);


        // 추출한 파일을 가지고 .bin 파일 생성
		hFile = MeshData::CreateFileWrite(binaryPath);

        // 임시 이름, 위치 정보 저장
        uint32 size = m_mapDeployedObjectInfo.size();
        saveInt(hFile, size);
        for (auto& info : m_mapDeployedObjectInfo)
        {
            saveString(hFile, info.first);
            saveVecData<objectLocationInfo>(hFile, info.second);
        }

        // 임시 이름, 진짜 메쉬 이름 저장
        size = m_objectStaticMesh.size();
        saveInt(hFile, size);
        for (auto& info : m_objectStaticMesh)
        {
            saveString(hFile, info.first);
            saveString(hFile, info.second);
        }

        CloseHandle(hFile);

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
            wstring strName = loadString(hFile);
            vector<objectLocationInfo> v = loadVecData<objectLocationInfo>(hFile);

            m_mapDeployedObjectInfo[strName] = v;
        }


       count = loadInt(hFile);

        for (uint32 i = 0; i < count; ++i)
        {
            wstring strName1 = loadString(hFile);
            wstring strName2 = loadString(hFile);

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
            // 오브젝트 생성
            shared_ptr<CGameObject> gameObject = make_shared<CGameObject>();

            // 위치 넣어주기
            gameObject->AddComponent(make_shared<Transform>());
            {
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
                // objectInfo.Rotation.y, objectInfo.Rotation.z, objectInfo.Rotation.x
                gameObject->GetTransform()->SetLocalRotation(Vec3(objectInfo.Rotation.y, objectInfo.Rotation.z, objectInfo.Rotation.x));

                gameObject->GetTransform()->SetLocalPosition(Vec3(objectInfo.Position.x, objectInfo.Position.z, -objectInfo.Position.y));

                gameObject->GetTransform()->SetLocalScale(Vec3(objectInfo.Scale.x, objectInfo.Scale.z, objectInfo.Scale.y));
            }

            // 메쉬 정보 추가
            gameObject->AddComponent(make_shared<MeshRenderer>());

            std::shared_ptr<Mesh> mesh = m_assetInfo[m_objectStaticMesh[info.first]].mesh;

            if (nullptr != mesh)
            {
                gameObject->GetMeshRenderer()->SetMesh(mesh);

                for (uint32 i = 0; i < m_assetInfo[m_objectStaticMesh[info.first]].materials.size(); i++)
                    gameObject->GetMeshRenderer()->SetMaterial(m_assetInfo[m_objectStaticMesh[info.first]].materials[i], i);

                m_MapObjectInfo.push_back(gameObject);
            }

            else
                gameObject.reset();
        }
    }
}

void FBXMapLoader::ExtractObjectInfoFromFBXAscii(std::wstring path)
{
	ifstream file(path);

	if (!file.is_open()) {
		MSG_BOX(L"FBXMapLoader::ExtractObjectInfoFromTXT - Failed to open file.");
		return;
	}

    string line;
    string name;
    int translation[3];
    float scaling[3];
    float rotation[3];

    bool checkStart = false;
    uint32_t count = 0;

    while (getline(file, line)) {
        if (line.find("Model::") != string::npos) {

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
            if (line.find("Mesh") != string::npos)
            {
                checkStart = true;
            }

            size_t pos = line.find("::");
            if (pos != string::npos) {
                name = line.substr(pos + 2, line.find('"', pos + 2) - pos - 2);
            }
        }
        else if (line.find("Lcl Translation") != string::npos) {
            size_t pos = line.find("A\",");
            if (pos != string::npos) {
                sscanf_s(line.substr(pos + 3).c_str(), "%d,%d,%d", &translation[0], &translation[1], &translation[2]);
            }
        }
        else if (line.find("Lcl Scaling") != string::npos) {
            size_t pos = line.find("A\",");
            if (pos != string::npos) {
                sscanf_s(line.substr(pos + 3).c_str(), "%f,%f,%f", &scaling[0], &scaling[1], &scaling[2]);
            }
        }
        else if (line.find("Lcl Rotation") != string::npos) {
            size_t pos = line.find("A\",");
            if (pos != string::npos) {
                sscanf_s(line.substr(pos + 3).c_str(), "%f,%f,%f", &rotation[0], &rotation[1], &rotation[2]);
            }
        }
        else if (line.find("}") != string::npos) {

            if (checkStart)
            {
                objectLocationInfo info{};
                info.Position = Vec4(translation[0], -translation[1], translation[2], 0.f);
                info.Rotation = Vec4(rotation[0], rotation[1], -rotation[2], 0.f);
                info.Scale = Vec4(scaling[0], scaling[1], scaling[2], 1.f);

                wstring wstrName{ name.begin(), name.end() };
                m_mapDeployedObjectInfo[wstrName].push_back(info);

                checkStart = false;

                ++count;
            }
        }



        if (line.find(";Geometry::") != string::npos) { // check if the line starts with ";Geometry::"
            int pos1 = line.find("::") + 2; // get the position of the first data
            int pos2 = line.find(",", pos1); // get the position of the second data
            string data1 = line.substr(pos1, pos2 - pos1); // extract the first data
            string data2 = line.substr(pos2 + 9); // extract the second data

            wstring meshName{ data2.begin(), data2.end() };
            wstring staticMeshName{ data1.begin(), data1.end() }; 

            m_objectStaticMesh[meshName] = staticMeshName;
        }
    }
}
