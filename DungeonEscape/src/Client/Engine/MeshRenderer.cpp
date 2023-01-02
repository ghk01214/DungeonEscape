#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"

CMeshRenderer::CMeshRenderer() : CComponent(COMPONENT_TYPE::MESH_RENDERER)
{

}

CMeshRenderer::~CMeshRenderer()
{

}

void CMeshRenderer::Render()
{
	//GetTransform()->Update();

	m_material->Update();
	m_mesh->Render();
}