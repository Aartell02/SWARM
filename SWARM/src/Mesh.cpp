#include "Mesh.hpp"
#include <stdexcept>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

void Mesh::CreateBuffers(ID3D12Device* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
	if (vertices.empty() || indices.empty()) {
		throw std::runtime_error("Cannot create mesh with empty vertices or indices");
	}

	m_indexCount = static_cast<uint32_t>(indices.size());

	// Create vertex buffer
	{
		const UINT vertexBufferSize = static_cast<UINT>(vertices.size() * sizeof(Vertex));

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = vertexBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer)))) {
			throw std::runtime_error("Failed to create vertex buffer");
		}

		// Copy data
		void* pVertexDataBegin = nullptr;
		D3D12_RANGE readRange{ 0, 0 };
		if (FAILED(m_vertexBuffer->Map(0, &readRange, &pVertexDataBegin))) {
			throw std::runtime_error("Failed to map vertex buffer");
		}
		memcpy(pVertexDataBegin, vertices.data(), vertexBufferSize);
		m_vertexBuffer->Unmap(0, nullptr);

		// Set view
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	// Create index buffer
	{
		const UINT indexBufferSize = static_cast<UINT>(indices.size() * sizeof(uint32_t));

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = indexBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_indexBuffer)))) {
			throw std::runtime_error("Failed to create index buffer");
		}

		// Copy data
		void* pIndexDataBegin = nullptr;
		D3D12_RANGE readRange{ 0, 0 };
		if (FAILED(m_indexBuffer->Map(0, &readRange, &pIndexDataBegin))) {
			throw std::runtime_error("Failed to map index buffer");
		}
		memcpy(pIndexDataBegin, indices.data(), indexBufferSize);
		m_indexBuffer->Unmap(0, nullptr);

		// Set view
		m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
		m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_indexBufferView.SizeInBytes = indexBufferSize;
	}
}

D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBufferView() const {
	return m_vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView() const {
	return m_indexBufferView;
}

// ===== MeshFactory implementations =====

Mesh MeshFactory::CreateCube(ID3D12Device* device, float size, const XMFLOAT4& color) {
	float s = size * 0.5f;

	std::vector<Vertex> vertices = {
		// Front face
		{ XMFLOAT3(-s, -s, -s), XMFLOAT3(0, 0, -1), color },
		{ XMFLOAT3(s, -s, -s), XMFLOAT3(0, 0, -1), color },
		{ XMFLOAT3(s, s, -s), XMFLOAT3(0, 0, -1), color },
		{ XMFLOAT3(-s, s, -s), XMFLOAT3(0, 0, -1), color },

		// Back face
		{ XMFLOAT3(s, -s, s), XMFLOAT3(0, 0, 1), color },
		{ XMFLOAT3(-s, -s, s), XMFLOAT3(0, 0, 1), color },
		{ XMFLOAT3(-s, s, s), XMFLOAT3(0, 0, 1), color },
		{ XMFLOAT3(s, s, s), XMFLOAT3(0, 0, 1), color },

		// Top face
		{ XMFLOAT3(-s, s, -s), XMFLOAT3(0, 1, 0), color },
		{ XMFLOAT3(s, s, -s), XMFLOAT3(0, 1, 0), color },
		{ XMFLOAT3(s, s, s), XMFLOAT3(0, 1, 0), color },
		{ XMFLOAT3(-s, s, s), XMFLOAT3(0, 1, 0), color },

		// Bottom face
		{ XMFLOAT3(-s, -s, s), XMFLOAT3(0, -1, 0), color },
		{ XMFLOAT3(s, -s, s), XMFLOAT3(0, -1, 0), color },
		{ XMFLOAT3(s, -s, -s), XMFLOAT3(0, -1, 0), color },
		{ XMFLOAT3(-s, -s, -s), XMFLOAT3(0, -1, 0), color },

		// Right face
		{ XMFLOAT3(s, -s, -s), XMFLOAT3(1, 0, 0), color },
		{ XMFLOAT3(s, -s, s), XMFLOAT3(1, 0, 0), color },
		{ XMFLOAT3(s, s, s), XMFLOAT3(1, 0, 0), color },
		{ XMFLOAT3(s, s, -s), XMFLOAT3(1, 0, 0), color },

		// Left face
		{ XMFLOAT3(-s, -s, s), XMFLOAT3(-1, 0, 0), color },
		{ XMFLOAT3(-s, -s, -s), XMFLOAT3(-1, 0, 0), color },
		{ XMFLOAT3(-s, s, -s), XMFLOAT3(-1, 0, 0), color },
		{ XMFLOAT3(-s, s, s), XMFLOAT3(-1, 0, 0), color },
	};

	std::vector<uint32_t> indices = {
		// Front
		0, 2, 1, 0, 3, 2,
		// Back
		4, 6, 5, 4, 7, 6,
		// Top
		8, 10, 9, 8, 11, 10,
		// Bottom
		12, 14, 13, 12, 15, 14,
		// Right
		16, 18, 17, 16, 19, 18,
		// Left
		20, 22, 21, 20, 23, 22,
	};

	Mesh mesh;
	mesh.CreateBuffers(device, vertices, indices);
	return mesh;
}

Mesh MeshFactory::CreatePlane(ID3D12Device* device, float width, float depth, const XMFLOAT4& color) {
	float w = width * 0.5f;
	float d = depth * 0.5f;

	std::vector<Vertex> vertices = {
		{ XMFLOAT3(-w, 0, -d), XMFLOAT3(0, 1, 0), color },
		{ XMFLOAT3(w, 0, -d), XMFLOAT3(0, 1, 0), color },
		{ XMFLOAT3(w, 0, d), XMFLOAT3(0, 1, 0), color },
		{ XMFLOAT3(-w, 0, d), XMFLOAT3(0, 1, 0), color },
	};

	std::vector<uint32_t> indices = {
		0, 2, 1,
		0, 3, 2,
	};

	Mesh mesh;
	mesh.CreateBuffers(device, vertices, indices);
	return mesh;
}
