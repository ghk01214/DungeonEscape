#pragma once

namespace Client
{
	const unsigned int	g_iWinCX = 1280;
	const unsigned int	g_iWinCY = 720;

	enum SCENE
	{
		SCENE_STATIC,
		SCENE_LOADING,
		SCENE_LOGO,
		SCENE_LOBBY,
		SCENE_CHARACTER_SELECT,
		SCENE_GAMEPLAY,
		SCENE_END
	};
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
