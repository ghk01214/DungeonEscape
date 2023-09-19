#pragma once

#include "Button_Script.h"

class GimmickTutorialUIButton_Script final : public Button_Script
{
public:
	GimmickTutorialUIButton_Script();
	virtual ~GimmickTutorialUIButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	constexpr bool IsOpenTutorial() const { return m_openTutorial; }

	void SetOpenTutorialFlag(bool flag);
	void SetVisible(bool flag);

private:
	bool m_openTutorial;
};

