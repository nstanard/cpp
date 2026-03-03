#include "renderer/dx12/DX12Renderer.h"
#include "Mesh.h"
#include <iostream>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

bool DX12Renderer::Init(void* windowHandle) {
    std::cout << "[DX12] Initializing...\n";

    // windowHandle is an HWND — no GLFW translation needed
    HWND hwnd = static_cast<HWND>(windowHandle);

    #ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
        }
    }
    #endif

    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)))) {
        std::cerr << "[DX12] Failed to create DXGI factory\n";
        return false;
    }

    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)))) {
        std::cerr << "[DX12] Failed to create device\n";
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    if (FAILED(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)))) {
        std::cerr << "[DX12] Failed to create command queue\n";
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width       = m_width;
    swapChainDesc.Height      = m_height;
    swapChainDesc.Format      = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = FRAME_COUNT;
    swapChainDesc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    ComPtr<IDXGISwapChain1> swapChain1;
    if (FAILED(factory->CreateSwapChainForHwnd(m_commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1))) {
        std::cerr << "[DX12] Failed to create swap chain\n";
        return false;
    }

    swapChain1.As(&m_swapChain);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FRAME_COUNT;
    rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)))) {
        std::cerr << "[DX12] Failed to create RTV heap\n";
        return false;
    }

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])))) {
            std::cerr << "[DX12] Failed to get swap chain buffer\n";
            return false;
        }
        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.ptr += m_rtvDescriptorSize;
    }

    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)))) {
        std::cerr << "[DX12] Failed to create command allocator\n";
        return false;
    }

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error))) {
        if (error) std::cerr << "[DX12] Root signature error: " << (char*)error->GetBufferPointer() << "\n";
        return false;
    }

    if (FAILED(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)))) {
        std::cerr << "[DX12] Failed to create root signature\n";
        return false;
    }

    const char* vertexShaderSource = R"(
        struct VSInput { float3 position : POSITION; float4 color : COLOR; };
        struct PSInput { float4 position : SV_POSITION; float4 color : COLOR; };
        PSInput main(VSInput input) {
            PSInput output;
            output.position = float4(input.position, 1.0f);
            output.color    = input.color;
            return output;
        }
    )";

    const char* pixelShaderSource = R"(
        struct PSInput { float4 position : SV_POSITION; float4 color : COLOR; };
        float4 main(PSInput input) : SV_TARGET { return input.color; }
    )";

    ComPtr<ID3DBlob> vertexShader, pixelShader, shaderError;

    if (FAILED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShader, &shaderError))) {
        if (shaderError) std::cerr << "[DX12] VS error: " << (char*)shaderError->GetBufferPointer() << "\n";
        return false;
    }

    if (FAILED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShader, &shaderError))) {
        if (shaderError) std::cerr << "[DX12] PS error: " << (char*)shaderError->GetBufferPointer() << "\n";
        return false;
    }

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout                                = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature                             = m_rootSignature.Get();
    psoDesc.VS                                         = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
    psoDesc.PS                                         = { pixelShader->GetBufferPointer(),  pixelShader->GetBufferSize()  };
    psoDesc.RasterizerState.FillMode                   = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode                   = D3D12_CULL_MODE_NONE;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    psoDesc.DepthStencilState.DepthEnable              = FALSE;
    psoDesc.DepthStencilState.StencilEnable            = FALSE;
    psoDesc.SampleMask                                 = UINT_MAX;
    psoDesc.PrimitiveTopologyType                      = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets                           = 1;
    psoDesc.RTVFormats[0]                              = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count                           = 1;

    if (FAILED(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)))) {
        std::cerr << "[DX12] Failed to create PSO\n";
        return false;
    }

    if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)))) {
        std::cerr << "[DX12] Failed to create command list\n";
        return false;
    }
    m_commandList->Close();

    const UINT maxVertexBufferSize = sizeof(Vertex) * 1024;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width            = maxVertexBufferSize;
    resourceDesc.Height           = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels        = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (FAILED(m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer)))) {
        std::cerr << "[DX12] Failed to create vertex buffer\n";
        return false;
    }

    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes  = sizeof(Vertex);

    if (FAILED(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))) {
        std::cerr << "[DX12] Failed to create fence\n";
        return false;
    }
    m_fenceValue = 1;

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_fenceEvent) {
        std::cerr << "[DX12] Failed to create fence event\n";
        return false;
    }

    std::cout << "[DX12] Initialization complete\n";
    return true;
}

void DX12Renderer::BeginFrame() {
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get());

    D3D12_VIEWPORT viewport = {};
    viewport.Width    = static_cast<float>(m_width);
    viewport.Height   = static_cast<float>(m_height);
    viewport.MaxDepth = 1.0f;

    D3D12_RECT scissorRect = {};
    scissorRect.right  = m_width;
    scissorRect.bottom = m_height;

    m_commandList->RSSetViewports(1, &viewport);
    m_commandList->RSSetScissorRects(1, &scissorRect);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource   = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += m_frameIndex * m_rtvDescriptorSize;

    const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void DX12Renderer::DrawMesh(const Mesh* mesh) {
    const UINT byteSize = sizeof(Vertex) * mesh->GetVertexCount();
    void* pData = nullptr;
    m_vertexBuffer->Map(0, nullptr, &pData);
    memcpy(pData, mesh->GetVertices(), byteSize);
    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexBufferView.SizeInBytes = byteSize;

    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->DrawInstanced(mesh->GetVertexCount(), 1, 0, 0);
}

void DX12Renderer::EndFrame() {
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource   = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);
    m_commandList->Close();

    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    m_swapChain->Present(1, 0);
    WaitForGPU();
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void DX12Renderer::Shutdown() {
    std::cout << "[DX12] Shutting down...\n";
    WaitForGPU();
    if (m_fenceEvent) {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }
    std::cout << "[DX12] Shutdown complete\n";
}

void DX12Renderer::WaitForGPU() {
    const uint64_t fence = m_fenceValue;
    m_commandQueue->Signal(m_fence.Get(), fence);
    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fence) {
        m_fence->SetEventOnCompletion(fence, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}
