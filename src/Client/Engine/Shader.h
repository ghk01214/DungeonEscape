#pragma once

// [일감 기술서] 외주 인력들이 뭘 해야할지 기술
class CShader
{
public:
	void Init(const std::wstring& path);
	void Update();

private:
	void CreateShader(const std::wstring& path, const std::string& name, const std::string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
	void CreateVertexShader(const std::wstring& path, const std::string& name, const std::string& version);
	void CreatePixelShader(const std::wstring& path, const std::string& name, const std::string& version);

private:
	ComPtr<ID3DBlob>					m_vsBlob;
	ComPtr<ID3DBlob>					m_psBlob;
	ComPtr<ID3DBlob>					m_errBlob;

	ComPtr<ID3D12PipelineState>			m_pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  m_pipelineDesc = {};
};

