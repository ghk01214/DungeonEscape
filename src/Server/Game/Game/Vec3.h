#pragma once

#include <DirectXMath.h>

//DIRECTX::XMFLOAT3을 상속하는 기본 벡터 클래스
//dot, projection등의 기타 유틸리티는 이후 추가
//XMVECTOR를 사용하는 방법도 있으나, 현재까지는 조사를 하지 못함.

struct Vec3 : public DirectX::XMFLOAT3
{
public:
    Vec3();
    Vec3(const DirectX::XMFLOAT2& v);
    Vec3(const DirectX::XMFLOAT3& v);
    Vec3(const DirectX::XMFLOAT4& v);
    Vec3(const DirectX::XMFLOAT4X4* m);
    Vec3(const float& x, const float& y, const float& z);

public:
    Vec3& operator += (const Vec3& v);
    Vec3& operator -= (const Vec3& v);
    Vec3& operator *= (const float& s);
    Vec3& operator /= (const float& s);
    Vec3& operator *= (const Vec3& v);
    Vec3& operator /= (const Vec3& v);

    Vec3 operator + () const; // +v
    Vec3 operator - () const; // -v

    Vec3 operator + (const Vec3& v) const;
    Vec3 operator - (const Vec3& v) const;
    Vec3 operator * (const float& s) const;
    Vec3 operator / (const float& s) const;
    Vec3 operator * (const Vec3& v) const;
    Vec3 operator / (const Vec3& v) const;

    bool operator == (const Vec3& v) const;
    bool operator != (const Vec3& v) const;

public:
    operator DirectX::XMFLOAT2() const;
    operator DirectX::XMFLOAT3() const;
    operator DirectX::XMFLOAT4() const;

public:


public:
    static Vec3 back();
    static Vec3 down();
    static Vec3 forward();
    static Vec3 left();
    static Vec3 right();
    static Vec3 up();
    static Vec3 one();
    static Vec3 zero();
};