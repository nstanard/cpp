#pragma once

#include "renderer/IRenderer.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <cstdint>

using Microsoft::WRL::ComPtr;

class DX12Renderer : public IRenderer {
public:
    DX12Renderer()  = default;
    ~DX12Renderer() override = default;

    bool Init(void* windowHandle) override;
    void BeginFrame() override;
    void DrawMesh(const Mesh* mesh) override;
    void EndFrame()   override;
    void Shutdown()   override;

private:
    static constexpr uint32_t FRAME_COUNT = 2;
    
    ComPtr<ID3D12Device>              m_device;
    ComPtr<ID3D12CommandQueue>        m_commandQueue;
    ComPtr<IDXGISwapChain3>           m_swapChain;
    ComPtr<ID3D12DescriptorHeap>      m_rtvHeap;
    ComPtr<ID3D12Resource>            m_renderTargets[FRAME_COUNT];
    ComPtr<ID3D12CommandAllocator>    m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12RootSignature>       m_rootSignature;
    ComPtr<ID3D12PipelineState>       m_pipelineState;
    ComPtr<ID3D12Resource>            m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW          m_vertexBufferView;
    
    ComPtr<ID3D12Fence>               m_fence;
    uint64_t                          m_fenceValue = 0;
    HANDLE                            m_fenceEvent = nullptr;
    
    uint32_t                          m_rtvDescriptorSize = 0;
    uint32_t                          m_frameIndex = 0;
    uint32_t                          m_width = 1280;
    uint32_t                          m_height = 720;
    
    void WaitForGPU();
};
