#include "pch.h"
#include "Lobby_Script.h"

#include "Timer.h"

#include "MeshRenderer.h"
#include "Material.h"

Lobby_Script::Lobby_Script()
{
}

Lobby_Script::~Lobby_Script()
{
}

void Lobby_Script::Awake()
{
}

void Lobby_Script::Start()
{
	m_curTextureCount = 0;

	uint32_t size = GetMeshRenderer()->GetMaterialSize();

	for (uint32_t i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetFloat(2, 1.f);
	}
}

void Lobby_Script::Update()
{

}

void Lobby_Script::LateUpdate()
{
}

void Lobby_Script::InsertTextures(std::shared_ptr<Texture> texture)
{
	m_textures.push_back(texture);
}
