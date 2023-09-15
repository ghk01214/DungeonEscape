#include "pch.h"
#include "BossTutorial_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <UI.h>

BossTutorial_Script::BossTutorial_Script()
{
}

BossTutorial_Script::~BossTutorial_Script()
{
}

void BossTutorial_Script::Awake()
{
	__super::Awake();
}

void BossTutorial_Script::Start()
{
	__super::Start();

	GetUI()->SetVisible(false);
}

void BossTutorial_Script::Update()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::Update();


}

void BossTutorial_Script::LateUpdate()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::LateUpdate();
}

void BossTutorial_Script::ChangePage(int32_t index)
{
	uint32_t size{ GetMeshRenderer()->GetMaterialSize() };
	for (int32_t i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[index]);
	}
}

void BossTutorial_Script::SetVisible(bool flag)
{
	GetUI()->SetVisible(flag);
}
