/*
  dx9test0001.c

  # This file must be saved .c
  # must #define CINTERFACE before #include <d3d9.h> and use ->lpVtbl->

  # pre release dlls
    >copy "C:\Windows\SysWOW64\d3d9.dll" .
    >copy "d3d11\d3dx9.dll" .
    >\dm\bin\implib _dmc_d3d9.lib _dmc_d3d9_partial.def
    >\dm\bin\implib _dmc_d3dx9.lib _dmc_d3dx9_partial.def
    # compile
    >\dm\bin\dmc dx9test0001.c winmm.lib D3DxConsole.lib D3DxTextureBmp.lib \
      dx9adl.lib _dmc_d3d9.lib _dmc_d3dx9.lib \
      -I dxsdk\include
*/

#include "dx9adl.h"

#if 0
#define CLI_WIDTH (200 * 4)
#define CLI_HEIGHT (200 * 3)
#else
#define CLI_WIDTH (64 * 16)
#define CLI_HEIGHT (64 * 9)
#endif
char szClassName[] = "Direct3D Test Class";
char szAppName[] = "Direct3D Test";

DWORD TXSRC = 0, TXDST = 1;

BOOL InitD3DItems(RENDERD3DITEMSSTATE *pIS)
{
  debugout("callback InitD3DItems\n");
  D3DXCreateTextureFromFileA(FNDD(pIS), "..\\res\\_col_4.png",
    (LPDIRECT3DTEXTURE9 *)PPU(pIS, TXSRC));
  D3DXTXB_CreateTexture(FNDD(pIS), 256, 256, PPU(pIS, TXDST));
  return TRUE;
}

BOOL CleanupD3DItems(RENDERD3DITEMSSTATE *pIS)
{
  debugout("callback CleanupD3DItems\n");
  return TRUE;
}

BOOL RenderD3DItems(RENDERD3DITEMSSTATE *pIS)
{
  if(pIS->stat & 0x00008000){
    LPD3DXSPRITE *ppSprite = (LPD3DXSPRITE *)PPS(pIS, PSO_SPRITE);

    D3DXTXB_RewriteTexture(PPU(pIS, TXDST), PPU(pIS, TXSRC));
    BltTexture(pIS, 0xFFFFFFFF, (LPDIRECT3DTEXTURE9 *)PPU(pIS, TXDST),
      0, 0, 256, 256,
      0., 0., 0., 10., 100., 1.);
    BltTexture(pIS, 0xFFFFFFFF, (LPDIRECT3DTEXTURE9 *)PPS(pIS, PSO_STRING),
      0, 0, 512, 512,
      0., 0., 0., 10., 10., .5);
    BltString(pIS, 0xFF808080, "BLTSTRING", 3, 192, 32, 0.1);
  }else{
    D9FOUNDATION *d9f = pIS->d9fnd;
    Q_D3DMATRIX *menv = d9f->pMenv;

    D3DXMatrixRotationX(menv->world, pIS->nowTime * 0.06f * M_PI / 180.0f);
    SetupMatrices(pIS);
    DrawAxis(pIS);
  }
  return TRUE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR sCmdLine, int nShow)
{
  RENDERD3DITEMSSTATE istat = {0};
  istat.width = CLI_WIDTH, istat.height = CLI_HEIGHT;
  istat.bgc = D3DCOLOR_ARGB(192, 32, 32, 32);
  istat.fgc = D3DCOLOR_ARGB(255, 192, 192, 192);
  istat.mode = 1 | 0x0CC00000;
  InitD3DApp(hInst, nShow, szClassName, szAppName,
    &istat, InitD3DItems, CleanupD3DItems, RenderD3DItems);
  return MsgLoop(&istat);
}
