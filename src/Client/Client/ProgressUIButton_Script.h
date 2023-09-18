#pragma once

#include "Button_Script.h"

class ProgressUIButton_Script final : public Button_Script
{
public:
	ProgressUIButton_Script();
	virtual ~ProgressUIButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	constexpr bool IsOpenProgress() const { return m_openProgress; }

	void SetOpenProgressFlag(bool flag);
	void SetVisible(bool flag);

private:
	bool m_openProgress;
};

