#pragma once


class CTableDescriptorHeap
{
public:
	void Init(uint32 count);

	void Clear();
	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);

	void CommitTable();

	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return m_descHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);		// Constant Buffer View의 레지스터 핸들값 반환
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER reg);		// Shadow Resource View의 레지스터 핸들값 반환

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 reg);

private:
	ComPtr<ID3D12DescriptorHeap> m_descHeap;
	uint64					m_handleSize = 0;
	uint64					m_groupSize = 0;
	uint64					m_groupCount = 0;

	uint32					m_currentGroupIndex = 0;	// 현재 사용하고 있는 그룹의 인덱스 번호, 매 프레임마다 0으로 초기화 후 사용한다.
};

