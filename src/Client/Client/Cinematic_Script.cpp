#include "pch.h"
#include "Cinematic_Script.h"

#include "Transform.h"
#include "Timer.h"
#include "Camera_Script.h"

#include "MeshData.h"

Cinematic_Script::Cinematic_Script()
{
}

Cinematic_Script::~Cinematic_Script()
{
}

void Cinematic_Script::Awake()
{
	// 파일 정보 로드
	for (int i = 1; i < static_cast<int>(server::CUT_SCENE_TYPE::MAX); ++i)
	{
		wstring path = L"..\\Resources\\Cinematic\\Info";
		wstring str = to_wstring(i) + L".bin";

		HANDLE hFile = MeshData::CreateFileRead(path + str);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			//wstring str{ L"Movement_Script::LateUpdate - Failed to CreateFile From : " };
			//str += path;
			//MSG_BOX(str.c_str());
			return;
		}

		vector<Matrix> vecMat;

		int iCount = loadInt(hFile);
		vecMat.reserve(iCount);

		vecMat = loadVecData<Matrix>(hFile);

		for (int j = 0; j < iCount; ++j)
		{
			CINEMATIC_INFO info;
			info.m_time = 3.f;
			info.matWorld = vecMat[j];

			m_info[i].push_back(info);
		}
	}
}

void Cinematic_Script::Start()
{

}

void Cinematic_Script::Update()
{
	if (m_bPlay)
	{
		if (m_info[m_currentScene].empty())
			return;

		// 카메라를 현재 인덱스의 정보로 이동
		auto transform = GetTransform();
		auto& curInfo = m_info[m_currentScene][m_curIndex];

		// 공전
		Matrix matWorld = curInfo.matWorld;
		transform->SetWorldMatrix(matWorld);

		m_accTime += DELTA_TIME;

		if (m_accTime > m_info[m_currentScene][m_curIndex].m_time)
		{
			++m_curIndex;
			m_accTime = 0.f;
		}
	}
}

void Cinematic_Script::LateUpdate()
{
	if (m_bPlay)
	{
		if (m_info[m_currentScene].empty())
			return;

		if (m_info[m_currentScene].size() <= m_curIndex)
		{
			StopCinematic();
		}
	}
}

void Cinematic_Script::PlayCinematic(int iCount)
{
	m_currentScene = iCount;
	m_bPlay = true;
	m_curIndex = 0;

	m_cameraBasicScript->PlayCinematic();
}

void Cinematic_Script::StopCinematic(void)
{
	m_bPlay = false;
	m_curIndex = 0;
	m_accTime = 0.f;

	m_cameraBasicScript->StopCinematic();
}

void Cinematic_Script::SetData(server::CUT_SCENE_TYPE eType, const vector<CINEMATIC_INFO>& info)
{
	m_info[static_cast<int>(eType)] = info;
}