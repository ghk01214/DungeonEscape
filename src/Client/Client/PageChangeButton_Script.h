#pragma once

#include "Button_Script.h"

class PageChangeButton_Script : public Button_Script
{
public:
	PageChangeButton_Script();
	virtual ~PageChangeButton_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

public:
	constexpr bool GetChangePageFlag() const { return m_changePage; }

	void SetVisible(bool flag);
	void SetChangePageFlag(bool flag);

private:
	bool m_changePage;
};

