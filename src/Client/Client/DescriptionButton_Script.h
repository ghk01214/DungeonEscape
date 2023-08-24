#pragma once

#include "Button_Script.h"

class DescriptionButton_Script final : public Button_Script
{
public:
	DescriptionButton_Script();
	virtual ~DescriptionButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	void SetCharacterType(server::FBX_TYPE type);
	void SetCharacterType(int32_t index);

private:
	server::FBX_TYPE m_character;

	bool m_select;
};

