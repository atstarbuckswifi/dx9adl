/*
  D3DxConsole.h
*/

#ifndef __D3DxConsole_H__
#define __D3DxConsole_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <windows.h>

#ifndef __DCPORT
#ifdef __D3DxConsole_MAKE_DLL__
#define __DCPORT __declspec(dllexport) // make dll mode
#else // ndef __D3DxConsole_MAKE_DLL__
#define __DCPORT __declspec(dllimport) // use dll mode
#endif // __D3DxConsole_MAKE_DLL__
#endif // __DCPORT

__DCPORT BOOL APIENTRY DllMain(HINSTANCE, DWORD, LPVOID);

__DCPORT void debugalloc();
__DCPORT void debugout(char *fmt, ...);
__DCPORT void debugfree();

#endif // __D3DxConsole_H__
