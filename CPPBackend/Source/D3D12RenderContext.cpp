#include "pch.h"

#include "D3D12RenderContext.h"

constexpr inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
		throw std::runtime_error("HRESULT Failed!");
}

DX12Renderer::DX12Renderer()
{

}

void DX12Renderer::Init(HWND WindowHandle)
{

}

void DX12Renderer::Update(float DeltaMs)
{
}

bool DX12Renderer::Init_Impl(void* hWnd)
{
	UINT FactoryFlags = 0u;

	// Enable debug layers if in debug builds
//#if defined _DEBUG
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
//#endif

	HRESULT hr = CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&m_pDxgiFactory));
	ThrowIfFailed(hr);

	CreateDevice();
	CreateCommandQueue();
	CreateSwapChain();
	CreateViewPort();
	CreateScissorRect();

	return true;
}

void DX12Renderer::RenderFrame_Impl()
{
	m_pCommandAllocator->Reset();
	m_pCommandList->Reset(m_pCommandAllocator.Get(), m_pPipelineState.Get());

	//m_pCommandList->ClearRenderTargetView()

}

void DX12Renderer::GetPhysicalAdapter()
{
	ComPtr<IDXGIAdapter1> pAdapter;
	m_pAdapter = nullptr;
	DXGI_ADAPTER_DESC1 Desc = {};

	for (UINT AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_pDxgiFactory->EnumAdapters1(AdapterIndex, &pAdapter); ++AdapterIndex)
	{
		Desc = {};
		pAdapter->GetDesc1(&Desc);

		// Make sure we get the video card that is not a software adapter
		if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		// See if D3D 12 is supported and create a device
		if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr))) 
		{
			if (m_pAdapter != nullptr) 
				m_pAdapter->Release();
			
			m_pAdapter = pAdapter.Detach();
		}
	}
}

void DX12Renderer::CreateDevice()
{
	GetPhysicalAdapter();

	HRESULT hr = D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice));
	ThrowIfFailed(hr);
}

void DX12Renderer::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hr = m_pDevice->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&m_pCommandQueue));
	ThrowIfFailed(hr);
}

void DX12Renderer::CreateSwapChain()
{
	HRESULT hr;

	DXGI_MODE_DESC backBufferDesc = {};
	backBufferDesc.Width = m_WindowProps.WindowWidth;
	backBufferDesc.Height = m_WindowProps.WindowHeight;

	m_SampleDesc = {};
	m_SampleDesc.Count = 1;

	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
	SwapChainDesc.BufferCount = m_FrameBufferCount;
	SwapChainDesc.Width = m_WindowProps.WindowWidth;
	SwapChainDesc.Height = m_WindowProps.WindowHeight;
	SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	SwapChainDesc.SampleDesc = m_SampleDesc;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> TempSwapChain = {};
	hr = m_pDxgiFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), *((HWND*)m_WindowProps.hWindow), &SwapChainDesc, nullptr, nullptr, &TempSwapChain);
	ThrowIfFailed(hr);

	ThrowIfFailed(TempSwapChain.As(&m_pSwapChain));

	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void DX12Renderer::CreateViewPort()
{
	m_Client_ViewPort.TopLeftX = 0;
	m_Client_ViewPort.TopLeftY = 0;
	m_Client_ViewPort.Width = static_cast<FLOAT>(m_WindowProps.WindowWidth);
	m_Client_ViewPort.Height = static_cast<FLOAT>(m_WindowProps.WindowHeight);
	m_Client_ViewPort.MinDepth = 0.0f;
	m_Client_ViewPort.MaxDepth = 1.0f;
}

void DX12Renderer::CreateScissorRect()
{
	m_Client_ScissorRect.left = 0;
	m_Client_ScissorRect.top = 0;
	m_Client_ScissorRect.right = m_WindowProps.WindowWidth;
	m_Client_ScissorRect.bottom = m_WindowProps.WindowHeight;
}
