﻿#pragma once

// GPU에게 일을 맡기는 인력사무소, 객체 생성등을 여기서 처리한다.
class CDevice
{
public:
	void Init();

	ComPtr<IDXGIFactory> GetDXGI() { return m_dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return m_device; }

private:
	// COM(Component Object Model)
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
	// - ComPtr 일종의 스마트 포인터
	ComPtr<ID3D12Debug>			m_debugController;
	ComPtr<IDXGIFactory>		m_dxgi; // 화면 관련 기능들
	ComPtr<ID3D12Device>		m_device; // 각종 객체 생성
};