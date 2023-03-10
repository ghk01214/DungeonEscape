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
#include "DepthStencilBuffer.h"

namespace network
{
	class CNetwork;
}

class Engine
{
public:
	Engine();
	~Engine();

public:
	void Init(const WindowInfo& info, std::shared_ptr<network::CNetwork> pNetwork);
	void Update();
	void LateUpdate();

	void UpdateEngine();

public:
	const WindowInfo& GetWindow() { return m_window; }
	std::shared_ptr<CDevice> GetDevice(void) { return m_device; }
	std::shared_ptr<CCommandQueue> GetCmdQueue(void) { return m_cmdQueue; }
	std::shared_ptr<CSwapChain> GetSwapChain(void) { return m_swapChain; }
	std::shared_ptr<CRootSignature> GetRootSignature(void) { return m_rootSignature; }
	std::shared_ptr<CTableDescriptorHeap> GetTableDescHeap() { return m_tableDescHeap; }
	std::shared_ptr<CDepthStencilBuffer> GetDepthStencilBuffer() { return m_depthStencilBuffer; }
	std::shared_ptr<CConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) { return m_constantBuffers[static_cast<uint8>(type)]; }

public:
	void Render();
	void RenderBegin();	// 일감(리소스 및 사용법) 맡기기
	void RenderEnd();	// 가져온 일감으로 그리기

	void ResizeWindow(int32 width, int32 height);

private:
	void ShowFps();
	void CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count);	// CB를 생성하는 함수

private:
	WindowInfo		m_window;
	D3D12_VIEWPORT	m_viewport = {};
	D3D12_RECT		m_scissorRect = {};

	std::shared_ptr<CDevice> m_device = std::make_shared<CDevice>();
	std::shared_ptr<CCommandQueue> m_cmdQueue = std::make_shared<CCommandQueue>();
	std::shared_ptr<CSwapChain> m_swapChain = std::make_shared<CSwapChain>();
	std::shared_ptr<CRootSignature> m_rootSignature = std::make_shared<CRootSignature>();
	std::shared_ptr<CTableDescriptorHeap> m_tableDescHeap = std::make_shared<CTableDescriptorHeap>();
	std::shared_ptr<CDepthStencilBuffer> m_depthStencilBuffer = std::make_shared<CDepthStencilBuffer>();

	std::vector<std::shared_ptr<CConstantBuffer>> m_constantBuffers;
};

