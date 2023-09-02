#include "pch.h"
#include "Movement_Script.h"

#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "MeshData.h"

Movement_Script::Movement_Script() :
	m_curSenematic{ 4 }
{
}

Movement_Script::~Movement_Script()
{
}

void Movement_Script::Awake()
{
}

void Movement_Script::Start()
{
	m_vPosition = Vec3(6000.f, 1000.f, 20000.f);
}

void Movement_Script::Update()
{
	auto transform = GetTransform();

	Matrix matRotation = Matrix::CreateRotationX(XMConvertToRadians(m_vRotation.x));
	matRotation *= Matrix::CreateRotationY(XMConvertToRadians(m_vRotation.y));
	matRotation *= Matrix::CreateRotationZ(XMConvertToRadians(m_vRotation.z));

	// 공전
	Matrix matWorld = matRotation * Matrix::CreateTranslation(m_vPosition);
	transform->SetWorldMatrix(matWorld);

	// X축 회전
	{
		long MouseMoveY = GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_Y);
		m_vRotation.x += MouseMoveY * DELTA_TIME * 5.f;
	}
	// Y축 회전
	{
		long MouseMoveX = GET_SINGLE(CInput)->Get_DIMMoveState(CInput::DIMM_X);
		m_vRotation.y += MouseMoveX * DELTA_TIME * 5.f;
	}

	Vec3 movement{};
	matWorld = transform->GetWorldMatrix();

	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::W))
	{
		movement += matWorld.Backward() * m_speed * DELTA_TIME;
	}
	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::S))
	{
		movement += matWorld.Forward() * m_speed * DELTA_TIME;
	}
	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::A))
	{
		movement += matWorld.Left() * m_speed * DELTA_TIME;
	}
	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::D))
	{
		movement += matWorld.Right() * m_speed * DELTA_TIME;
	}

	m_vPosition = transform->GetWorldPosition() + movement;
	matWorld.Translation(m_vPosition);
	transform->SetWorldMatrix(matWorld);
}

void Movement_Script::LateUpdate()
{
	// 현재 행렬 푸쉬
	if (GET_SINGLE(CInput)->GetButtonDown(KEY_TYPE::NUM_1))
	{
		m_vecMat.push_back(GetTransform()->GetWorldMatrix());
	}

	// 저장된 행렬 정보 파일 쓰기
	if (GET_SINGLE(CInput)->GetButtonDown(KEY_TYPE::NUM_2))
	{
		wstring path = L"..\\Resources\\Cinematic\\Info";
		wstring str = to_wstring(m_curSenematic) + L".bin";

		HANDLE hFile = MeshData::CreateFileWrite(path + str);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			wstring str{ L"Movement_Script::LateUpdate - Failed to CreateFile From : " };
			str += path;
			MSG_BOX(str.c_str());
			return;
		}

		saveInt(hFile, m_vecMat.size());

		saveVecData(hFile, m_vecMat);

		CloseHandle(hFile);
	}

	// 파일에 작성된 행렬 정보 로드
	if (GET_SINGLE(CInput)->GetButtonDown(KEY_TYPE::NUM_3))
	{
		wstring path = L"..\\Resources\\Senematic\\Info";
		wstring str = to_wstring(m_curSenematic) + L".bin";

		HANDLE hFile = MeshData::CreateFileRead(path + str);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			wstring str{ L"Movement_Script::LateUpdate - Failed to CreateFile From : " };
			str += path;
			MSG_BOX(str.c_str());
			return;
		}

		int iCount = loadInt(hFile);
		m_vecMat.reserve(iCount);

		m_vecMat = loadVecData<Matrix>(hFile);
	}
}
