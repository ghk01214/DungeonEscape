#pragma once

namespace game
{
	class CSession;

	class CScene
	{
	public:
		CScene();
		~CScene();

		void DecodeKeyInput(uint8_t keyInput, server::KEY_STATE keyState);
	private:

	};
}