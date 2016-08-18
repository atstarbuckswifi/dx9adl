/*
  D3DxConsole.dll

  D3DxConsole.c

  # make D3DxConsole.dll
    >\dm\bin\dmc -WD -L/implib D3DxConsole.c kernel32.lib
    >copy D3DxConsole.dll ..
    >copy D3DxConsole.lib ..
    >copy D3DxConsole.h ..
*/

#define __D3DxConsole_MAKE_DLL__
#include "D3DxConsole.h"

#if 1 // when dmc and windows
__DCPORT void debugalloc()
{
  FLOAT dummy_float = 1.234; // abort BUG 'Floating point not loaded' no static
  AllocConsole();
}
__DCPORT void debugout(char *fmt, ...)
{
  DWORD r;
  char buf[1024];
  va_list arg;
  va_start(arg, fmt);
  vsprintf(buf, fmt, arg);
  WriteConsole(GetStdHandle(STD_ERROR_HANDLE), buf, strlen(buf), &r, NULL);
  va_end(arg);
}
__DCPORT void debugfree()
{
  FreeConsole();
}
#else
__DCPORT void debugalloc()
{
  // dummy
}
__DCPORT void debugout(char *fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  vfprintf(stderr, fmt, arg);
  va_end(arg);
}
__DCPORT void debugfree()
{
  // dummy
}
#endif

BOOL APIENTRY DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
  switch(reason){
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  default:
    break;
  }
  return TRUE;
}
