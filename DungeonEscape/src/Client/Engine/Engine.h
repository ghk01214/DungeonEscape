#pragma once

class Engine
{
public:
	Engine();
	~Engine();

public:
	void Init(const WindowInfo& info);
	void Render();

public:
	void RenderBegin();	// 일감(리소스 및 사용법) 맡기기
	void RenderEnd();	// 가져온 일감으로 그리기

	void ResizeWindow(int32 width, int32 height);

private:
	WindowInfo		m_window;
	D3D12_VIEWPORT	m_viewport;
	D3D12_RECT		m_scissorRect;

	std::shared_ptr<class CDevice> m_device;
	std::shared_ptr<class CCommandQueue> m_cmdQueue;
	std::shared_ptr<class CSwapChain> m_swapChain;
	std::shared_ptr<class CDescriptorHeap> m_descHeap;
};

