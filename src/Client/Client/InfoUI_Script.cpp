#include "pch.h"
#include "InfoUI_Script.h"

#include "Camera_Script.h"
#include "FontManager.h"
#include "Transform.h"

#include "Engine.h"

InfoUI_Script::InfoUI_Script()
{
}

InfoUI_Script::~InfoUI_Script()
{
}

void InfoUI_Script::Awake()
{
}

void InfoUI_Script::Start()
{
}

void InfoUI_Script::Update()
{
	return;

	// 카메라 기준 위치
	m_cameraPos = m_cameraInfoScript->GetTransform()->GetLocalToWorldMatrix().Translation();
	m_distanceAxis = m_cameraInfoScript->GetDistanceAxisXY();
	m_rotationAxis = m_cameraInfoScript->GetRotationAxisXY();


	//std::shared_ptr<CScene> activeScene = GET_SINGLE(SceneManager)->GetActiveScene();
	//auto& player = activeScene->GetPlayer();

	{
		std::string strPos{ std::format("Pos : {:.2f}, {:.2f}", m_cameraPos.x, m_cameraPos.y) };
		std::wstring wstrPos{ strPos.begin(), strPos.end() };

		const WindowInfo& windowInfo = GEngine->GetWindow();

		Vec2 fontPos = Vec2(0, windowInfo.height * 0.4f);

		GET_SINGLE(FontManager)->RenderFonts(wstrPos, fontPos, Vec2(0.1f, 0.1f), 1.f);
	}
	{
		std::string strRot{ std::format("rotation - X : {:.2f}, Y : {:.2f}", m_rotationAxis.x, m_rotationAxis.y) };
		std::wstring wstrRot{ strRot.begin(), strRot.end() };

		const WindowInfo& windowInfo = GEngine->GetWindow();

		Vec2 fontPos = Vec2(0, windowInfo.height * 0.3f);
		GET_SINGLE(FontManager)->RenderFonts(wstrRot, fontPos, Vec2(0.1f, 0.1f), 1.f);
	}
	//{
	//	GET_SINGLE(FontManager)->RenderFonts(wstrPos, fontPos, Vec2(0.1f, 0.1f), 1.f);
	//}
}

void InfoUI_Script::LateUpdate()
{
}
