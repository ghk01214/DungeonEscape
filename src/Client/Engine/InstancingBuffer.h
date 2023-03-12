#pragma once

struct InstancingParams
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
};

class InstancingBuffer
{
public:
	InstancingBuffer();
	~InstancingBuffer();

	void Init(uint32 maxCount = 10);

	void Clear();
	void AddData(InstancingParams& params);
	void PushData();

public:
	uint32						GetCount() { return static_cast<uint32>(m_data.size()); }
	ComPtr<ID3D12Resource>		GetBuffer() { return m_buffer; }
	D3D12_VERTEX_BUFFER_VIEW	GetBufferView() { return m_bufferView; }

	void	SetID(uint64 instanceId) { m_instanceId = instanceId; }
	uint64	GetID() { return m_instanceId; }

private:
	uint64						m_instanceId = 0;
	ComPtr<ID3D12Resource>		m_buffer;
	D3D12_VERTEX_BUFFER_VIEW	m_bufferView;

	uint32						m_maxCount = 0;
	vector<InstancingParams>	m_data;
};