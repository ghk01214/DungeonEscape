#pragma once

#include "Component.h"

class UI : public Component
{
public:
	UI();
	virtual ~UI();

public:
	virtual void Awake(void) override;
	virtual void Start(void) override;
	virtual void Update(void) override;
	virtual void LateUpdate(void) override;
	virtual void FinalUpdate(void) override;

public:
	void SetVisible(bool visible) { m_bVisible = visible; }
	void SetPopUp(bool popUp) { m_bPopUp = popUp; }
	void OnTop(void) { m_rank = ++m_curRank; }
	void SetPopUpUI(bool isPopUpUI) { m_isPopUpUI = isPopUpUI; }

public:
	int GetRank(void) { return m_rank; }
	bool GetPopUp(void) { return m_bPopUp; }
	bool GetVisible(void) { return m_bVisible; }
	bool isPopUpUI(void) { return m_isPopUpUI; }

private:
	int m_rank = 0;
	bool m_bPopUp = false;
	bool m_bVisible = true;
	bool m_isPopUpUI = false;

public:
	static int m_curRank;
};

