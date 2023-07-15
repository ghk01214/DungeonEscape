#include "pch.h"
#include "BB_Fire.h"
#include "Resources.h"

BB_Fire::BB_Fire()
{
	// 텍스쳐 설정
	vector<shared_ptr<Texture>> textures = GET_SINGLE(Resources)->LoadTextures(L"Effect_Fire", L"..\\Resources\\Texture\\Effect\\Sprite\\Fire.png", 64);

	SetBBInfo(BB_TYPE::ATLAS, textures, 0.0001f);
}

BB_Fire::~BB_Fire()
{
}

void BB_Fire::Awake()
{
	__super::Awake();
}

void BB_Fire::Start()
{
	__super::Start();
}

void BB_Fire::Update()
{
	__super::Update();
}

void BB_Fire::LateUpdate()
{
	__super::LateUpdate();
}

void BB_Fire::FinalUpdate()
{
	__super::FinalUpdate();
}
