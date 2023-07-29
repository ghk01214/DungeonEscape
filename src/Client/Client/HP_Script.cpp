#include "pch.h"
#include "HP_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <Transform.h>
#include <Timer.h>
#include <Engine.h>

HP_Script::HP_Script()
{
}

HP_Script::~HP_Script()
{
}

void HP_Script::Awake()
{
	__super::Awake();
}

void HP_Script::Start()
{
	__super::Start();
}

void HP_Script::Update()
{
	__super::Update();

	// 한 쪽 방향으로 수축하는 바 작성 법
	// 델타 타임 대신 줄이고 싶은 비율대로 줄이기
	/*Vec3 pos{ GetTransform()->GetLocalPosition() };
	Vec3 scale{ GetTransform()->GetLocalScale() };

	pos.x -= DELTA_TIME / 2.f;
	scale.x -= DELTA_TIME;

	for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
	{
		GetTransform()->SetLocalPosition(pos);
		GetTransform()->SetLocalScale(scale);
	}*/
}

void HP_Script::LateUpdate()
{
	__super::LateUpdate();
}