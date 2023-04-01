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
			auto player{ GET_SCENE->GetPlayer() };
			auto id{ GetNetwork()->GetID() };
			auto pos{ player[id]->GetTransform()->GetLocalPosition() };

			GetNetwork()->SendMovePacket(DIRECTION::LEFT);
		}

		if (INPUT->GetButton(KEY_TYPE::RIGHT))
		{
			auto player{ GET_SCENE->GetPlayer() };
			auto id{ GetNetwork()->GetID() };
			auto pos{ player[id]->GetTransform()->GetLocalPosition() };

			GetNetwork()->SendMovePacket(DIRECTION::RIGHT);
		}

		if (INPUT->GetButton(KEY_TYPE::UP))
		{
			auto player{ GET_SCENE->GetPlayer() };
			auto id{ GetNetwork()->GetID() };
			auto pos{ player[id]->GetTransform()->GetLocalPosition() };

			GetNetwork()->SendMovePacket(DIRECTION::UP);
		}

		if (INPUT->GetButton(KEY_TYPE::DOWN))
		{
			auto player{ GET_SCENE->GetPlayer() };
			auto id{ GetNetwork()->GetID() };
			auto pos{ player[id]->GetTransform()->GetLocalPosition() };

			GetNetwork()->SendMovePacket(DIRECTION::DOWN);
		}
	}
}
