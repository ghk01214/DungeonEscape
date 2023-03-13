#include "pch.h"
#include "Monster_Script.h"

#include "Animator.h"
#include "Input.h"

#include "Transform.h"
#include "Timer.h"

void Monster_Dragon::Update(void)
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


	if (INPUT->GetButton(KEY_TYPE::LEFT))
	{
		Vec3 Pos = GetTransform()->GetWorldPosition();
		Pos.x += DELTA_TIME * 200.f;

		GetTransform()->SetLocalPosition(Pos);
	}

	if (INPUT->GetButton(KEY_TYPE::RIGHT))
	{
		Vec3 Pos = GetTransform()->GetWorldPosition();
		Pos.x -= DELTA_TIME * 200.f;

		GetTransform()->SetLocalPosition(Pos);
	}

	if (INPUT->GetButton(KEY_TYPE::UP))
	{
		Vec3 Pos = GetTransform()->GetWorldPosition();
		Pos.y += DELTA_TIME * 200.f;

		GetTransform()->SetLocalPosition(Pos);
	}

	if (INPUT->GetButton(KEY_TYPE::DOWN))
	{
		Vec3 Pos = GetTransform()->GetWorldPosition();
		Pos.y -= DELTA_TIME * 200.f;

		GetTransform()->SetLocalPosition(Pos);
	}
}
