#include "pch.h"
#include "Transform.h"

CTransform::CTransform() : CComponent(COMPONENT_TYPE::TRANSFORM)
{

}

CTransform::~CTransform()
{

}

// CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&m_transform, sizeof(m_transform));