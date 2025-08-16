#pragma once

#ifdef NYX_BUILD_DLL
#ifdef NYX_DLL_EXPORT
#define NYX_API __declspec(dllexport)
#else
#define NYX_API __declspec(dllimport)
#endif
#endif
#ifndef NYX_API
#define NYX_API
#endif
