#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

CMeshRenderer::CMeshRenderer() : CComponent(COMPONENT_TYPE::MESH_RENDERER)
{

}

CMeshRenderer::~CMeshRenderer()
{

}

void CMeshRenderer::Render()
{
	GetTransform()->PushData();

	m_material->PushData();
	m_mesh->Render();
}