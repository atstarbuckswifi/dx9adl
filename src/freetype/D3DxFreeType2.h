/*
  D3DxFreeType2.dll

  D3DxFreeType2.h
*/

#ifndef __D3DxFreeType2_H__
#define __D3DxFreeType2_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <windows.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "D3DxFT2_types.h"

// #define GLYPH_DEBUG
#define GLYPH_CMX 1024
#define GLYPH_BMX 4096

#ifndef __FTPORT
#ifdef __D3DxFreeType2_MAKE_DLL__
#define __FTPORT __declspec(dllexport) // make dll mode
#else // ndef __D3DxFreeType2_MAKE_DLL__
#define __FTPORT __declspec(dllimport) // use dll mode
#endif // __D3DxFreeType2_MAKE_DLL__
#endif // __FTPORT

__FTPORT BOOL APIENTRY DllMain(HINSTANCE, DWORD, LPVOID);

__FTPORT int D3DXFT2_MoveToFunc(const FT_Vector *to, void *u);
__FTPORT int D3DXFT2_LineToFunc(const FT_Vector *to, void *u);
__FTPORT int D3DXFT2_ConicToFunc(
  const FT_Vector *control, const FT_Vector *to, void *u);
__FTPORT int D3DXFT2_CubicToFunc(
  const FT_Vector *control1, const FT_Vector *control2,
  const FT_Vector *to, void *u);
__FTPORT BOOL D3DXFT2_GlyphContours(GLYPH_TBL *gt); // dummy
__FTPORT BOOL D3DXFT2_GlyphAlloc(GLYPH_TBL *gt, size_t cmx, size_t bmx);
__FTPORT BOOL D3DXFT2_GlyphFree(GLYPH_TBL *gt);
__FTPORT BOOL D3DXFT2_GlyphBmp(GLYPH_TBL *gt,
  FT_Bitmap *bmp, FT_Int x, FT_Int y);
__FTPORT BOOL D3DXFT2_GlyphShowBmp(GLYPH_TBL *gt);
__FTPORT DWORD D3DXFT2_GlyphOutline(GLYPH_TBL *gt);
__FTPORT DWORD D3DXFT2_Status(GLYPH_TBL *gt);

#endif // __D3DxFreeType2_H__
