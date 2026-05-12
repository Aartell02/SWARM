#include "Renderer.hpp"
#include <stdexcept>
#include <SDL3/SDL.h>
#include <string>
#include <d3dcompiler.h>

// Throws on HRESULT failure with the line number so you can find it fast
#define CHECK(hr) if (FAILED(hr)) throw std::runtime_error("DX12 failed line " + std::to_string(__LINE__))

void Renderer::Init(void* hwnd, uint32_t width, uint32_t height) {
    m_viewportWidth = width;
    m_viewportHeight = height;

    CreateDevice();
    CreateCommandQueue();
    CreateSwapChain(hwnd, width, height);
    CreateRTVHeap();
    CreateFrameResources();
    CreateCommandAllocatorAndList();
    CreateFence();
    CreatePipelineStateObject();
    CreateConstantBuffer();

    m_camera.Init(static_cast<float>(width) / static_cast<float>(height));

}

void Renderer::CreateDevice() {
    CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory)));

    ComPtr<IDXGIAdapter1> adapter;
    for (uint32_t i = 0; m_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device))))
            break;

    if (!m_device)
        throw std::runtime_error("No DX12 capable GPU found");
}

void Renderer::CreateCommandQueue() {
    D3D12_COMMAND_QUEUE_DESC desc{
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 0
    };
    CHECK(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS (&m_cmdQueue)));
}

void Renderer::CreateSwapChain(void* hwnd, uint32_t width, uint32_t height) {
    DXGI_SWAP_CHAIN_DESC1 desc{
        .Width = width,
        .Height = height,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .Stereo = FALSE,
        .SampleDesc = {
            .Count = 1,
            .Quality = 0
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = FRAME_COUNT,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = 0
    };


    ComPtr<IDXGISwapChain1> sc;
    CHECK(m_factory->CreateSwapChainForHwnd(
        m_cmdQueue.Get(),
        static_cast<HWND>(hwnd),
        &desc,
        nullptr,
        nullptr,
        &sc));
    CHECK(sc.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::CreateRTVHeap() {
    D3D12_DESCRIPTOR_HEAP_DESC desc{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = FRAME_COUNT,
    };
    CHECK(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvHeap)));
    m_rtvDescSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void Renderer::CreateFrameResources() {
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

    for (uint32_t i = 0; i < FRAME_COUNT; ++i) {
        CHECK(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
        m_device->CreateRenderTargetView(
            m_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.ptr += m_rtvDescSize;
    }
}

void Renderer::CreateCommandAllocatorAndList() {
    CHECK(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator)));

    CHECK(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cmdList)));

    CHECK(m_cmdList->Close());           // Close immediately; opened each frame
}

void Renderer::CreateFence() {
    CHECK(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&m_fence)));
    m_fenceValue = 0;
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_fenceEvent) throw std::runtime_error("Failed to create fence event");
}

void Renderer::BeginRender() {
	m_currentObjectIndex = 0; // Reset object index for this frame
	m_camera.Update(); // Update camera matrices before rendering
    // --- Record commands ---
    CHECK(m_cmdAllocator->Reset());
    CHECK(m_cmdList->Reset(m_cmdAllocator.Get(), m_pipelineState.Get()));

    // Transition: Present → Render Target
    D3D12_RESOURCE_BARRIER barrier{
		.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Transition = {
            .pResource = m_renderTargets[m_frameIndex].Get(),
            .StateBefore = D3D12_RESOURCE_STATE_PRESENT,
            .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET
		}
    };

    //barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_cmdList->ResourceBarrier(1, &barrier);

    // Clear buffer
    const float clearColor[] = { 0.39f, 0.58f, 0.58f, 1.0f };

    D3D12_CPU_DESCRIPTOR_HANDLE rtv =
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtv.ptr += static_cast<size_t>(m_frameIndex) * m_rtvDescSize;

    m_cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

    // Set render target
    m_cmdList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

    // Set PSO and root signature
    m_cmdList->SetPipelineState(m_pipelineState.Get());
    m_cmdList->SetGraphicsRootSignature(m_rootSignature.Get());

    // Set viewport
    D3D12_VIEWPORT viewport{
		.TopLeftX = 0,
        .TopLeftY = 0,
        .Width = static_cast<float>(m_viewportWidth),
        .Height = static_cast<float>(m_viewportHeight),
        .MinDepth = 0.0f,
		.MaxDepth = 1.0f
    };
    m_cmdList->RSSetViewports(1, &viewport);

    // Set scissor rect
    D3D12_RECT scissorRect{
		.left = 0,
		.top = 0,
		.right = static_cast<LONG>(m_viewportWidth),
		.bottom = static_cast<LONG>(m_viewportHeight)
    };
    m_cmdList->RSSetScissorRects(1, &scissorRect);
}

