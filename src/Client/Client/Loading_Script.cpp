#include "pch.h"
#include "Loader.h"
#include "Loading_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <SceneManager.h>

#include "Scene_Loading.h"

Loading_Script::Loading_Script()
{
}

Loading_Script::~Loading_Script()
{
}

void Loading_Script::Awake()
{
	__super::Awake();
}

void Loading_Script::Start()
{
	__super::Start();
}

void Loading_Script::Update()
{
	__super::Update();

	//auto loader{ std::dynamic_pointer_cast<Scene_Loading>(GET_SCENE)->GetLoader() };

	//for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
	//{
	//	GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[loader->GetProgress() / 10]);
	//}
}

void Loading_Script::LateUpdate()
{
	__super::LateUpdate();
}
