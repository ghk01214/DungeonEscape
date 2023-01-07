#pragma once
#include "Component.h"

class CMonoBehaviour : public CComponent
{
public:
	CMonoBehaviour();
	virtual ~CMonoBehaviour();


private:
	virtual void FinalUpdate() sealed { }	// sealed - 다른 class들이 CMonoBehaviour를 상속받아서 FinalUpdate함수를 오버라이드하는것을 막아줌
};

