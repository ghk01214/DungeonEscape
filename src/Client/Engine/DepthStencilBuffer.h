#pragma once


class CDepthStencilBuffer
{
public:
	void Init(const WindowInfo& window, DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT);	// 기본 포맷 : DXGI_FORMAT_D32_FLOAT -> Depth 용도로 32bit float으로 활용

	D3D12_CPU_DESCRIPTOR_HANDLE	GetDSVCpuHandle() { return m_dsvHandle; }
	DXGI_FORMAT GetDSVFormat() { return m_dsvFormat; }

private:
	// Depth Stencil View
	ComPtr<ID3D12Resource>				m_dsvBuffer;
	ComPtr<ID3D12DescriptorHeap>		m_dsvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_dsvHandle = {};
	DXGI_FORMAT							m_dsvFormat = {};
};

