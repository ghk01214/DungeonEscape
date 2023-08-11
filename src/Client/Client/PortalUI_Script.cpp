#include "pch.h"
#include "PortalUI_Script.h"

#include "FontManager.h"

PortalUI_Script::PortalUI_Script()
{
}

PortalUI_Script::~PortalUI_Script()
{
}

void PortalUI_Script::Awake()
{
}

void PortalUI_Script::Start()
{
}

void PortalUI_Script::Update()
{
	switch (m_count)
	{
		//case 0:
		//m_content = L"Player 0 / 3";
		//break;
		case 1:
		m_content = L"Player 1 / 3";
		break;
		case 2:
		m_content = L"Player 2 / 3";
		break;
		case 3:
		m_content = L"Player 3 / 3";
		break;
		default:
		break;
	}

	if (m_count != 0)
		;// GET_SINGLE(FontManager)->RenderFonts(m_content, m_pos, m_size);
}

void PortalUI_Script::LateUpdate()
{
}

void PortalUI_Script::SetInfo(Vec2 pos, Vec2 size)
{
	m_pos = pos;
	m_size = size;
}
