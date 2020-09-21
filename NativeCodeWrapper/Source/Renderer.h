#pragma once
#include "DX12Renderer.h"

#pragma comment (lib, "CPPLib.lib")

using namespace System;
using namespace System::Runtime::InteropServices;

namespace NativeCodeWrapper {

	public ref class Renderer
	{
	public:
		Renderer() {}

		void SendMessage(String^ Msg)
		{
			DX12Renderer Instance;
			char* str = (char*)(void*)Marshal::StringToHGlobalAnsi(Msg);
			Instance.PrintMessage(str);
		}
	};
}