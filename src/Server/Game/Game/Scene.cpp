#include "pch.h"
#include "Scene.h"

namespace game
{
	CScene::CScene()
	{
	}

	CScene::~CScene()
	{
	}

	void CScene::DecodeKeyInput(uint8_t keyInput, server::KEY_STATE keyState)
	{
		auto ks{ static_cast<uint8_t>(keyState) };

		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::LEFT)) != 0x00)
		{

		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::RIGHT)) != 0x00)
		{

		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::UP)) != 0x00)
		{

		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::DOWN)) != 0x00)
		{

		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::SPACE)) != 0x00)
		{

		}
	}
}