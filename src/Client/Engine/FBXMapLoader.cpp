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
    std::shared_ptr<MeshData> v = GET_SINGLE(Resources)->LoadFBX(path);

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

                Matrix matScale = Matrix::CreateScale(Vec3(objectInfo.Scale.x, objectInfo.Scale.z, objectInfo.Scale.y));
                /*Matrix matRotation = Matrix::CreateRotationX(XMConvertToRadians(objectInfo.Rotation.y));
                matRotation *= Matrix::CreateRotationY(XMConvertToRadians(-objectInfo.Rotation.z));
                matRotation *= Matrix::CreateRotationZ(XMConvertToRadians(objectInfo.Rotation.x));*/
                Matrix matRotation = Matrix::CreateRotationX(XMConvertToRadians(-objectInfo.Rotation.x));
                matRotation *= Matrix::CreateRotationY(XMConvertToRadians(-objectInfo.Rotation.z));
                matRotation *= Matrix::CreateRotationZ(XMConvertToRadians(-objectInfo.Rotation.y));
                Matrix matTranslation = Matrix::CreateTranslation(Vec3(objectInfo.Position.x, objectInfo.Position.z, objectInfo.Position.y));
                Matrix matWorld = matScale * matRotation * matTranslation;

                gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

                gameObject->GetTransform()->SetWorldMatrix(matWorld);
            }

            // 메쉬 정보 추가
            gameObject->AddComponent(make_shared<MeshRenderer>());

            std::shared_ptr<Mesh> mesh = m_assetInfo[info.first].mesh;


            if (nullptr != mesh)
            {
                gameObject->GetMeshRenderer()->SetMesh(mesh);

                for (uint32 i = 0; i < m_assetInfo[info.first].materials.size(); i++)
                {
                    gameObject->GetMeshRenderer()->SetMaterial(m_assetInfo[info.first].materials[i], i);
                }

                if (info.second.size() == 1)
                    gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 0);
                else
                    gameObject->GetMeshRenderer()->GetMaterial()->SetInt(0, 1);

                m_MapObjectInfo.push_back(gameObject);
            }

            else
                gameObject.reset();


            gameObject->SetName(info.first);

            // 여기서 그림자 적용을 할 static mesh 이름 설정
            if (info.first == L"SM_Round_Pillar")
            {
                gameObject->SetStatic(false);   // -> false로 하면 그림자 나옴, true면 적용 x
            }
            else 
                gameObject->SetStatic(true);

        }
    }
}

void FBXMapLoader::ExtractObjectInfoFromFBX(std::wstring path)
{
    FBXLoader loader;
    loader.LoadFbx(path);

    m_mapDeployedObjectInfo.clear();

    for (uint32 i = 0; i < loader.GetMeshCount(); ++i)
    {
        const std::wstring& wstrName = loader.GetMesh(i).name;
        objectLocationInfo locationInfo{};
        locationInfo.Position = MeshData::ConvertFbxDouble3ToVector3(loader.GetMesh(i).transform.translation);
        locationInfo.Rotation = MeshData::ConvertFbxDouble3ToVector3(loader.GetMesh(i).transform.rotation);
        locationInfo.Scale = MeshData::ConvertFbxDouble3ToVector3(loader.GetMesh(i).transform.scaling);

        m_mapDeployedObjectInfo[wstrName].push_back(locationInfo);
    }
}