/*
  D3DxTextureBmp.dll

  D3DxTextureBmp.h
*/

#ifndef __D3DXTEXTUREBMP_H__
#define __D3DXTEXTUREBMP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <windows.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "D3DxFT2_types.h"

#ifndef __DTPORT
#ifdef __D3DXTEXTUREBMP_MAKE_DLL__
#define __DTPORT __declspec(dllexport) // make dll mode
#else // ndef __D3DXTEXTUREBMP_MAKE_DLL__
#define __DTPORT __declspec(dllimport) // use dll mode
#endif // __D3DXTEXTUREBMP_MAKE_DLL__
#endif // __DTPORT

__DTPORT BOOL APIENTRY DllMain(HINSTANCE, DWORD, LPVOID);

// LPDIRECT3DDEVICE9, LPDIRECT3DTEXTURE9 *
__DTPORT BOOL D3DXTXB_CreateTexture(VOID *pDev,
  unsigned int w, unsigned int h, VOID **ppTexture);
// LPDIRECT3DTEXTURE9 *, D3DLOCKED_RECT *
__DTPORT BOOL D3DXTXB_LockRect(VOID **ppTexture, D3DLOCKED_RECT *pRect);
// LPDIRECT3DTEXTURE9 *
__DTPORT BOOL D3DXTXB_UnlockRect(VOID **ppTexture);
__DTPORT BOOL D3DXTXB_RenderFontGlyph(GLYPH_TBL *gt,
  FT_Bitmap *bmp, FT_Int x, FT_Int y);
// D3DLOCKED_RECT *
__DTPORT BOOL D3DXTXB_CopyTexture(VOID *sRect, VOID *dRect, int tofloat);
// LPDIRECT3DTEXTURE9 *
__DTPORT BOOL D3DXTXB_RewriteTexture(VOID **ppTexture, VOID **ppTextureSrc);

#endif // __D3DXTEXTUREBMP_H__
