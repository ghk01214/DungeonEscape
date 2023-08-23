#pragma once

#include "Button_Script.h"

class DescriptionButton_Script : public Button_Script
{
public:
	DescriptionButton_Script();
	virtual ~DescriptionButton_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetCharacterType(server::FBX_TYPE type);
	void SetCharacterType(int32_t index);

private:
	server::FBX_TYPE m_character;

	bool m_select;
};

