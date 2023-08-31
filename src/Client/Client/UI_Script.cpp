#include "pch.h"
#include "UI_Script.h"

#include <Timer.h>
#include <MeshRenderer.h>
#include <Material.h>

UI_Script::UI_Script() :
	m_curTextureCount{ 0 },
	m_alpha{ 1.f },
	m_active{ false }
{
}

UI_Script::~UI_Script()
{
}

void UI_Script::Awake()
{
}

void UI_Script::Start()
{
}

void UI_Script::Update()
{
}

void UI_Script::LateUpdate()
{
}

void UI_Script::InsertTextures(std::shared_ptr<Texture> texture)
{
	m_textures.push_back(texture);
}

void UI_Script::ChangeObjectAlpha()
{
	for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetFloat(2, m_alpha);
	}
}

void UI_Script::SetActive(bool flag)
{
	m_active = flag;
}
