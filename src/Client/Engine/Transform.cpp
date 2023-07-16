#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"
#include "Timer.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{

}

Transform::~Transform()
{

}

void Transform::FinalUpdate()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(XMConvertToRadians(_localRotation.x));
	matRotation *= Matrix::CreateRotationY(XMConvertToRadians(_localRotation.y));
	matRotation *= Matrix::CreateRotationZ(XMConvertToRadians(_localRotation.z));
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matScale * matRotation * matTranslation;

	_matLocalMulWorld = _matLocal * _matWorld;

	shared_ptr<Transform> parent = GetParent().lock();
	if (parent != nullptr)
	{
		_matLocalMulWorld *= parent->GetLocalToWorldMatrix();
	}
}

void Transform::PushData()
{
	Matrix world = _matLocalMulWorld;

	TransformParams transformParams = {};
	transformParams.matWorld = world;
	transformParams.matView = Camera::S_MatView;
	transformParams.matProjection = Camera::S_MatProjection;
	transformParams.matWV = world * Camera::S_MatView;
	transformParams.matWVP = world * Camera::S_MatView * Camera::S_MatProjection;
	transformParams.matViewInv = Camera::S_MatView.Invert();

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParams, sizeof(transformParams));
}

void Transform::LookAt(const Vec3& dir)
{
	Vec3 front = dir;
	front.Normalize();

	Vec3 right = Vec3::Up.Cross(dir);
	if (right == Vec3::Zero)
		right = Vec3::Forward.Cross(dir);

	right.Normalize();

	Vec3 up = front.Cross(right);
	up.Normalize();

	Matrix matrix = XMMatrixIdentity();
	matrix.Right(right);
	matrix.Up(up);
	matrix.Backward(front);

	Vec3 temp = DecomposeRotationMatrix(matrix);
	_localRotation.x = XMConvertToDegrees(temp.x);
	_localRotation.y = XMConvertToDegrees(temp.y);
	_localRotation.z = XMConvertToDegrees(temp.z);
}

bool Transform::CloseEnough(const float& a, const float& b, const float& epsilon)
{
	return (epsilon > std::abs(a - b));
}

Vec3 Transform::DecomposeRotationMatrix(const Matrix& rotation)
{
	Vec4 v[4];
	XMStoreFloat4(&v[0], rotation.Right());
	XMStoreFloat4(&v[1], rotation.Up());
	XMStoreFloat4(&v[2], rotation.Backward());
	XMStoreFloat4(&v[3], rotation.Translation());

	Vec3 ret;
	if (CloseEnough(v[0].z, -1.0f))
	{
		float x = 0;
		float y = XM_PI / 2;
		float z = x + atan2(v[1].x, v[2].x);
		ret = Vec3{ x, y, z };
	}
	else if (CloseEnough(v[0].z, 1.0f))
	{
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2(-v[1].x, -v[2].x);
		ret = Vec3{ x, y, z };
	}
	else
	{
		float y1 = -asin(v[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(v[1].z / cos(y1), v[2].z / cos(y1));
		float x2 = atan2f(v[1].z / cos(y2), v[2].z / cos(y2));

		float z1 = atan2f(v[0].y / cos(y1), v[0].x / cos(y1));
		float z2 = atan2f(v[0].y / cos(y2), v[0].x / cos(y2));

		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			ret = Vec3{ x1, y1, z1 };
		}
		else
		{
			ret = Vec3{ x2, y2, z2 };
		}
	}

	return ret;
}

void Transform::TurnAxisY(bool clockWise)
{
	XMVECTOR vRight = GetRight();
	XMVECTOR vUp = GetUp();
	XMVECTOR vLook = GetLook();

	Matrix matRotation;
	if (clockWise)
		matRotation = XMMatrixRotationY(DELTA_TIME * m_transformDesc.RotationPerSec);
	else
		matRotation = XMMatrixRotationY(-DELTA_TIME * m_transformDesc.RotationPerSec);

	vRight = XMVector3TransformNormal(vRight, matRotation);
	vUp = XMVector3TransformNormal(vUp, matRotation);
	vLook = XMVector3TransformNormal(vLook, matRotation);

	SetRight(vRight);
	SetUp(vUp);
	SetLook(vLook);
}

float Transform::GetWorldMatrixDegreeAxisY(void)
{
	Vec3 vRotation = XMQuaternionRotationMatrix(_matWorld);
	return XMConvertToDegrees(vRotation.y);
}

Quat Transform::GetWorldMatrixQuat(void)
{
	return XMQuaternionRotationMatrix(_matWorld);
}
