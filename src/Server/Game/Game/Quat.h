#pragma once

#include <DirectXMath.h>

//DIRECTX::XMFLOAT4을 상속하는 기본 벡터 클래스
//기타 유틸리티는 이후 추가

struct Quat : public DirectX::XMFLOAT4
{
    using Base = DirectX::XMFLOAT4;

public:
    Quat();
    Quat(const DirectX::XMFLOAT4& q);
	Quat(const Quat& q);
    Quat(float x, float y, float z, float w);
	Quat(const DirectX::XMVECTOR& v);

	Quat& operator += (const Quat& q);
	Quat& operator -= (const Quat& q);
	Quat& operator *= (const Quat& q);
	Quat& operator *= (float s);
	Quat& operator /= (float s);

	Quat  operator + () const;
	Quat  operator - () const;

	Quat operator + (const Quat& q) const;
	Quat operator - (const Quat& q) const;
	Quat operator * (const Quat& q) const;
	Quat operator * (float s) const;
	Quat operator / (float s) const;

public:
	float SqrMagnitude() const;
	float Magnitude() const;
	Quat Normalized() const;
	void Normalize(); 
	Quat Inversed() const;
	void Inverse();			//normalize 와 inverse는 현재 값을 변경시킨다. const(X)
};

