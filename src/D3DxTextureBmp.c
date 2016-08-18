/*
  D3DxTextureBmp.dll

  D3DxTextureBmp.c

    >\dm\bin\dmc -WD -L/implib D3DxTextureBmp.c D3DxConsole.lib \
      kernel32.lib _dmc_d3d9.lib _dmc_d3dx9.lib \
      -I dxsdk\include \
      -I FreeType\freetype-2.4.11-custom-windll\include -I FreeType -D_DLL
*/

#include "dx9adl.h"

#define __D3DXTEXTUREBMP_MAKE_DLL__
#include "D3DxTextureBmp.h"
#include "D3DxConsole.h"

// #define TEXTURE_FLOAT
// #define TEXTURE_GRAD
#define TEXTURE_MIXBG
#define TEXTURE_ALPHA

// LPDIRECT3DDEVICE9, LPDIRECT3DTEXTURE9 *
__DTPORT BOOL D3DXTXB_CreateTexture(VOID *pDev,
  unsigned int w, unsigned int h, VOID **ppTexture)
{
#ifndef TEXTURE_FLOAT
  D3DFORMAT fmt = D3DFMT_A8R8G8B8;
#else
  D3DFORMAT fmt = D3DFMT_A32B32G32R32F;
#endif
  // to CreateTexture with
  //  0, D3DPOOL_MANAGED
  //  D3DUSAGE_DYNAMIC (resize ok), D3DPOOL_DEFAULT
  //  D3DUSAGE_RENDERTARGET (write ok), D3DPOOL_DEFAULT
  CI(CreateTexture, (LPDIRECT3DDEVICE9)pDev,
    w, h, 1, 0, // D3DUSAGE_RENDERTARGET ?
    fmt, D3DPOOL_MANAGED, // D3DPOOL_DEFAULT ?
    (LPDIRECT3DTEXTURE9 *)ppTexture, NULL);
  return TRUE;
}

// LPDIRECT3DTEXTURE9 *, D3DLOCKED_RECT *
__DTPORT BOOL D3DXTXB_LockRect(VOID **ppTexture, D3DLOCKED_RECT *pRect)
{
  CI(LockRect, *(LPDIRECT3DTEXTURE9 *)ppTexture, 0, pRect, NULL, 0);
  return TRUE;
}

// LPDIRECT3DTEXTURE9 *
__DTPORT BOOL D3DXTXB_UnlockRect(VOID **ppTexture)
{
  CI(UnlockRect, *(LPDIRECT3DTEXTURE9 *)ppTexture, 0);
  return TRUE;
}

__DTPORT BOOL D3DXTXB_RenderFontGlyph(GLYPH_TBL *gt,
  FT_Bitmap *bmp, FT_Int x, FT_Int y)
{
  int tw = gt->tw, th = gt->th;
  CUSTOMRECT *lrc = gt->rct;
  FT_Int i, j, p, q;
  FT_Int x_max = x + bmp->width;
  FT_Int y_max = y + bmp->rows;

  for(j = y, q = 0; j < y_max; j++, q++){
#ifndef TEXTURE_FLOAT
    LPDWORD data = (LPDWORD)((LPBYTE)lrc->pBits + j * lrc->Pitch);
#else
    FLOAT *data = (FLOAT *)((LPBYTE)lrc->pBits + j * lrc->Pitch);
#endif
    for(i = x, p = 0; i < x_max; i++, p++){
      if(i < 0 || j < 0 || i >= tw || j >= th) continue;
      BYTE c = bmp->buffer[q * bmp->width + p];
#ifndef TEXTURE_FLOAT
      BYTE a = (data[i] >> 24) & 0x0FF;
      BYTE r = (data[i] >> 16) & 0x0FF;
      BYTE g = (data[i] >> 8) & 0x0FF;
      BYTE b = data[i] & 0x0FF;
      BYTE na = a, nr = r, ng = g, nb = b;
      if(c){
        nr ^= 0xFF, ng ^= 0xFF, nb ^= 0xFF;
        if(c < 128) nr = (nr + r) / 2, ng = (ng + r) / 2, nb = (nb + r) / 2;
        a = na, r = nr, g = nb, b = nb;
      }
      data[i] = (a << 24) | (r << 16) | (g << 8) | b;
#else
      FLOAT r = data[i * 4 + 0];
      FLOAT g = data[i * 4 + 1];
      FLOAT b = data[i * 4 + 2];
      FLOAT a = data[i * 4 + 3];
      FLOAT na = a, nr = r, ng = g, nb = b;
      if(c){
        nr = 1.0 - nr, ng = 1.0 - ng, nb = 1.0 - nb;
        if(c < 128) nr = (nr + r) / 2, ng = (ng + r) / 2, nb = (nb + r) / 2;
        a = na, r = nr, g = nb, b = nb;
      }
      data[i * 4 + 0] = r;
      data[i * 4 + 1] = g;
      data[i * 4 + 2] = b;
      data[i * 4 + 3] = a;
#endif
    }
  }
  return TRUE;
}

