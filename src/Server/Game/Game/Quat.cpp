#include "pch.h"
#include "Quat.h"

Quat::Quat() : DirectX::XMFLOAT4(0, 0, 0, 1)
{
}

Quat::Quat(const DirectX::XMFLOAT4& q) : DirectX::XMFLOAT4(q)
{
}

Quat::Quat(const Quat& q) : DirectX::XMFLOAT4(q)
{
}

Quat::Quat(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w)
{
}

Quat::Quat(const DirectX::XMVECTOR& v)
{
    DirectX::XMStoreFloat4(this, v);
}

Quat& Quat::operator+=(const Quat& q)
{
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
}

Quat& Quat::operator-=(const Quat& q)
{
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
}

Quat& Quat::operator*=(const Quat& q)
{
    DirectX::XMVECTOR thisQ = DirectX::XMLoadFloat4(this);
    DirectX::XMVECTOR otherQ = DirectX::XMLoadFloat4(&q);
    DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply(thisQ, otherQ);
    DirectX::XMStoreFloat4(this, result);
    return *this;
}

Quat& Quat::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

Quat& Quat::operator/=(float s)
{
    x /= s;
    y /= s;
    z /= s;
    w /= s;
    return *this;
}

Quat Quat::operator+() const
{
    return *this;
}

Quat Quat::operator-() const
{
    return Quat(-x, -y, -z, w);
}

Quat Quat::operator+(const Quat& q) const
{
    return Quat(x + q.x, y + q.y, z + q.z, w + q.w);
}

Quat Quat::operator-(const Quat& q) const
{
    return Quat(x - q.x, y - q.y, z - q.z, w - q.w);
}

Quat Quat::operator*(const Quat& q) const
{
    DirectX::XMVECTOR thisQ = DirectX::XMLoadFloat4(this);
    DirectX::XMVECTOR otherQ = DirectX::XMLoadFloat4(&q);
    DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply(thisQ, otherQ);
    Quat output;
    DirectX::XMStoreFloat4(&output, result);
    return output;
}

Quat Quat::operator*(float s) const
{
    return Quat(x * s, y * s, z * s, w * s);
}

Quat Quat::operator/(float s) const
{
    return Quat(x / s, y / s, z / s, w / s);
}

Vec3 Quat::ToEuler()
{
    Vec3 euler;

    // Roll (X-Axis)
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    euler.x = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (Y-Axis)
    float sinp = 2.0f * (w * y - z * x);
    if (std::abs(sinp) >= 1)
        euler.y = std::copysign(M_PI / 2, sinp); // Use 90 degrees if out of range
    else
        euler.y = std::asin(sinp);

    // Yaw (Z-Axis)
    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    euler.z = std::atan2(siny_cosp, cosy_cosp);

    //angle > degree
    euler.x *= 180.0f / static_cast<float>(M_PI);
    euler.y *= 180.0f / static_cast<float>(M_PI);
    euler.z *= 180.0f / static_cast<float>(M_PI);

    return euler;
}

float Quat::SqrMagnitude() const
{
    return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(XMLoadFloat4(this)));
}

float Quat::Magnitude() const
{
    return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(XMLoadFloat4(this)));
}

Quat Quat::Normalized() const
{
    Quat q = *this;
    q.Normalize();
    return q;
}

void Quat::Normalize()
{
    DirectX::XMStoreFloat4(this, DirectX::XMQuaternionNormalize(XMLoadFloat4(this)));
}

Quat Quat::Inversed() const
{
    Quat q = *this;
    q.Inverse();
    return q;
}

void Quat::Inverse()
{
    DirectX::XMStoreFloat4(this, DirectX::XMQuaternionInverse(XMLoadFloat4(this)));
}