void Renderer::EndRender() {
    // Transition: Render Target → Present
    D3D12_RESOURCE_BARRIER barrier{
		.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Transition = {
            .pResource = m_renderTargets[m_frameIndex].Get(),
            .StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
            .StateAfter = D3D12_RESOURCE_STATE_PRESENT
		}
    };
    //barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_cmdList->ResourceBarrier(1, &barrier);

    CHECK(m_cmdList->Close());

    // --- Execute ---
    ID3D12CommandList* lists[] = { m_cmdList.Get() };
    m_cmdQueue->ExecuteCommandLists((UINT)std::size(lists), lists);

    WaitForGPU();

    CHECK(m_swapChain->Present(1, 0));

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::WaitForGPU() {
    CHECK(m_cmdQueue->Signal(m_fence.Get(), ++m_fenceValue));
    if (m_fence->GetCompletedValue() < m_fenceValue) {
        CHECK(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void Renderer::CreatePipelineStateObject() {
    // Compile vertex shader
    ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;
    HRESULT hr = D3DCompileFromFile(L"shaders/VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            std::string errorMsg(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
            throw std::runtime_error("Vertex shader compilation failed: " + errorMsg);
        }
        throw std::runtime_error("Vertex shader compilation failed or file not found");
    }

    hr = D3DCompileFromFile(L"shaders/IndexShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            std::string errorMsg(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
            throw std::runtime_error("Pixel shader compilation failed: " + errorMsg);
        }
        throw std::runtime_error("Pixel shader compilation failed or file not found");
    }

    // Define vertex input layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // Create root signature
    D3D12_ROOT_PARAMETER rootParam{
		.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV,
        .Descriptor = {
            .ShaderRegister = 0,
            .RegisterSpace = 0
        },
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX
    };

    D3D12_ROOT_SIGNATURE_DESC rootSigDesc{
        .NumParameters = 1,
		.pParameters = &rootParam,
		.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    };

    ComPtr<ID3DBlob> rootSigBlob;
    CHECK(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, nullptr));
    CHECK(m_device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

    // Create PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{
		.pRootSignature = m_rootSignature.Get(),
        .VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() },
        .PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() },
        .BlendState = {
            .AlphaToCoverageEnable = FALSE,
            .IndependentBlendEnable = FALSE,
            .RenderTarget = {
                {
                    .BlendEnable = FALSE,
                    .LogicOpEnable = FALSE,
                    .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
                }
            }
        },
        .SampleMask = UINT_MAX,
        .RasterizerState = {
            .FillMode = D3D12_FILL_MODE_SOLID,
            .CullMode = D3D12_CULL_MODE_BACK,
            .FrontCounterClockwise = FALSE
        },
        .DepthStencilState = {
            .DepthEnable = FALSE, // to be true
            .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = D3D12_COMPARISON_FUNC_LESS,
            .StencilEnable = FALSE
        },
        .InputLayout = { inputLayout, _countof(inputLayout) },
        .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        .NumRenderTargets = 1,
        .RTVFormats = { DXGI_FORMAT_R8G8B8A8_UNORM },
        .DSVFormat = DXGI_FORMAT_UNKNOWN,
		.SampleDesc = { 1, 0 }
    };

    CHECK(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void Renderer::CreateConstantBuffer() {
    const UINT bufferSize = ((sizeof(ConstantBuffer) + 255) & ~255) * MAX_OBJECTS; // Align to 256 bytes

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resourceDesc{
		.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Width = bufferSize,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = { 1, 0 },
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		.Flags = D3D12_RESOURCE_FLAG_NONE
    };

    CHECK(m_device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_constantBuffer)));

    D3D12_RANGE readRange{ 0, 0 };
    CHECK(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstantBufferData)));
}

void Renderer::RenderMesh(const Mesh& mesh, const Transform& transform) {
    // Update constant buffer

    XMMATRIX world = transform.GetWorldMatrix();
    XMMATRIX worldViewProj = XMMatrixTranspose(world * m_camera.m_viewMatrix * m_camera.m_projMatrix);

    // Write into the correct slot
    constexpr UINT alignedSize = (sizeof(ConstantBuffer) + 255) & ~255;
    auto* slot = reinterpret_cast<ConstantBuffer*>(reinterpret_cast<uint8_t*>(m_pConstantBufferData) + m_currentObjectIndex * alignedSize);
    slot->worldViewProj = worldViewProj;

    // Offset the GPU address to match
    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_constantBuffer->GetGPUVirtualAddress() + m_currentObjectIndex * alignedSize;

    // Set constant buffer
    m_cmdList->SetGraphicsRootConstantBufferView(0, cbAddress);

    // Set vertex and index buffers
    D3D12_VERTEX_BUFFER_VIEW vbView = mesh.GetVertexBufferView();
    D3D12_INDEX_BUFFER_VIEW ibView = mesh.GetIndexBufferView();
    m_cmdList->IASetVertexBuffers(0, 1, &vbView);
    m_cmdList->IASetIndexBuffer(&ibView);
    m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Draw
    m_cmdList->DrawIndexedInstanced(mesh.GetIndexCount(), 1, 0, 0, 0);
	m_currentObjectIndex++;
}

void Renderer::Shutdown() {
    WaitForGPU();
    if (m_pConstantBufferData) {
        m_constantBuffer->Unmap(0, nullptr);
        m_pConstantBufferData = nullptr;
    }
    if (m_fenceEvent) CloseHandle(m_fenceEvent);
}

void Renderer::SetCameraTarget(Transform* target) {
    m_camera.SetTarget(target);
}