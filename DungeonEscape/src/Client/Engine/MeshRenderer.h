#pragma once
#include "Component.h"

class CMesh;
class CMaterial;

class CMeshRenderer : public CComponent
{
public:
	CMeshRenderer();
	virtual ~CMeshRenderer();

	void SetMesh(std::shared_ptr<CMesh> mesh) { m_mesh = mesh; }
	void SetMaterial(std::shared_ptr<CMaterial> material) { m_material = material; }

	virtual void Update() override { Render(); }

	void Render();

private:
	std::shared_ptr<CMesh> m_mesh;
	std::shared_ptr<CMaterial> m_material;
};

