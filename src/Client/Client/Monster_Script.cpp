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

#include <Network.h>

void Monster_Dragon::Update(void)
{
	if (m_gameObject.lock()->GetClientID() == network::pNetwork->GetID())
	{
		if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex + 1) % count;
			//GetAnimator()->Play(index);

			network::pNetwork->SendAniIndexPacket(index, GetAnimator()->GetUpdateTime());
		}

		if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
		{
			int32 count = GetAnimator()->GetAnimCount();
			int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

			int32 index = (currentIndex - 1 + count) % count;
			//GetAnimator()->Play(index);

			network::pNetwork->SendAniIndexPacket(index, GetAnimator()->GetUpdateTime());
		}


		if (INPUT->GetButton(KEY_TYPE::LEFT))
		{
			auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };
			auto id{ network::pNetwork->GetID() };
			auto pos{ player[id]->GetTransform()->GetLocalPosition() };

			network::pNetwork->SendMovePacket(DIRECTION::LEFT);
		}

		if (INPUT->GetButton(KEY_TYPE::RIGHT))
		{
			auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };
			auto id{ network::pNetwork->GetID() };
			auto pos{ player[id]->GetTransform()->GetLocalPosition() };

			network::pNetwork->SendMovePacket(DIRECTION::RIGHT);
		}

		if (INPUT->GetButton(KEY_TYPE::UP))
		{
			auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };
			auto id{ network::pNetwork->GetID() };
			auto pos{ player[id]->GetTransform()->GetLocalPosition() };

			network::pNetwork->SendMovePacket(DIRECTION::UP);
		}

		if (INPUT->GetButton(KEY_TYPE::DOWN))
		{
			auto player{ GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer() };
			auto id{ network::pNetwork->GetID() };
			auto pos{ player[id]->GetTransform()->GetLocalPosition() };

			network::pNetwork->SendMovePacket(DIRECTION::DOWN);
		}
	}
}
