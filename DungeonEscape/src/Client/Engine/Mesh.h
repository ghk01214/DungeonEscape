#pragma once

// [유니티짱]과 같이 정점으로 이루어진 물체
class CMesh
{
public:
	void Init(const std::vector<Vertex>& vertexBuffer, const std::vector<uint32>& indexbuffer);		// 인덱스 버퍼와 버텍스 버퍼를 가져옴
	void Render();

private:
	void CreateVertexBuffer(const std::vector<Vertex>& buffer);
	void CreateIndexBuffer(const std::vector<uint32>& buffer);

private:
	// 인덱스 버퍼
	ComPtr<ID3D12Resource>		m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;
	uint32 m_indexCount = 0;

private:
	// 버텍스 버퍼
	ComPtr<ID3D12Resource>		m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView = {};
	uint32 m_vertexCount = 0;
};

