/*
  D3DxGlyph.dll

  D3DxGlyph.h
*/

#ifndef __D3DXGLYPH_H__
#define __D3DXGLYPH_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <windows.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "D3DxFT2_types.h"

#ifndef __DGPORT
#ifdef __D3DXGLYPH_MAKE_DLL__
#define __DGPORT __declspec(dllexport) // make dll mode
#else // ndef __D3DXGLYPH_MAKE_DLL__
#define __DGPORT __declspec(dllimport) // use dll mode
#endif // __D3DXGLYPH_MAKE_DLL__
#endif // __DGPORT

#ifndef GLPS
typedef struct _VERTEX_GLYPH_ {
  VOID *pQQM; // QQMATRIX *
  VOID **ppTexture; // LPDIRECT3DTEXTURE9 *
  VOID **ppVtxGlyph; // LPDIRECT3DVERTEXBUFFER9 *
  size_t szGlyph;
} VERTEX_GLYPH;

#define GLPPT(G) ((LPDIRECT3DTEXTURE9 *)(G)->ppTexture)
#define GLPT(G) (*(LPDIRECT3DTEXTURE9 *)(G)->ppTexture)
#define GLPPV(G) ((LPDIRECT3DVERTEXBUFFER9 *)(G)->ppVtxGlyph)
#define GLPV(G) (*(LPDIRECT3DVERTEXBUFFER9 *)(G)->ppVtxGlyph)
#define GLPS(G) ((G)->szGlyph)
#endif

__DGPORT BOOL APIENTRY DllMain(HINSTANCE, DWORD, LPVOID);

__DGPORT BOOL D3DXGLP_InitFont();
__DGPORT BOOL D3DXGLP_CleanupFont();
__DGPORT BOOL D3DXGLP_GlyphContours(GLYPH_TBL *gt);
__DGPORT BOOL D3DXGLP_DrawGlyph(GLYPH_TBL *gt);

#endif // __D3DXGLYPH_H__
