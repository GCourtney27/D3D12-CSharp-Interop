#pragma once

#include "Core.h"

class RENDER_API RenderContext
{
public:
	struct WindowProps
	{
		void* hWindow;
		unsigned int WindowWidth = 600U;
		unsigned int WindowHeight = 400U;
	};
public:
	RenderContext() {}
	virtual ~RenderContext() {}

	static RenderContext* Get() { return s_Instance; }

	static bool Init(void* WindowHandle) { return s_Instance->Init_Impl(WindowHandle); }
	static void RenderFrame() { s_Instance->RenderFrame_Impl(); }

	static void GetRenderState() { s_Instance->GetRenderState_Impl(); }

protected:
	virtual void GetRenderState_Impl() = 0;
	virtual bool Init_Impl(void* hWnd) = 0;
	virtual void RenderFrame_Impl() = 0;
protected:
	WindowProps m_WindowProps;
private:
	static RenderContext* s_Instance;
};
