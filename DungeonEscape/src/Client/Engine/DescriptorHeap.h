#pragma once


// [기안서]
// 외주를 맡길 때 이런 저런 정보들을 같이 넘겨줘야 하는데,
// 아무 형태로나 요청하면 못 알아먹는다
// - 각종 리소스를 어떤 용도로 사용하는지 꼼꼼하게 적어서 넘겨주는 용도

class CSwapChain;

class CDescriptorHeap // View
{
public:
	void Init(ComPtr<ID3D12Device> device, std::shared_ptr<CSwapChain> swapChain);

	D3D12_CPU_DESCRIPTOR_HANDLE		GetRTV(int32 idx) { return m_rtvHandle[idx]; }

	D3D12_CPU_DESCRIPTOR_HANDLE		GetBackBufferView();

private:
	// rtv(render target view)
	ComPtr<ID3D12DescriptorHeap>	m_rtvHeap;
	uint32							m_rtvHeapSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];
									
	std::shared_ptr<CSwapChain>		m_swapChain;
};