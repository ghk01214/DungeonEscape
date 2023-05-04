#include "pch.h"
#include "Player_Script.h"

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

void Player_Mistic::Update(void)
{
	int32 count = GetAnimator()->GetAnimCount();
	int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
	int32 index = 0;

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
	{
		index = (currentIndex + 1) % count;


		GetNetwork()->SendAniIndexPacket(index, GetAnimator()->GetUpdateTime());
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		index = (currentIndex - 1 + count) % count;

		GetNetwork()->SendAniIndexPacket(index, GetAnimator()->GetUpdateTime());
		GetAnimator()->Play(index);
	}


	uint8_t input{};
	server::KEY_STATE state{ server::KEY_STATE::NONE };

	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		input |= static_cast<uint8_t>(server::KEY_TYPE::LEFT);
		state = server::KEY_STATE::PRESS;
	}

	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		input |= static_cast<uint8_t>(server::KEY_TYPE::LEFT);
		state = server::KEY_STATE::PRESS;
	}

	if (INPUT->GetButton(KEY_TYPE::UP))
	{
		input |= static_cast<uint8_t>(server::KEY_TYPE::LEFT);
		state = server::KEY_STATE::PRESS;
	}

	if (INPUT->GetButton(KEY_TYPE::DOWN))
	{
		input |= static_cast<uint8_t>(server::KEY_TYPE::LEFT);
		state = server::KEY_STATE::PRESS;
	}

	if (INPUT->GetButton(KEY_TYPE::SPACE))
	{
		input |= static_cast<uint8_t>(server::KEY_TYPE::LEFT);
		state = server::KEY_STATE::PRESS;
	}
}