// D3DLOCKED_RECT *
__DTPORT BOOL D3DXTXB_CopyTexture(VOID *sRect, VOID *dRect, int tofloat)
{
  D3DLOCKED_RECT *src = sRect;
  D3DLOCKED_RECT *dst = dRect;
  int x, y;

  for(y = 256; --y >= 0; ){
#ifndef TEXTURE_FLOAT
    LPDWORD pSrc = (LPDWORD)((LPBYTE)src->pBits + y * src->Pitch);
    LPDWORD pDst = (LPDWORD)((LPBYTE)dst->pBits + y * dst->Pitch);
    memcpy(pDst, pSrc, dst->Pitch >= src->Pitch ? src->Pitch : dst->Pitch);
#else
    if(tofloat == 1){ // src(A8R8G8B8) -> dst(A32B32G32R32F)
      LPDWORD pSrc = (LPDWORD)((LPBYTE)src->pBits + y * src->Pitch);
      FLOAT *pDst = (FLOAT *)((LPBYTE)dst->pBits + y * dst->Pitch);
      for(x = 0; x < 256; ++x){
        DWORD d = *pSrc++;
        pDst[x * 4 + 0] = ((d >> 16) & 0x0FF) / 255.0f;
        pDst[x * 4 + 1] = ((d >> 8) & 0x0FF) / 255.0f;
        pDst[x * 4 + 2] = (d & 0x0FF) / 255.0f;
        pDst[x * 4 + 3] = ((d >> 24) & 0x0FF) / 255.0f;
      }
    }else{ // 0: src(A32B32G32R32F) -> dst(A8R8G8B8)
      FLOAT *pSrc = (FLOAT *)((LPBYTE)src->pBits + y * src->Pitch);
      LPDWORD pDst = (LPDWORD)((LPBYTE)dst->pBits + y * dst->Pitch);
      for(x = 0; x < 256; ++x){
        BYTE r = (BYTE)(pSrc[x * 4 + 0] * 255.0f);
        BYTE g = (BYTE)(pSrc[x * 4 + 1] * 255.0f);
        BYTE b = (BYTE)(pSrc[x * 4 + 2] * 255.0f);
        BYTE a = (BYTE)(pSrc[x * 4 + 3] * 255.0f);
        *pDst++ = (a << 24) | (r << 16) | (g << 8) | b;
      }
    }
#endif
  }
  return TRUE;
}

