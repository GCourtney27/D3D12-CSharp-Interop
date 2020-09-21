#pragma once

#if defined LIBRARY_EXPORTS
	#define RENDER_API __declspec(dllexport)
#else
	#define RENDER_API __declspec(dllimport)
#endif
