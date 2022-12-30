#pragma once

class CSwapChain;
class CDescriptorHeap;

class CCommandQueue
{
public:
	CCommandQueue();
	~CCommandQueue();

	void Init(ComPtr<ID3D12Device> device, std::shared_ptr<CSwapChain> swapChain);
	void WaitSync();

	void RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect);
	void RenderEnd();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return m_cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdList(void) { return m_cmdList; }

private:
	// CommandQueue : DX12에 등장
	// 외주를 요청할 때, 하나씩 요청하면 비효율적
	// [외주 목록]에 일감을 차곡차곡 기록했다가 한 방에 요청하는 것
	ComPtr<ID3D12CommandQueue>			m_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		m_cmdAlloc;	// CommandList에 추가된 Command들은 Allocator의 메모리에 저장된다.
	ComPtr<ID3D12GraphicsCommandList>	m_cmdList;

	// Fence : 울타리(?)
	// CPU / GPU 동기화를 위한 간단한 도구
	ComPtr<ID3D12Fence>					m_fence;
	uint32								m_fenceValue = 0;
	HANDLE								m_fenceEvent = INVALID_HANDLE_VALUE;

	std::shared_ptr<CSwapChain>			m_swapChain;
};

