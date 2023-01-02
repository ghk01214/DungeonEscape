#pragma once
#include "Component.h"

struct TransformMatrix
{
	Vec4 offset;
};


class CTransform : public CComponent
{
public:
	CTransform();
	virtual ~CTransform();

	// TODO : 온갖 Parent/Child 관계

private:
	// TODO : World 위치 관련

};

