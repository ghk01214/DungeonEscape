#pragma once

class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return m_graphicsRootSignature; }
	ComPtr<ID3D12RootSignature>	GetComputeRootSignature() { return m_computeRootSignature; }

private:
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

private:
	D3D12_STATIC_SAMPLER_DESC	m_samplerDesc; 
	ComPtr<ID3D12RootSignature>	m_graphicsRootSignature;	
	ComPtr<ID3D12RootSignature>	m_computeRootSignature;
};

