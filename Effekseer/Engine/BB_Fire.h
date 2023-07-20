#pragma once

#include "BillBoard.h"

class BB_Fire final : public BillBoard
{
public:
	BB_Fire();
	virtual ~BB_Fire();

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();

};

