#include "pch.h"
#include "Material.h"
#include "Engine.h"

Material::Material() : Object(OBJECT_TYPE::MATERIAL)
{

}

Material::~Material()
{

}

void Material::PushGraphicsData()
{
	// CBV 업로드
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&m_params, sizeof(m_params));

	// SRV 업로드
	for (size_t i = 0; i < m_textures.size(); i++)
	{
		if (m_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		GEngine->GetGraphicsDescHeap()->SetSRV(m_textures[i]->GetSRVHandle(), reg);
	}

	// 파이프라인 세팅
	m_shader->Update();
}

void Material::PushComputeData()
{
	// CBV 업로드
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushComputeData(&m_params, sizeof(m_params));

	// SRV 업로드
	for (size_t i = 0; i < m_textures.size(); i++)
	{
		if (m_textures[i] == nullptr)
			continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		GEngine->GetComputeDescHeap()->SetSRV(m_textures[i]->GetSRVHandle(), reg);
	}

	// 파이프라인 세팅
	m_shader->Update();
}

void Material::Dispatch(uint32 x, uint32 y, uint32 z)
{
	// CBV + SRV + SetPipelineState
	PushComputeData();

	// SetDescriptorHeaps + SetComputeRootDescriptorTable
	GEngine->GetComputeDescHeap()->CommitTable();

	COMPUTE_CMD_LIST->Dispatch(x, y, z);

	GEngine->GetComputeCmdQueue()->FlushComputeCommandQueue();
}

shared_ptr<Material> Material::Clone()
{
	shared_ptr<Material> material = make_shared<Material>();

	material->SetShader(m_shader);
	material->m_params = m_params;
	material->m_textures = m_textures;

	return material;
}