#pragma once

// [유니티짱]과 같이 정점으로 이루어진 물체
class CMesh
{
public:
	void Init(std::vector<Vertex>& vec);
	void Render();

private:
	ComPtr<ID3D12Resource>		m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView = {};
	uint32 _vertexCount = 0;
};

