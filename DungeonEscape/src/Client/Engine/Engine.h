#pragma once

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "TableDescriptorHeap.h"
#include "Texture.h"

class Engine
{
public:
	Engine();
	~Engine();

public:
	void Init(const WindowInfo& info);
	void Render();

public:
	std::shared_ptr<CDevice> GetDevice(void) { return m_device; }
	std::shared_ptr<CCommandQueue> GetCmdQueue(void) { return m_cmdQueue; }
	std::shared_ptr<CSwapChain> GetSwapChain(void) { return m_swapChain; }
	std::shared_ptr<CRootSignature> GetRootSignature(void) { return m_rootSignature; }
	std::shared_ptr<CConstantBuffer> GetCB() { return m_cb; }
	std::shared_ptr<CTableDescriptorHeap> GetTableDescHeap() { return m_tableDescHeap; }

public:
	void RenderBegin();	// 일감(리소스 및 사용법) 맡기기
	void RenderEnd();	// 가져온 일감으로 그리기

	void ResizeWindow(int32 width, int32 height);

private:
	WindowInfo		m_window;
	D3D12_VIEWPORT	m_viewport = {};
	D3D12_RECT		m_scissorRect = {};

	std::shared_ptr<CDevice> m_device;
	std::shared_ptr<CCommandQueue> m_cmdQueue;
	std::shared_ptr<CSwapChain> m_swapChain;
	std::shared_ptr<CRootSignature> m_rootSignature;
	std::shared_ptr<CConstantBuffer> m_cb;
	std::shared_ptr<CTableDescriptorHeap> m_tableDescHeap;
};

