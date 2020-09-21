#pragma once
#include <atlstr.h>

#include "Core.h"

#include "RenderContext.h"

using Microsoft::WRL::ComPtr;

class RENDER_API DX12Renderer : public RenderContext
{
public:
	DX12Renderer();

	void Init(HWND WindowHandle);

	void Update(float DeltaMs);

	virtual bool Init_Impl(void* hWnd) override;
	virtual void RenderFrame_Impl() override;

	virtual void GetRenderState_Impl() override
	{
		CString Message = "Hello From DX12 context";
		MessageBox(NULL, Message, L"Test Msg Box", MB_OK);
	}

private:
	void GetPhysicalAdapter();
	void CreateDevice();
	void CreateCommandQueue();
	void CreateSwapChain();
	void CreateViewPort();
	void CreateScissorRect();
	void CreateRTVs();
	void CreateCommandLists();
	void CreateSyncObjects();

	void WaitForGPU();
	void MoveToNextFrame();
protected:
	static const uint8_t m_FrameBufferCount = 3;
	D3D12_VIEWPORT m_Client_ViewPort;
	D3D12_RECT m_Client_ScissorRect;
	UINT m_RTVDescriptorIncrementSize;

	// CPU/GPU Syncronization
	int						m_FrameIndex = 0;
	UINT64					m_FenceValues[m_FrameBufferCount] = {};
	HANDLE					m_FenceEvent = {};
	ComPtr<ID3D12Fence>		m_pFence;

	ComPtr<ID3D12Device> m_pDevice;
	ComPtr<IDXGIAdapter1> m_pAdapter;
	ComPtr<IDXGIFactory4> m_pDxgiFactory;

	ComPtr<IDXGISwapChain3> m_pSwapChain;
	ComPtr<ID3D12Resource> m_pRenderTargets[m_FrameBufferCount];

	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
	ComPtr<ID3D12CommandAllocator> m_pCommandAllocators[m_FrameBufferCount];
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	ComPtr<ID3D12PipelineState> m_pPipelineState;
	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;

	DXGI_SAMPLE_DESC m_SampleDesc = {};

	// Resources
	ComPtr<ID3D12Resource> m_pVertexBuffer;
};

