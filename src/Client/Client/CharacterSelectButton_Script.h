﻿#pragma once

#include "Button_Script.h"

class CharacterSelectButton_Script final : public Button_Script
{
public:
	CharacterSelectButton_Script(server::FBX_TYPE character = server::FBX_TYPE::NONE);
	virtual ~CharacterSelectButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

private:
	TEXTURE_TYPE Select();

public:
	constexpr server::FBX_TYPE GetCharacterType() const { return m_character; }
	constexpr bool IsSelect() const { return m_select; }

	void SetCharacterType(server::FBX_TYPE type);
	void SetCharacterType(int32_t index);

private:
	bool m_select;
	bool m_prevSelect;

	server::FBX_TYPE m_character;
};

