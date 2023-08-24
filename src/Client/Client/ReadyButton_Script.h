#pragma once

#include "Button_Script.h"

class ReadyButton_Script final : public Button_Script
{
public:
	ReadyButton_Script();
	virtual ~ReadyButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	constexpr bool IsReady() const { return m_ready; }

	void SetCharacterSelectFlag(bool flag);

private:
	bool m_ready;
	bool m_characterSelect;
};

