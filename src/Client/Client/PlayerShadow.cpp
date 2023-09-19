#include "pch.h"
#include "PlayerShadow.h"

#include "GameObject.h"
#include "SceneManager.h"
#include "Transform.h"
#include "Scene.h"

#include <iostream>

#include "Input.h"
#include "Timer.h"

PlayerShadow::PlayerShadow()
{
}

PlayerShadow::~PlayerShadow()
{
}

void PlayerShadow::Start()
{
}

void PlayerShadow::Update()
{
}

void PlayerShadow::LateUpdate()
{
	auto player = (*GET_SINGLE(SceneManager)->GetActiveScene()->GetPlayer().begin());

	Vec3 vPos = GetTransform()->GetLocalToWorldMatrix().Translation();

	Matrix matWorld = GetTransform()->GetLocalToWorldMatrix();

	static float length2 = 530.f;

	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUMPAD_1))
	{
		length2 -= DELTA_TIME * 50.f;
	}
	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUMPAD_2))
	{
		length2 += DELTA_TIME * 50.f;
	}

	vPos = player->GetTransform()->GetLocalToWorldMatrix().Translation();
	vPos.y += length2;

	GetTransform()->SetLocalPosition(vPos);
	GetTransform()->SetLocalScale(Vec3{ 2.5f, 1.f, 2.5f });

	//cout << "Shadow : " << vPos.x << " " << vPos.y << " " << vPos.z << " " << endl;
	//cout << "length : " << length2 << endl;
}
