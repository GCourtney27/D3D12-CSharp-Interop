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
	m_WindowProps.hWindow = hWnd;

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
	CreateRTVs();
	CreateCommandLists();
	CreateSyncObjects();
	
	return true;
}

void DX12Renderer::RenderFrame_Impl()
{
	m_pCommandAllocators[m_FrameIndex]->Reset();
	m_pCommandList->Reset(m_pCommandAllocators[m_FrameIndex].Get(), m_pPipelineState.Get());

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RTVDescriptorIncrementSize);

	m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	HRESULT hr = m_pCommandList->Close();
	ThrowIfFailed(hr);

	// Execute
	ID3D12CommandList* ppCommandLists[] =
	{
		m_pCommandList.Get()
	};
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	
	WaitForGPU();

	hr = m_pSwapChain->Present(0, NULL);
	ThrowIfFailed(hr);
	
	MoveToNextFrame();
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

void DX12Renderer::CreateRTVs()
{
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.NumDescriptors = m_FrameBufferCount;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = m_pDevice->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_pRTVHeap));
	ThrowIfFailed(hr);

	m_RTVDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < m_FrameBufferCount; i++)
	{
		hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]));
		ThrowIfFailed(hr);

		m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);

		rtvHandle.Offset(1, m_RTVDescriptorIncrementSize);
	}
}

void DX12Renderer::CreateCommandLists()
{
	HRESULT hr;
	for (UINT i = 0; i < m_FrameBufferCount; ++i)
	{
		hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocators[i]));
		ThrowIfFailed(hr);
	}
	hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pCommandList));
	ThrowIfFailed(hr);
	m_pCommandList->Close();
}

void DX12Renderer::CreateSyncObjects()
{
	HRESULT hr;
	hr = m_pDevice->CreateFence(m_FenceValues[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	ThrowIfFailed(hr);
	m_FenceValues[m_FrameIndex]++;

	m_FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_FenceEvent == nullptr)
		throw std::runtime_error("Failed to create fence event.");
}

void DX12Renderer::WaitForGPU()
{
	HRESULT hr;
	// Schedule a Signal command in the queue.
	hr = m_pCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]);
	ThrowIfFailed(hr);

	// Wait until the fence has been processed.
	hr = m_pFence.Get()->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
	ThrowIfFailed(hr);

	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	m_FenceValues[m_FrameIndex]++;
}

void DX12Renderer::MoveToNextFrame()
{
	HRESULT hr;

	// Schedule a Signal command in the queue.
	const UINT64 currentFenceValue = m_FenceValues[m_FrameIndex];
	hr = m_pCommandQueue->Signal(m_pFence.Get(), currentFenceValue);
	ThrowIfFailed(hr);

	// Advance the frame index.
	m_FrameIndex = (m_FrameIndex + 1) % m_FrameBufferCount;

	// Check to see if the next frame is ready to start.
	if (m_pFence->GetCompletedValue() < m_FenceValues[m_FrameIndex])
	{
		hr = m_pFence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
		ThrowIfFailed(hr);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	m_FenceValues[m_FrameIndex] = currentFenceValue + 1;
}
