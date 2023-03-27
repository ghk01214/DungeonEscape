#include "pch.h"
#include "MeshData.h"
#include "FBXLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Animator.h"

MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA)
{
}

MeshData::~MeshData()
{
}

shared_ptr<MeshData> MeshData::LoadFromFBX(const wstring& path)
{
	FBXLoader loader;

	// Binary 파일 존재여부 확인
	loader.makeBinaryFilePath(path);
	wstring binaryPath = loader.GetBinaryFilePath();

	// .bin 파일 존재여부 확인
	// CreateFile : 읽기 파일(GENERIC_READ), 존재하면 열기(OPEN_EXISTING), 전용 역할 부여 없이(FILE_ATTRIBUTE_NORMAL) 생성
	HANDLE		hFile = CreateFile(binaryPath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	// 파일이 존재한다면
	if (INVALID_HANDLE_VALUE != hFile)
	{
		// nullptr 반환
		CloseHandle(hFile);
		return nullptr;
	}

	loader.LoadFbx(path);

	shared_ptr<MeshData> meshData = make_shared<MeshData>();

	for (int32 i = 0; i < loader.GetMeshCount(); i++)
	{
		shared_ptr<Mesh> mesh = Mesh::CreateFromFBX(&loader.GetMesh(i), loader);

		GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);

		// Material 찾아서 연동
		vector<shared_ptr<Material>> materials;
		for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++)
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(loader.GetMesh(i).materials[j].name);
			materials.push_back(material);
		}

		MeshRenderInfo info = {};
		info.mesh = mesh;
		info.materials = materials;
		meshData->m_meshRenders.push_back(info);
	}

	return meshData;
}

void MeshData::Load(const wstring& _strFilePath)
{
	FBXLoader loader;
	loader.makeBinaryFilePath(_strFilePath);
	wstring binaryPath = loader.GetBinaryFilePath();

	// .bin 파일 존재여부 확인
	HANDLE hFile = CreateFileRead(binaryPath);

	// 파일이 존재하지 않는다면
	if (INVALID_HANDLE_VALUE == hFile)
	{
		wstring str{ L"MeshData::Load - Failed to CreateFile From : " };
		str += _strFilePath;
		MSG_BOX(str.c_str());
		return;
	}

	// 파일이 존재한다면, 열린 파일로 부터 Fbx를 로드
	loader.LoadFbxFromBinary(hFile);

	// 4. 생성된 meshData의 vector<MeshRenderInfo> _meshRenders의 정보 중 shared_ptr<Mesh> mesh의 정보 저장
	uint32 num = loadInt(hFile);

	m_meshRenders.clear();
	m_meshRenders.resize(num);

	uint32 count = 0;

	for (auto& meshRenderer : m_meshRenders)
	{
		meshRenderer.mesh = std::make_shared<Mesh>();
		meshRenderer.mesh->LoadDataBinary(hFile, &loader.GetMesh(count));

		GET_SINGLE(Resources)->Add<Mesh>(meshRenderer.mesh->GetName(), meshRenderer.mesh);

		// Material 찾아서 연동
		vector<shared_ptr<Material>> materials;
		for (size_t j = 0; j < loader.GetMesh(count).materials.size(); j++)
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(loader.GetMesh(count).materials[j].name);
			materials.push_back(material);
		}

		meshRenderer.materials = materials;

		++count;
	}

	// 읽기용으로 생성한 파일을 닫는다.
	CloseHandle(hFile);
}

void MeshData::Save(const wstring& _strFilePath)
{
	// 추출된 데이터 저장
	/*
		1. FBXLoader 생성
		2. 저장용 파일 생성
		3. FBXLoader 정보 저장
		4. 생성된 meshData의 vector<MeshRenderInfo> _meshRenders의 정보 중 shared_ptr<Mesh> mesh의 정보 저장
		5. 파일 닫기
	*/
	FBXLoader loader;

	// Binary 파일 경로 생성
	loader.makeBinaryFilePath(_strFilePath);
	wstring binaryPath = loader.GetBinaryFilePath();

	// 1. FBXLoader 생성
	loader.LoadFbx(_strFilePath);

	// 2. 저장용 파일 생성
	HANDLE hFile = CreateFileWrite(binaryPath.c_str());

	// 파일이 잘 생성되었는지 검사
	if (INVALID_HANDLE_VALUE == hFile)
	{
		wstring str{ L"MeshData::Save - Failed to CreateFile From : " };
		str += _strFilePath;
		MSG_BOX(str.c_str());
		return;
	}

	// 3. FBXLoader 정보 저장
	loader.SaveFbxToBinary(hFile);

	// 4. 생성된 meshData의 vector<MeshRenderInfo> _meshRenders의 정보 중 shared_ptr<Mesh> mesh의 정보 저장
	saveInt(hFile, m_meshRenders.size());
	for (auto& meshRenderer : m_meshRenders)
	{
		meshRenderer.mesh->SaveDataBinary(hFile);
	}

	// 5. 파일 닫기
	CloseHandle(hFile);
}

vector<shared_ptr<CGameObject>> MeshData::Instantiate()
{
	vector<shared_ptr<CGameObject>> v;

	for (MeshRenderInfo& info : m_meshRenders)
	{
		shared_ptr<CGameObject> gameObject = make_shared<CGameObject>();
		gameObject->AddComponent(make_shared<Transform>());
		gameObject->AddComponent(make_shared<MeshRenderer>());
		gameObject->GetMeshRenderer()->SetMesh(info.mesh);

		for (uint32 i = 0; i < info.materials.size(); i++)
			gameObject->GetMeshRenderer()->SetMaterial(info.materials[i], i);

		if (info.mesh->IsAnimMesh())
		{
			shared_ptr<Animator> animator = make_shared<Animator>();
			gameObject->AddComponent(animator);
			animator->SetBones(info.mesh->GetBones());
			animator->SetAnimClip(info.mesh->GetAnimClip());
		}

		v.push_back(gameObject);
	}


	return v;
}

HANDLE MeshData::CreateFileWrite(const wstring& path)
{
	// CreateFile : 쓰기 파일(GENERIC_WRITE), 존재하더라도 덮어쓰기(CREATE_ALWAYS), 전용 역할 부여 없이(FILE_ATTRIBUTE_NORMAL) 생성
	return CreateFile(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

HANDLE MeshData::CreateFileRead(const wstring& path)
{
	// CreateFile : 읽기 파일(GENERIC_READ), 존재하면 열기(OPEN_EXISTING), 전용 역할 부여 없이(FILE_ATTRIBUTE_NORMAL) 생성
	return CreateFile(path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}
