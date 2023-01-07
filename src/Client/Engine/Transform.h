#pragma once
#include "Component.h"

class CTransform : public CComponent
{
public:
	CTransform();
	virtual ~CTransform();

	virtual void FinalUpdate() override;
	void PushData();

public:
	// Parent 기준
	const Vec3& GetLocalPosition() { return m_localPosition; }
	const Vec3& GetLocalRotation() { return m_localRotation; }
	const Vec3& GetLocalScale() { return m_localScale; }

	const Matrix& GetLocalToWorldMatrix() { return m_matWorld; }
	const Vec3& GetWorldPosition() { return m_matWorld.Translation(); }

	Vec3 GetRight() { return m_matWorld.Right(); }
	Vec3 GetUp() { return m_matWorld.Up(); }
	Vec3 GetLook() { return m_matWorld.Backward(); }	// SimpleMath가 오른손 좌표계이기 때문에 왼손좌표계로 사용하기 위해 반대쪽을 바라보는 함수 사용

	void SetLocalPosition(const Vec3& position) { m_localPosition = position; }
	void SetLocalRotation(const Vec3& rotation) { m_localRotation = rotation; }
	void SetLocalScale(const Vec3& scale) { m_localScale = scale; }

public:
	void SetParent(std::shared_ptr<CTransform> parent) { m_parent = parent; }
	std::weak_ptr<CTransform> GetParent() { return m_parent; }

private:
	// Parent 기준
	Vec3 m_localPosition = {};
	Vec3 m_localRotation = {};
	Vec3 m_localScale = { 1.f, 1.f, 1.f };

	Matrix m_matLocal = {};
	Matrix m_matWorld = {};

	std::weak_ptr<CTransform> m_parent;	// 순환참조 방지용 weak_ptr
};

