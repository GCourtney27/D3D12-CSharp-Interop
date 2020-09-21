#pragma once

#include "RenderContext.h"

#pragma comment (lib, "CPPLib.lib")

using namespace System;
using namespace System::Runtime::InteropServices;

namespace NativeCodeWrapper {

	public ref class Renderer
	{
	public:
		Renderer() {}

		void Init(IntPtr hWnd)
		{
			static bool RendererCreated = false;
			if (!RendererCreated)
			{
				RenderContext::Get()->Init(hWnd.ToPointer());
			}
		}

		void RenderFrame()
		{
			RenderContext::Get()->RenderFrame();

		}

		void SendMessage(String^ Msg)
		{
			/*DX12Renderer Instance;
			char* str = (char*)(void*)Marshal::StringToHGlobalAnsi(Msg);
			Instance.PrintMessage(str);*/
		}
	};
}