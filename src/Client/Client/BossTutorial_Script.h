#pragma once

#include "UI_Script.h"

class BossTutorial_Script : public UI_Script
{
public:
	enum PAGE
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

public:
	void ChangePage(int32_t index);
	void SetVisible(bool flag);

};