// LPDIRECT3DTEXTURE9 *
__DTPORT BOOL D3DXTXB_RewriteTexture(VOID **ppTexture, VOID **ppTextureSrc)
{
  LPDIRECT3DTEXTURE9 *ppTex = (LPDIRECT3DTEXTURE9 *)ppTexture;
  LPDIRECT3DTEXTURE9 *ppTexSrc = (LPDIRECT3DTEXTURE9 *)ppTextureSrc;
  D3DLOCKED_RECT srcrc, dstrc;
  int x, y;

  CI(LockRect, *ppTex, 0, &dstrc, NULL, 0);
  CI(LockRect, *ppTexSrc, 0, &srcrc, NULL, 0);
  D3DXTXB_CopyTexture(&srcrc, &dstrc, 1);
  CI(UnlockRect, *ppTexSrc, 0);
if(1){ // slow !!! (but fast when full screen)
  for(y = 256; --y >= 0; ){
#ifndef TEXTURE_FLOAT
    LPDWORD data = (LPDWORD)((LPBYTE)dstrc.pBits + y * dstrc.Pitch);
#else
    FLOAT *fdata = (FLOAT *)((LPBYTE)dstrc.pBits + y * dstrc.Pitch);
#endif
    for(x = 0; x < 256; ++x){
#ifndef TEXTURE_GRAD
      BYTE a = 192, b = 32, g = 192 * y / 255, r = 240 * x / 255;
#ifndef TEXTURE_FLOAT
#ifndef TEXTURE_MIXBG
      // *data++ = (a << 24) | (r << 16) | (g << 8) | b;
      // *data++ &= (a << 24) | (r << 16) | (g << 8) | b;
      // *data++ ^= (a << 24) | (r << 16) | (g << 8) | b;
      *data++ &= 0x00FFFFFF ^ ((a << 24) | (r << 16) | (g << 8) | b);
#else
      BYTE oa = (*data >> 24) & 0x0FF;
      BYTE or = (*data >> 16) & 0x0FF;
      BYTE og = (*data >> 8) & 0x0FF;
      BYTE ob = *data & 0x0FF;
#ifndef TEXTURE_ALPHA
      *data = oa * a / 255;
#else
      *data = (oa ? oa : 255) * a * x / 255 / 255;
#endif
      *data = (*data << 8) | (or * (0x0FF ^ r) / 255);
      *data = (*data << 8) | (og * (0x0FF ^ g) / 255);
      *data = (*data << 8) | (ob * (0x0FF ^ b) / 255);
      ++data;
#endif
#else
#ifndef TEXTURE_MIXBG
      fdata[0] = ((BYTE)(fdata[0] * 255.0f) & (0x0FF ^ r)) / 255.0f;
      fdata[1] = ((BYTE)(fdata[1] * 255.0f) & (0x0FF ^ g)) / 255.0f;
      fdata[2] = ((BYTE)(fdata[2] * 255.0f) & (0x0FF ^ b)) / 255.0f;
      fdata[3] = ((BYTE)(fdata[3] * 255.0f) & a) / 255.0f;
#else
      fdata[0] *= (0x0FF ^ r) / 255.0f;
      fdata[1] *= (0x0FF ^ g) / 255.0f;
      fdata[2] *= (0x0FF ^ b) / 255.0f;
#ifndef TEXTURE_ALPHA
      fdata[3] *= a / 255.0f;
#else
      fdata[3] = (fdata[3] != 0.0f ? fdata[3] : 1.0f) * (a * x / 255) / 255.0f;
#endif
#endif
      fdata += 4;
#endif
#else
      FLOAT a = 0.88f, b = 0.25f, g = 0.75f * y / 255, r = 1.0f * x / 255;
#ifndef TEXTURE_FLOAT
      *data = (BYTE)(a * 255.0f);
      *data = (*data << 8) | (BYTE)(r * 255.0f);
      *data = (*data << 8) | (BYTE)(g * 255.0f);
      *data = (*data << 8) | (BYTE)(b * 255.0f);
      ++data;
#else
      *fdata++ = r;
      *fdata++ = g;
      *fdata++ = b;
      *fdata++ = a;
#endif
#endif
    }
  }
}
  CI(UnlockRect, *ppTex, 0);
  return TRUE;
}

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
