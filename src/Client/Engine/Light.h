#pragma once
#include "Component.h"

enum class LIGHT_TYPE : uint8
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

struct LightColor
{
	Vec4	diffuse;
	Vec4	ambient;
	Vec4	specular;
};

struct LightInfo
{
	LightColor	color;
	Vec4		position;
	Vec4		direction;
	int32		lightType;
	float		range;
	float		angle;
	int32		padding;
};

struct LightParams
{
	uint32		lightCount;
	Vec3		padding;
	LightInfo	lights[50];
};

class Light : public Component
{
public:
	Light();
	virtual ~Light();

	virtual void FinalUpdate() override;
	void Render();
	void RenderShadow();

public:
	LIGHT_TYPE GetLightType() { return static_cast<LIGHT_TYPE>(m_lightInfo.lightType); }

	const LightInfo& GetLightInfo() { return m_lightInfo; }

	void SetLightDirection(Vec3 direction);

	void SetDiffuse(const Vec3& diffuse) { m_lightInfo.color.diffuse = diffuse; }
	void SetAmbient(const Vec3& ambient) { m_lightInfo.color.ambient = ambient; }
	void SetSpecular(const Vec3& specular) { m_lightInfo.color.specular = specular; }

	void SetLightType(LIGHT_TYPE type);
	void SetLightRange(float range) { m_lightInfo.range = range; }
	void SetLightAngle(float angle) { m_lightInfo.angle = angle; }

	void SetLightIndex(int8 index) { m_lightIndex = index; }

private:
	LightInfo m_lightInfo = {};

	int8 m_lightIndex = -1;
	shared_ptr<class Mesh> m_volumeMesh;
	shared_ptr<class Material> m_lightMaterial;

	shared_ptr<CGameObject> m_shadowCamera;
};

