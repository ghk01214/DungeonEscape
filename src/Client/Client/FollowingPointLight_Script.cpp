#include "pch.h"
#include "FollowingPointLight_Script.h"

#include "GameObject.h"
#include "SceneManager.h"
#include "Transform.h"
#include "Scene.h"

#include <iostream>

#include "Input.h"
#include "Timer.h"

FollowingPointLight_Script::FollowingPointLight_Script()
{
}

FollowingPointLight_Script::~FollowingPointLight_Script()
{
}

void FollowingPointLight_Script::Start()
{
}

void FollowingPointLight_Script::Update()
{
	auto player = (*GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer().begin());

	Vec3 vPos = player->GetTransform()->GetWorldMatrix().Translation();

	//static float length2 = 500.f;

	//if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUMPAD_1))
	//{
	//	length2 -= DELTA_TIME * 2000.f;
	//}
	//if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUMPAD_2))
	//{
	//	length2 += DELTA_TIME * 2000.f;
	//}

	//vPos.y += length2;

	GetTransform()->SetLocalPosition(vPos);

	//cout << vPos.x << "  " << vPos.y << "  " << vPos.z << endl;
}

void FollowingPointLight_Script::LateUpdate()
{
}
