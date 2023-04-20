#include "pch.h"
#include "Vec3.h"

Vec3::Vec3() : DirectX::XMFLOAT3()
{
}

Vec3::Vec3(const DirectX::XMFLOAT2& v) : DirectX::XMFLOAT3(v.x, v.y, 0)
{
}

Vec3::Vec3(const DirectX::XMFLOAT3& v) : DirectX::XMFLOAT3(v)
{
}

Vec3::Vec3(const DirectX::XMFLOAT4& v) : DirectX::XMFLOAT3(v.x, v.y, v.z)
{
}

Vec3::Vec3(const DirectX::XMFLOAT4X4* m) : DirectX::XMFLOAT3(m->_41, m->_42, m->_43)
{
}

Vec3::Vec3(const float& x, const float& y, const float& z) : DirectX::XMFLOAT3(x, y, z)
{
}

Vec3& Vec3::operator+=(const Vec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vec3& Vec3::operator-=(const Vec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vec3& Vec3::operator*=(const float& s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vec3& Vec3::operator/=(const float& s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

Vec3& Vec3::operator*=(const Vec3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

Vec3& Vec3::operator/=(const Vec3& v)
{
	const float epsilon = 1e-6f;		// 0 예외처리

	if (std::abs(v.x) > epsilon) x /= v.x;
	if (std::abs(v.y) > epsilon) y /= v.y;
	if (std::abs(v.z) > epsilon) z /= v.z;

	return *this;
}

Vec3 Vec3::operator+() const
{
	return { +x,+y,+z };
}

Vec3 Vec3::operator-() const
{
	return { -x,-y,-z };
}

Vec3 Vec3::operator+(const Vec3& v) const
{
	return { x + v.x,y + v.y,z + v.z };
}

Vec3 Vec3::operator-(const Vec3& v) const
{
	return { x - v.x,y - v.y,z - v.z };
}

Vec3 Vec3::operator*(const float& s) const
{
	return { x * s,y * s,z * s };
}

Vec3 Vec3::operator/(const float& s) const
{
	return { x / s,y / s,z / s };
}

Vec3 Vec3::operator*(const Vec3& v) const
{
	return Vec3(x * v.x, y * v.y, z * v.z);
}

Vec3 Vec3::operator/(const Vec3& v) const
{
	const float epsilon = 1e-6f;

	float newX = (std::abs(v.x) > epsilon) ? x / v.x : x;
	float newY = (std::abs(v.y) > epsilon) ? y / v.y : y;
	float newZ = (std::abs(v.z) > epsilon) ? z / v.z : z;

	return Vec3(newX, newY, newZ);
}

bool Vec3::operator==(const Vec3& v) const
{
	//	return (x == v.x) && (y == v.y) && (z == v.z);
	// 정밀도가 지나치게 높다면 위의 코드로 변경
    DirectX::XMVECTOR vec1 = XMLoadFloat3(this);
    DirectX::XMVECTOR vec2 = XMLoadFloat3(&v);
    return DirectX::XMVector3Equal(vec1, vec2);	
}

bool Vec3::operator!=(const Vec3& v) const
{
	return !(*this == v);
}

Vec3::operator DirectX::XMFLOAT2() const
{
	return DirectX::XMFLOAT2(x, y);
}

Vec3::operator DirectX::XMFLOAT3() const
{
	return DirectX::XMFLOAT3(x, y, z);
}

Vec3::operator DirectX::XMFLOAT4() const
{
	return DirectX::XMFLOAT4(x, y, z, 0);
}

Vec3 Vec3::back()
{
	return Vec3(0, 0, -1);
}

Vec3 Vec3::down()
{
	return Vec3(0, -1, 0);
}

Vec3 Vec3::forward()
{
	return Vec3(0, 0, 1);
}

Vec3 Vec3::left()
{
	return Vec3(-1, 0, 0);
}

Vec3 Vec3::right()
{
	return Vec3(1, 0, 0);
}

Vec3 Vec3::up()
{
	return Vec3(0, 1, 0);
}

Vec3 Vec3::one()
{
	return Vec3(1, 1, 1);
}

Vec3 Vec3::zero()
{
	return Vec3(0, 0, 0);
}




