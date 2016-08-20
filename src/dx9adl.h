/*
  dx9adl.dll

  dx9adl.h

  anti device lost and additional dx9 library

  winmm.lib
  .\FreeType\freetype.lib
*/

#ifndef __DX9ADL_H__
#define __DX9ADL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>

// 0: not use, 1: 1ms
#define USE_TIME_BEGIN_PERIOD 0

#define USE_TRIANGLE_LIST
// #define USE_TRIANGLE_STRIP

// #define FONT_WITH_GDI32
// #define FONT_WITH_DRAWTEXT

#ifndef __D9PORT
#ifdef __DX9ADL_MAKE_DLL__
#define __D9PORT __declspec(dllexport) // make dll mode
#else // ndef __DX9ADL_MAKE_DLL__
#define __D9PORT __declspec(dllimport) // use dll mode
#endif // __DX9ADL_MAKE_DLL__
#endif // __D9PORT

#define ReleaseNilVI(P) ReleaseNil_Inline(&(VOID *)(P))
#define ReleaseNilV(P) ReleaseNil(&(VOID *)(P))
#define CIC(Fnc, Obj) (Obj)->lpVtbl->Fnc(Obj)
#define CI(Fnc, Obj, ...) (Obj)->lpVtbl->Fnc((Obj), ## __VA_ARGS__)
#define CINTERFACE
// #define sqrtf sqrt
// #define sinf sin
// #define cosf cos
// #define tanf tan
// typedef unsigned long long UINT64;
typedef ULONGLONG UINT64;
typedef ULONG UINT32;
typedef ULONG SIZE_T;
typedef ULONG UINT_PTR;
#include <d3d9.h>
// to include d3dx9.h all, funcs in d3dx9math.inl must be encapsulated into dll
#include <d3dx9.h>

#define IMG_STRING "..\\res\\_string.png" // 600 x 96
#define IMG_STRING_W 512
#define IMG_STRING_H 512

// #define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#ifndef VTX
typedef struct _CUSTOMVERTEX_ {
  D3DVECTOR v; // D3DXVECTOR3 v; // struct { FLOAT x, y, z; } v;
  DWORD color;
  FLOAT tu, tv; // texture
} CUSTOMVERTEX;

#define VTX(r, vx, vy, vz, c, ptu, ptv) do{ \
  r.v.x = (vx), r.v.y = (vy), r.v.z = (vz), r.color = (c); \
  r.tu = (ptu), r.tv = (ptv); \
}while(0)
#endif

// #define D3DFVF_CUSTOMCOR (D3DFVF_XYZ | D3DFVF_NORMAL)
#define D3DFVF_CUSTOMCOR (D3DFVF_XYZ | D3DFVF_DIFFUSE)

#ifndef COR
typedef struct _CUSTOMCOR_ {
  D3DVECTOR v; // D3DXVECTOR3 v; // struct { FLOAT x, y, z; } v;
  DWORD color;
} CUSTOMCOR;

#define COR(r, vx, vy, vz, c) do{ \
  r.v.x = (vx), r.v.y = (vy), r.v.z = (vz), r.color = (c); \
}while(0)
#endif

#ifndef CUV
typedef union _CUSTOMCUV_ {
  CUSTOMCOR c;
  CUSTOMVERTEX v;
} CUSTOMCUV;

#define CUV(r, vx, vy, vz, uc) do{ \
  r.c.v.x = (vx), r.c.v.y = (vy), r.c.v.z = (vz), r.c.color = (uc); \
  r.v.tu = 0.0f, r.v.tv = 0.0f; \
}while(0)
#endif

// sizeof(QUATERNIONIC_MATRIX) == sizeof(D3DMATRIX) == sizeof(D3DXMATRIXA16)
typedef struct _Q_D3DMATRIX_ {
  D3DMATRIX *tmp;
  D3DMATRIX *world;
  D3DMATRIX *view;
  D3DMATRIX *proj;
} Q_D3DMATRIX;

