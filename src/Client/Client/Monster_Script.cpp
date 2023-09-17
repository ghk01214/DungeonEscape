﻿#include "pch.h"
#include "Monster_Script.h"

#include <Animator.h>
#include <Input.h>
#include <Transform.h>
#include <Timer.h>
#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>
#include <NetworkManager.h>
#include <Network.h>
#include <EffectManager.h>

#include "BossCounterEffect_Script.h"

Monster_Script::Monster_Script() :
	m_aniEnd{ false }
{
}

Monster_Script::~Monster_Script()
{
}

void Monster_Script::Start()
{
	m_aniEnd = false;

	//Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	//matWorld *= Matrix::CreateScale(2.5f);
	//GetTransform()->SetWorldMatrix(matWorld);
}

void Monster_Script::Update()
{
#pragma region [FOR DEBUGGING]
	/*if (GET_NETWORK->GetID() == GetNetwork()->GetID())
	{
		if (INPUT->GetButtonDown(KEY_TYPE::NUM_1))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex + 1) % count;

			GetAnimator()->Play(index);
		}

		if (INPUT->GetButtonDown(KEY_TYPE::NUM_2))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex - 1 + count) % count;

			GetAnimator()->Play(index);
		}
	}*/
#pragma endregion
}

void Monster_Script::LateUpdate()
{
}

void Monster_Script::Transform(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	Quat quat;
	quat.x = packet.Read<float>();
	quat.y = packet.Read<float>();
	quat.z = packet.Read<float>();
	quat.w = packet.Read<float>();

	Vec3 scale;
	scale.x = packet.Read<float>();
	scale.y = packet.Read<float>();
	scale.z = packet.Read<float>();

	float scaleRatio{ packet.Read<float>() };

	quat.x = 0.f;
	quat.z = 0.f;

	pos.y -= m_halfHeight;

	Matrix matWorld{ Matrix::CreateScale(scale / scaleRatio) };

	if (quat.y == 0.f)
	{
		matWorld = GetTransform()->GetWorldMatrix();
		matWorld.Translation(pos);
	}
	else
	{
		matWorld *= Matrix::CreateFromQuaternion(quat);
		matWorld *= Matrix::CreateTranslation(pos);
	}

	GetTransform()->SetWorldMatrix(matWorld);

#pragma region [FOR DEBUGGING]
	//auto t{ GetTransform()->GetWorldPosition() };
	//std::cout << std::format("monster id - {}, t : {}, {}, {}", id, t.x, t.y, t.z) << std::endl;
#pragma endregion
}

void Monster_Script::Rotate(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	Quat quat;
	quat.x = 0.f;
	quat.y = packet.Read<float>();
	quat.z = 0.f;
	quat.w = packet.Read<float>();

	auto pos{ GetTransform()->GetWorldMatrix().Translation() };
	auto scale{ GetTransform()->GetScale() };

	Matrix matWorld{ Matrix::CreateScale(scale) };
	matWorld *= Matrix::CreateFromQuaternion(quat);
	matWorld *= Matrix::CreateTranslation(pos);
	GetTransform()->SetWorldMatrix(matWorld);
}

void Monster_Script::CounterEffect(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	bool render{ packet.Read<bool>() };

	if (render == true)
	{
		auto pos{ GetTransform()->GetWorldPosition() };
		pos.y += 10.f;
		m_counterEffectScript->SetPos(pos);
		m_counterEffectScript->FadeIn(0.5f);
	}
	else
	{
		m_counterEffectScript->FadeOut(0.5f);
	}
}

void Monster_Script::SetCounterEffectScript(std::shared_ptr<BossCounterEffect_Script> script)
{
	m_counterEffectScript = script;
}
