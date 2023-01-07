#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"

CTransform::CTransform() : CComponent(COMPONENT_TYPE::TRANSFORM)
{

}

CTransform::~CTransform()
{

}

void CTransform::FinalUpdate()
{
	// Scale
	Matrix matScale = Matrix::CreateScale(m_localScale);

	// Rotation -> 나중에 궤도 문제로 뒤집어질 수 있기 때문에 쿼터니언으로 바꿀 예정
	Matrix matRotation = Matrix::CreateRotationX(m_localRotation.x);
	matRotation *= Matrix::CreateRotationY(m_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(m_localRotation.z);

	// Translation
	Matrix matTranslation = Matrix::CreateTranslation(m_localPosition);

	// S * R * T
	m_matLocal = matScale * matRotation * matTranslation;
	m_matWorld = m_matLocal;

	// 부모의 WorldMatix를 가져와 곱해줌
	std::shared_ptr<CTransform> parent = GetParent().lock();
	if (parent != nullptr)
	{
		m_matWorld *= parent->GetLocalToWorldMatrix();
	}
}

void CTransform::PushData()
{
	Matrix matWVP = m_matWorld * CCamera::S_MatView * CCamera::S_MatProjection;
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&matWVP, sizeof(matWVP));
}