typedef struct _D9F_VECS_ {
  D3DVECTOR eyePt; // D3DXVECTOR3 v; // struct { FLOAT x, y, z; } v;
  DWORD reserved0;
  D3DVECTOR lookatPt; // D3DXVECTOR3 v; // struct { FLOAT x, y, z; } v;
  DWORD reserved1;
  D3DVECTOR upVec; // D3DXVECTOR3 v; // struct { FLOAT x, y, z; } v;
  DWORD reserved2;
  FLOAT fovY, aspect, zn, zf; // for D3DXMatrixPerspectiveFovLH
} D9F_VECS;

typedef struct _D9FOUNDATION_ {
  D3DPRESENT_PARAMETERS *pD3Dpp;
  LPDIRECT3D9 pD3D;
  LPDIRECT3DDEVICE9 pDev;
  VOID *pMenv; // pointer to Q_D3DMATRIX Menv = {&tmp, &world, &view, &proj};
  VOID *pVecs; // pointer to D9F_VECS (construction values of matrices)
  char *imstring; // in
  DWORD imw, imh; // in
} D9FOUNDATION;

typedef struct _RENDERD3DITEMSSTATE_ {
  VOID *parent; // in
  D9FOUNDATION *d9fnd; // out-in
  VOID **sysChain; // out(-in)
  VOID **usrChain; // out(-in)
  DWORD smx, umx; // out(-in)
  DWORD width, height; // in
  DWORD bgc, fgc; // in
  DWORD mode; // in
  DWORD stat; // out
  HWND hWnd; // out
  DWORD fps, prevTime, nowTime; // out
} RENDERD3DITEMSSTATE;

#define FNDPD(P) (&(LPDIRECT3DDEVICE9)(P)->d9fnd->pDev)
#define FNDD(P) ((LPDIRECT3DDEVICE9)(P)->d9fnd->pDev)

#define OCHAIN_SMX 0x0040
#define OCHAIN_UMX 0x4000

#define PSO_D3D     0 // LPDIRECT3D9
#define PSO_DEV     1 // LPDIRECT3DDEVICE9
#define PSO_SPRITE  2 // LPD3DXSPRITE
#define PSO_FONT    3 // LPD3DXFONT
#define PSO_STRING  4 // LPDIRECT3DTEXTURE9
#define PSO_STRVBUF 5 // LPDIRECT3DVERTEXBUFFER9

#define PPS(P, S) (&(P)->sysChain[S])
#define PSO(P, S) ((P)->sysChain[S])
#define PPU(P, U) (&(P)->usrChain[U])
#define PUO(P, U) ((P)->usrChain[U])

#define FNDPS(P) ((LPD3DXSPRITE *)PPS(P, PSO_SPRITE))
#define FNDS(P) ((LPD3DXSPRITE)PSO(P, PSO_SPRITE))
#define FNDPF(P) ((LPD3DXFONT *)PPS(P, PSO_FONT))
#define FNDF(P) ((LPD3DXFONT)PSO(P, PSO_FONT))

typedef BOOL (*CALLBACKINITD3DITEMS)(RENDERD3DITEMSSTATE *pIS);
typedef BOOL (*CALLBACKRESETD3DITEMS)(RENDERD3DITEMSSTATE *pIS);
typedef BOOL (*CALLBACKRELEASED3DITEMS)(RENDERD3DITEMSSTATE *pIS);
typedef BOOL (*CALLBACKCLEANUPD3DITEMS)(RENDERD3DITEMSSTATE *pIS);
typedef BOOL (*CALLBACKRENDERD3DITEMS)(RENDERD3DITEMSSTATE *pIS);

__D9PORT BOOL APIENTRY DllMain(HINSTANCE, DWORD, LPVOID);

