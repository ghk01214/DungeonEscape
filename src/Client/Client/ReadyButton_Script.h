#pragma once

#include "Button_Script.h"

class ReadyButton_Script : public Button_Script
{
public:
	ReadyButton_Script();
	virtual ~ReadyButton_Script();

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	constexpr bool IsReady() const { return m_ready; }

	void SetCharacterSelectFlag(bool flag);

private:
	bool m_ready;
	bool m_characterSelect;
};

