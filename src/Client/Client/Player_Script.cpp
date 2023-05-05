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
		//GetAnimator()->Play(index);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	{
		index = (currentIndex - 1 + count) % count;

		GetNetwork()->SendAniIndexPacket(index, GetAnimator()->GetUpdateTime());
		//GetAnimator()->Play(index);
	}
}
