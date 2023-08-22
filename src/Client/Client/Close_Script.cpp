#include "pch.h"
#include "Close_Script.h"

#include <Material.h>
#include <MeshRenderer.h>
#include <GameObject.h>
#include <UI.h>
#include <Input.h>

Close_Script::Close_Script()
{
}

Close_Script::~Close_Script()
{
}

void Close_Script::Awake()
{
	__super::Awake();
}

void Close_Script::Start()
{
	__super::Start();
}

void Close_Script::Update()
{
	__super::Update();

	if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true and m_click == false)
			{
				m_click = true;

				for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
				{
					GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[BUTTON_PRESSED]);
				}
			}
			else if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true and m_click == true)
			{
				m_click = false;

				for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
				{
					GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[BUTTON]);
				}
			}
		}
	}
}

void Close_Script::LateUpdate()
{
	__super::LateUpdate();
}
