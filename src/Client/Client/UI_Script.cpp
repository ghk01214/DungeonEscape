#include "pch.h"
#include "UI_Script.h"

#include "Timer.h"

#include "MeshRenderer.h"
#include "Material.h"

UI_Script::UI_Script() :
	m_curTextureCount{ 0 }
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
	uint32_t size = GetMeshRenderer()->GetMaterialSize();

	for (uint32_t i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetFloat(2, 1.f);
	}
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
