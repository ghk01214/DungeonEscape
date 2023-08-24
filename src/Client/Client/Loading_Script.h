#pragma once

#include "UI_Script.h"

class CLoader;

class Loading_Script final : public UI_Script
{
private:
	enum TEXTURE_TYPE
	{
		ZERO_P,
		TEN_P,
		TWENTY_P,
		THIRTY_P,
		FORTY_P,
		FIFTY_P,
		SIXTY_P,
		SEVENTY_P,
		EIGHTY_P,
		NINETY_P,

		MAX
	};
public:
	Loading_Script();
	virtual ~Loading_Script();

public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;
};

