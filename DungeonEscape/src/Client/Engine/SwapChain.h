﻿#pragma once


// 교환 사슬
// [외주 과정]
// - 현재 게임 세상에 있는 상황을 묘사
// - 어떤 공식으로 어떻게 계산할지 던져줌
// - GPU가 열심히 계산 (외주)
// - 결과물 받아서 화면에 그려준다

// [외주 결과물]을 어디에 받지?
// - 어떤 종이(Buffer)에 그려서 건내달라고 부탁해보자
// - 특수 종이를 만들어서 -> 처음에 건내주고 -> 결과물을 해당 종이에 받는다 OK
// - 우리 화면에 특수 종이(외주 결과물) 출력해준다

// [?]
// - 그런데 화면에 현재 결과물 출력하는 와중에, 다음 화면도 외주를 맡겨야 함
// - 현재 화면 결과물은 이미 화면 출력에 사용중
// - 특수 종이를 2개 만들어서, 하나는 현재 화면을 그려주고, 하나는 외주 맡기고...
// - Double Buffering!

// - [0] [1]
// 현재 화면 [0]  <-> GPU 작업중 [1] BackBuffer

class CSwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetSwapChain() { return m_swapChain; }
	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return m_renderTargets[index]; }

	uint32 GetCurrentBackBufferIndex() { return m_backBufferIndex; }	// 현재 백버퍼 인덱스가 몇번인지 반환
	ComPtr<ID3D12Resource> GetCurrentBackBufferResource() { return m_renderTargets[m_backBufferIndex]; }	// 현재 백버퍼 리소스 반환(버퍼, 그니깐 그림이 그려져 있는 종이 자체(ID3D12Resource)를 반환한다고 생각하면 됨)

private:
	ComPtr<IDXGISwapChain>	m_swapChain;
	ComPtr<ID3D12Resource>	m_renderTargets[SWAP_CHAIN_BUFFER_COUNT];
	uint32					m_backBufferIndex = 0;	// 0,1을 반복하며 바꿈
};

