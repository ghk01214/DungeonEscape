#include "pch.h"
#include "Creator.h"

#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <UI.h>
#include "Resources.h"

shared_ptr<CGameObject> Creator::CreateUIObject(shared_ptr<Texture> texture, shared_ptr<Shader> shader)
{
	std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();

	obj->AddComponent(std::make_shared<Transform>());

	obj->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 1.f));

	std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
	{
		std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}
	{
		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		material->SetFloat(2, 1.f);
		meshRenderer->SetMaterial(material);
	}
	obj->AddComponent(meshRenderer);

	std::shared_ptr<UI> ui = std::make_shared<UI>();
	{
		ui->SetVisible(true);
		ui->SetPopUp(false);
		//ui->OnTop();
	}
	obj->AddComponent(ui);

	return obj;
}