__D9PORT BOOL ReleaseNil(VOID **p);
__D9PORT VOID **PtrPtrS(RENDERD3DITEMSSTATE *pIS, DWORD id);
__D9PORT VOID *PtrSO(RENDERD3DITEMSSTATE *pIS, DWORD id);
__D9PORT VOID **PtrPtrU(RENDERD3DITEMSSTATE *pIS, DWORD id);
__D9PORT VOID *PtrUO(RENDERD3DITEMSSTATE *pIS, DWORD id);
BOOL InitDirect3D(RENDERD3DITEMSSTATE *pIS);
BOOL ResetDirect3D(RENDERD3DITEMSSTATE *pIS);
BOOL ReleaseDirect3D(RENDERD3DITEMSSTATE *pIS);
BOOL CleanupDirect3D(RENDERD3DITEMSSTATE *pIS);
__D9PORT BOOL SetupMatrices(RENDERD3DITEMSSTATE *pIS);
__D9PORT BOOL DrawAxis(RENDERD3DITEMSSTATE *pIS);
// LPDIRECT3DDEVICE9, LPDIRECT3DVERTEXBUFFER9 *
__D9PORT BOOL DX9_CreateVertexBuffer(VOID *pDev, size_t sz, VOID **ppVtxBuf);
// LPDIRECT3DVERTEXBUFFER9 *, VOID **
__D9PORT BOOL DX9_Lock(VOID **ppVtxBuf, size_t sz, VOID **ppLckVertices);
// LPDIRECT3DVERTEXBUFFER9 *
__D9PORT BOOL DX9_Unlock(VOID **ppVtxBuf);
// LPDIRECT3DVERTEXBUFFER9 *, CUSTOMVERTEX *
__D9PORT BOOL DX9_StoreVertices(VOID **ppVtxBuf, size_t sz, VOID *pVertices);
// CUSTOMVERTEX *, LPDIRECT3DVERTEXBUFFER9 *
__D9PORT BOOL DX9_LoadVertices(VOID *pVertices, size_t sz, VOID **ppVtxBuf);
// LPDIRECT3DDEVICE9, LPDIRECT3DTEXTURE9 *
__D9PORT BOOL DX9_SetTexture(VOID *pDev, VOID **ppTexture, int mode);
// LPDIRECT3DDEVICE9, LPDIRECT3DVERTEXBUFFER9 *
__D9PORT BOOL DX9_RenderVertices(VOID *pDev, VOID **ppVtxBuf, size_t sz,
  int num, int mode);
int CharGeometries(RENDERD3DITEMSSTATE *pIS, DWORD col, char *s, int n,
  float sx, float sy, float sz, float ox, float oy, float oz);
__D9PORT BOOL DrawString(RENDERD3DITEMSSTATE *pIS, DWORD col, char *s, int n,
  float sx, float sy, float sz, float ox, float oy, float oz);
__D9PORT BOOL BltString(RENDERD3DITEMSSTATE *pIS, DWORD col, char *s, int n,
  int ox, int oy, float oz);
__D9PORT BOOL BltTexture(RENDERD3DITEMSSTATE *pIS, DWORD col,
  LPDIRECT3DTEXTURE9 *ppTexture, int left, int top, int right, int bottom,
  double cx, double cy, double cz, double px, double py, double pz);
BOOL RenderDirect3D(RENDERD3DITEMSSTATE *pIS);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
__D9PORT BOOL InitD3DApp(HINSTANCE hInst, int nShow,
  char *szClassName, char *szAppName,
  RENDERD3DITEMSSTATE *pIS,
  CALLBACKINITD3DITEMS cbInitD3DItems,
  CALLBACKCLEANUPD3DITEMS cbCleanupD3DItems,
  CALLBACKRENDERD3DITEMS cbRenderD3DItems);
__D9PORT DWORD MsgLoop(RENDERD3DITEMSSTATE *pIS);
__D9PORT DWORD DX9_Mode(RENDERD3DITEMSSTATE *pIS);
__D9PORT DWORD DX9_Status(RENDERD3DITEMSSTATE *pIS);

#endif // __DX9ADL_H__
