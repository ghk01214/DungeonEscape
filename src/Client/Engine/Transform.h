#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		double			SpeedPerSec;
		double			RotationPerSec;

		tagTransformDesc()
		{
			SpeedPerSec = 1000.f;
			RotationPerSec = 20.f;
		}

	}TRANSFORMDESC;

public:
	Transform();
	virtual ~Transform();

	virtual void FinalUpdate() override;
	void PushData();

public:
	// Parent 기준
	const Vec3& GetLocalPosition() { return _localPosition; }
	const Vec3& GetLocalRotation() { return _localRotation; }
	const Vec3& GetLocalScale() { return _localScale; }
	const Vec3& GetWorldVec3Position() { return _worldPosition; }
	const Vec3& GetWorldVec3Rotation() { return _worldRotation; }
	const Vec3& GetWorldVec3Scale() { return _worldScale; }

	// TEMP
	float GetBoundingSphereRadius() { return max(max(_localScale.x, _localScale.y), _localScale.z); }

	const Matrix& GetLocalToWorldMatrix() { return _matLocalMulWorld; }
	const Matrix& GetLocalMatrix() { return _matLocal; }
	const Matrix& GetWorldMatrix() { return _matWorld; }
	Vec3 GetWorldPosition() { return _matWorld.Translation(); }

	Vec3 GetRight() { return _matWorld.Right(); }
	Vec3 GetUp() { return _matWorld.Up(); }
	Vec3 GetLook() { return _matWorld.Backward(); }
	void SetRight(const Vec3& right) { _matWorld.Right(right); }
	void SetUp(const Vec3& up) { return _matWorld.Up(up); }
	void SetLook(const Vec3& look) { return _matWorld.Backward(look); }

	void SetLocalPosition(const Vec3& position) { _localPosition = position; }
	void SetLocalRotation(const Vec3& rotation) { _localRotation = rotation; }
	void SetLocalScale(const Vec3& scale) { _localScale = scale; }
	void SetWorldVec3Position(const Vec3& position) { _worldPosition = position; }
	void SetWorldVec3Rotation(const Vec3& rotation) { _worldRotation = rotation; }
	void SetWorldVec3Scale(const Vec3& scale) { _worldScale = scale; }
	void SetWorldMatrix(Matrix& world) { _matWorld = world; }

	void LookAt(const Vec3& dir);

	static bool CloseEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon());
	static Vec3 DecomposeRotationMatrix(const Matrix& rotation);


public:
	void SetParent(shared_ptr<Transform> parent) { _parent = parent; }
	weak_ptr<Transform> GetParent() { return _parent; }

public:
	void TurnAxisY(bool clockWise);

private:
	// Parent 기준
	Vec3 _localPosition = { 0.f, 0.f, 0.f };
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localScale = { 1.f, 1.f, 1.f };

	Vec3 _worldPosition = { 0.f, 0.f, 0.f };
	Vec3 _worldRotation = { 0.f, 0.f, 0.f };
	Vec3 _worldScale = { 1.f, 1.f, 1.f };

	Matrix _matLocal = Matrix::Identity;
	Matrix _matWorld = Matrix::Identity;
	Matrix _matLocalMulWorld = Matrix::Identity;

	TRANSFORMDESC m_transformDesc;

	weak_ptr<Transform> _parent;
};

