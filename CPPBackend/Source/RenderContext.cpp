#include "pch.h"

#include "RenderContext.h"

#include "D3D12RenderContext.h"

RenderContext* RenderContext::s_Instance = new DX12Renderer();
