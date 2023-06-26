#pragma once

#include "Level.h"
class MapObject;

class TestLevel : public Level
{
public:
	TestLevel();
	virtual ~TestLevel() override;

public:
	void LoadMap();
	void LoadBasicMap1();
	void LoadBasicMap2();		//서버와 연관없는 물리 함수 구현을 위한 맵 로딩
	void LoadBasicMap3();

	void TestFunction();

public:
	virtual void Init() override;
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release() override;
};

