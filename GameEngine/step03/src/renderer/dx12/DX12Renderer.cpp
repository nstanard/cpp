// Stub — no <d3d12.h> yet. Real DX12 calls come in a future step.

#include "renderer/dx12/DX12Renderer.h"
#include <iostream>

bool DX12Renderer::Init()       { std::cout << "[DX12] Init\n";       return true; }
void DX12Renderer::BeginFrame() {              }
void DX12Renderer::EndFrame()   {                }
void DX12Renderer::Shutdown()   { std::cout << "[DX12] Shutdown\n";                }
