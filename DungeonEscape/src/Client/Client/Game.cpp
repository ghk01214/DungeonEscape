#include "pch.h"
#include "Game.h"
#include "Engine.h"

std::shared_ptr<CMesh> mesh = std::make_shared<CMesh>();
std::shared_ptr<CShader> shader = std::make_shared<CShader>();
std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();

void CGame::Init(const WindowInfo& Info)
{
	g_Engine->Init(Info);

	// 사각형 정점 4개(버텍스 버퍼) 생성, 위치 / 색상 / uv값 세팅
	std::vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[0].uv = Vec2(0.f, 0.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[1].uv = Vec2(1.f, 0.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[2].uv = Vec2(1.f, 1.f);
	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[3].uv = Vec2(0.f, 1.f);

	// 인덱스 버퍼 생성
	std::vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}

	mesh->Init(vec, indexVec);

	shader->Init(L"..\\Resources\\Shader\\default.hlsli");
	texture->Init(L"..\\Resources\\Texture\\blue_archive_celebration.png");

	g_Engine->GetCmdQueue()->WaitSync();
}

void CGame::Update()
{
	//g_Engine->Render();

	g_Engine->RenderBegin();

	shader->Update();

	{
		Transform t;
		t.offset = Vec4(0.25f, 0.25f, 0.1f, 0.f);
		mesh->SetTransform(t);

		mesh->SetTexture(texture);

		mesh->Render();
	}

	{
		Transform t;
		t.offset = Vec4(0.f, 0.f, 0.f, 0.f);
		mesh->SetTransform(t);

		mesh->SetTexture(texture);

		mesh->Render();
	}

	g_Engine->RenderEnd();
}
