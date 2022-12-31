#pragma once

// [계약서 / 결재]

// CPU [        ]    GPU [        ]
// 한국 [        ]   베트남 [       ]

class CRootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature>	GetSignature() { return m_signature; }

private:
	void CreateSamplerDesc();
	void CreateRootSignature();

private:
	D3D12_STATIC_SAMPLER_DESC m_samplerDesc;
	ComPtr<ID3D12RootSignature>	m_signature;
};

