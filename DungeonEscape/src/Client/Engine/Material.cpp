#include "pch.h"
#include "Material.h"
#include "Engine.h"

void CMaterial::Update()
{
	// CBV 업로드
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushData(&m_params, sizeof(m_params));

	// SRV 업로드
	for (size_t i = 0; i < m_textures.size(); i++)
	{
		if (m_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		g_Engine->GetTableDescHeap()->SetSRV(m_textures[i]->GetCpuHandle(), reg);
	}

	// 파이프라인 세팅
	m_shader->Update();
}
