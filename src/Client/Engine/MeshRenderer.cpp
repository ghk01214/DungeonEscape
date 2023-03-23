#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "InstancingBuffer.h"
#include "Resources.h"
#include "Animator.h"

MeshRenderer::MeshRenderer() : Component(COMPONENT_TYPE::MESH_RENDERER)
{

}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::SetMaterial(shared_ptr<Material> material, uint32 idx)
{
	if (m_materials.size() <= static_cast<size_t>(idx))
		m_materials.resize(static_cast<size_t>(idx + 1));

	m_materials[idx] = material;
}

void MeshRenderer::Render()
{
	for (uint32 i = 0; i < m_materials.size(); i++)
	{
		shared_ptr<Material>& material = m_materials[i];

		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		GetTransform()->PushData();

		if (GetAnimator())
		{
			GetAnimator()->PushData();
			material->SetInt(1, 1);
		}
		else
		{
			material->SetInt(1, 0);
		}

		material->PushGraphicsData();
		m_mesh->Render(1, i);
	}
}

void MeshRenderer::Render(shared_ptr<InstancingBuffer>& buffer)
{
	for (uint32 i = 0; i < m_materials.size(); i++)
	{
		shared_ptr<Material>& material = m_materials[i];

		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		buffer->PushData();

		if (GetAnimator())
		{
			GetAnimator()->PushData();
			material->SetInt(1, 1);
		}
		else
		{
			material->SetInt(1, 0);
		}

		material->PushGraphicsData();
		m_mesh->Render(buffer, i);
	}
}

void MeshRenderer::RenderShadow()
{
	GetTransform()->PushData();
	GET_SINGLE(Resources)->Get<Material>(L"Shadow")->PushGraphicsData();
	m_mesh->Render();
}

uint64 MeshRenderer::GetInstanceID()
{
	if (m_mesh == nullptr || m_materials.empty())
		return 0;

	//uint64 id = (m_mesh->GetID() << 32) | _material->GetID();
	InstanceID instanceID{ m_mesh->GetID(), m_materials[0]->GetID() };
	return instanceID.id;
}