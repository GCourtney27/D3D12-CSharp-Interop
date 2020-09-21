#pragma once
#include <atlstr.h>
#ifdef LIBRARY_EXPORTS
	#define DX_API __declspec(dllexport)
#else
	#define DX_API __declspec(dllimport)
#endif

class DX_API DX12Renderer
{
public:
	DX12Renderer();

	void PrintMessage(char* Msg)
	{
		CString Message = Msg;
		MessageBox(NULL, Message, L"Test Msg Box", MB_OK);
	}
};

