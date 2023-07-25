#pragma once

#include "Lobby_Script.h"

class Lobby_StartButton : public Lobby_Script
{
public:
	enum TEXTURE_TYPE
	{
		START_BUTTON,
		START_BUTTON_PRESSED,

		MAX
	};
public:
	Lobby_StartButton();
	virtual ~Lobby_StartButton();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	Vec3 m_pos;
	Vec3 m_scale;

	bool m_click;
	bool m_lobbyEnd;
};