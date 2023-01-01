#pragma once

class CShader;
class CTexture;

enum
{
	MATERIAL_INT_COUNT = 5,
	MATERIAL_FLOAT_COUNT = 5,
	MATERIAL_TEXTURE_COUNT = 5
};

struct MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void SetFloat(uint8 index, float value) { floatParams[index] = value; }

	std::array<int32, MATERIAL_INT_COUNT> intParams;
	std::array<float, MATERIAL_FLOAT_COUNT> floatParams;
};

class CMaterial
{
public:
	std::shared_ptr<CShader> GetShader() { return m_shader; }

	void SetShader(std::shared_ptr<CShader> shader) { m_shader = shader; }
	void SetInt(uint8 index, int32 value) { m_params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { m_params.SetFloat(index, value); }
	void SetTexture(uint8 index, std::shared_ptr<CTexture> texture) { m_textures[index] = texture; }

	void Update();

private:
	std::shared_ptr<CShader>	m_shader;
	MaterialParams		m_params;
	std::array<std::shared_ptr<CTexture>, MATERIAL_TEXTURE_COUNT> m_textures;
};

