﻿#include "pch.h"
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

void Monster_Dragon::Update(void)
{
	if (GET_NETWORK->GetID() == GetNetwork()->GetID())
	{
		if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex + 1) % count;

			GetNetwork()->SendAniIndexPacket(index, GetAnimator()->GetUpdateTime());
		}

		if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex - 1 + count) % count;

			GetNetwork()->SendAniIndexPacket(index, GetAnimator()->GetUpdateTime());
		}


		if (INPUT->GetButton(KEY_TYPE::LEFT))
		{
			GetNetwork()->SendMovePacket(DIRECTION::LEFT);
		}

		if (INPUT->GetButton(KEY_TYPE::RIGHT))
		{
			GetNetwork()->SendMovePacket(DIRECTION::RIGHT);
		}

		if (INPUT->GetButton(KEY_TYPE::UP))
		{
			GetNetwork()->SendMovePacket(DIRECTION::UP);
		}

		if (INPUT->GetButton(KEY_TYPE::DOWN))
		{
			GetNetwork()->SendMovePacket(DIRECTION::DOWN);
		}

		if (INPUT->GetButton(KEY_TYPE::SPACE))
		{
			auto id{ GetNetwork()->GetID() };
			auto pos{ GET_PLAYER[id]->GetTransform()->GetLocalPosition() };

			GetNetwork()->SendJumpPacket();
		}
	}
}
