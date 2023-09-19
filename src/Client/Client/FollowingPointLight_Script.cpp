﻿#include "pch.h"
#include "FollowingPointLight_Script.h"

#include "GameObject.h"
#include "SceneManager.h"
#include "Transform.h"
#include "Scene.h"

#include <iostream>

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

	vPos.y += 200.f;

	GetTransform()->SetLocalPosition(vPos);

	//cout << vPos.x << "  " << vPos.y << "  " << vPos.z << endl;
}

void FollowingPointLight_Script::LateUpdate()
{
}
