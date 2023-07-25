#include "pch.h"
#include "Monster_Script.h"

#include "Animator.h"
#include "Input.h"

#include "Transform.h"
#include "Timer.h"

#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>

#include <NetworkManager.h>
#include <Network.h>

Monster_Script::Monster_Script()
{
}

Monster_Script::~Monster_Script()
{
}

void Monster_Script::Start()
{
	m_aniEnd = false;

	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld *= Matrix::CreateScale(2.5f);
	GetTransform()->SetWorldMatrix(matWorld);
}

void Monster_Script::Update()
{
#pragma region [FOR DEBUGGING]
	/*if (GET_NETWORK->GetID() == GetNetwork()->GetID())
	{
		if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex + 1) % count;

			GetAnimator()->Play(index);
		}

		if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
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

	//Vec3 rot;
	//rot.x = packet.Read<float>();
	//rot.y = packet.Read<float>();
	//rot.z = packet.Read<float>();
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

	pos.y -= m_halfHeight;

	Matrix matWorld{ Matrix::CreateScale(scale / scaleRatio) };
	if (quat.y == 0.f)
	{
		matWorld = GetTransform()->GetWorldMatrix();
		matWorld.Translation(pos);
	}
	else
	{
		matWorld *= Matrix::CreateFromQuaternion(Quat{ 0.f, quat.y, 0.f, quat.w });
		matWorld *= Matrix::CreateTranslation(pos);
	}

	GetTransform()->SetWorldMatrix(matWorld);

#pragma region [FOR DEBUGGING]
	//auto t{ GetTransform()->GetWorldPosition() };
	//std::cout << std::format("monster id - {}, t : {}, {}, {}", id, t.x, t.y, t.z) << std::endl;
#pragma endregion
}
