#include "pch.h"
#include "Scenematic_Script.h"

#include "Transform.h"
#include "Timer.h"
#include "Camera_Script.h"

Scenematic_Script::Scenematic_Script()
{
}

Scenematic_Script::~Scenematic_Script()
{
}

void Scenematic_Script::Awake()
{
	SCENEMATIC_INFO info;
	info.vCameraPos = Vec3(2012.3777f, -860.09766f, 6711.399f);
	info.vDegree = Vec3(182.29234f, -11.782078f, 0.f);
	info.m_time = 5.f;

	m_info[1].push_back(info);
}

void Scenematic_Script::Start()
{

}

void Scenematic_Script::Update()
{
	if (m_bPlay)
	{
		m_accTime += DELTA_TIME;

		if (m_accTime > m_info[m_currentScene][m_curIndex].m_time)
		{
			++m_curIndex;
			m_accTime = 0.f;
		}

		// 카메라를 현재 인덱스의 정보로 이동
		auto transform = GetTransform();
		auto& curInfo = m_info[m_currentScene][m_curIndex];
		auto& curPos = curInfo.vCameraPos;
		auto& curDegree = curInfo.vDegree;

		Matrix matRotation = Matrix::CreateRotationX(XMConvertToRadians(curPos.x));
		matRotation *= Matrix::CreateRotationY(XMConvertToRadians(curPos.y));
		matRotation *= Matrix::CreateRotationZ(XMConvertToRadians(curPos.z));
		Matrix matTranslation = Matrix::CreateTranslation(curPos);

		// 공전
		Matrix matWorld = matRotation * matTranslation;
		transform->SetWorldMatrix(matWorld);
	}
}

void Scenematic_Script::LateUpdate()
{
	if (m_info[m_currentScene].size() < m_curIndex)
	{
		StopSenematic();
	}
}

void Scenematic_Script::PlaySenematic(int iCount)
{
	m_currentScene = iCount;
	m_bPlay = true;
	m_curIndex = 0;

	m_cameraBasicScript->PlaySenematic();
}

void Scenematic_Script::StopSenematic(void)
{
	m_bPlay = false;
	m_curIndex = 0;
	m_accTime = 0.f;

	m_cameraBasicScript->StopSenematic();
}

void Scenematic_Script::SetData(server::CUT_SCENE_TYPE eType, const vector<SCENEMATIC_INFO>& info)
{
	m_info[static_cast<int>(eType)] = info;
}