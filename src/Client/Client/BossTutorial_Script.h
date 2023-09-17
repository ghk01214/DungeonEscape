#pragma once

#include "UI_Script.h"

class BossTutorial_Script : public UI_Script
{
public:
	enum PAGE_NUM
	{
		PAGE1,
		PAGE2,

		MAX
	};
public:
	BossTutorial_Script();
	virtual ~BossTutorial_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

private:
	void ChangePage(int32_t index);

public:
	void ChangeToPrevPage();
	void ChangeToNextPage();

	void SetVisible(bool flag);
	void SetPage(int32_t num);

private:
	int32_t m_currPage;
};

