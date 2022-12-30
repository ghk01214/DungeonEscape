#include "pch.h"
#include "Game.h"
#include "Engine.h"

std::shared_ptr<CMesh> mesh = std::make_shared<CMesh>();
std::shared_ptr<CShader> shader = std::make_shared<CShader>();

void CGame::Init(const WindowInfo& Info)
{
	g_Engine->Init(Info);

	std::vector<Vertex> vec{ 3 };
	vec[0].pos = Vec3(0.f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[1].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.0f, 0.f, 1.f);
	vec[2].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);

	mesh->Init(vec);

	shader->Init(L"..\\Resources\\Shader\\default.hlsli");

	g_Engine->GetCmdQueue()->WaitSync();

}

void CGame::Update()
{
	//g_Engine->Render();

	g_Engine->RenderBegin();

	shader->Update();
	mesh->Render();

	g_Engine->RenderEnd();
}
