/*
  dx9sample.c

  # This file must be saved .c
  # must #define CINTERFACE before #include <d3d9.h> and use ->lpVtbl->

  # pre release dlls
    >copy "C:\Windows\SysWOW64\d3d9.dll" .
    >copy "d3d11\d3dx9.dll" .
    >\dm\bin\implib _dmc_d3d9.lib _dmc_d3d9_partial.def
    >\dm\bin\implib _dmc_d3dx9.lib _dmc_d3dx9_partial.def
    >copy .\FreeType\freetype.lib .
    >copy .\FreeType\D3DxConsole.dll .
    >copy .\FreeType\D3DxConsole.lib .
    >copy .\FreeType\D3DxConsole.h ..
    >copy .\FreeType\D3DxFreeType2.dll .
    >copy .\FreeType\D3DxFreeType2.lib .
    >... # make D3DxTextureBmp.dll and .lib
    >... # make dx9adl.dll and .lib
    >... # make D3DxGlyph.dll and .lib
    # compile
    >\dm\bin\dmc dx9sample.c winmm.lib _dmc_d3d9.lib _dmc_d3dx9.lib \
      D3DxConsole.lib D3DxFreeType2.lib D3DxTextureBmp.lib \
      dx9adl.lib D3DxGlyph.lib \
      -I dxsdk\include \
      -I FreeType\freetype-2.4.11-custom-windll\include -I FreeType -D_DLL
*/

#include "dx9adl.h"
#include "quaternion.h"

#include "D3DxFreeType2.h"
#include "D3DxGlyph.h"
#include "D3DxTextureBmp.h"
#include "D3DxConsole.h"

#define FNT_FACE "FreeType\\mikaP.ttf"
// #define FNT_FACE "FreeType\\SourceCodePro-Regular.ttf"
// #define FNT_FACE "FreeType\\InconsolataGo-Regular.ttf"
// #define FNT_FACE "FreeType\\InconsolataGo-Bold.ttf"
// #define FNT_FACE "FreeType\\Inconsolata.otf"

#define IMG_SAMPLE "res\\_Nashinoki_Shrine.png"
#define IMG_COL4 "res\\_col_4.png"
#define IMG_D_00 "res\\_D_00.png"
#define IMG_D_01 "res\\_D_01.dds" // "res\\_D_01.png"
#define IMG_D_02 "res\\_D_02.png"
#define IMG_D_go "res\\_D_go.png"

#if 0
#define CLI_WIDTH (200 * 4)
#define CLI_HEIGHT (200 * 3)
#else
#define CLI_WIDTH (64 * 16)
#define CLI_HEIGHT (64 * 9)
#endif
char szClassName[] = "Direct3D Sample Class";
char szAppName[] = "Direct3D Sample";

DWORD TXSRC = 0, TXDST = 1, VtxGlp = 2, VtxBuf = 3;
DWORD VtxFan[] = {8, 9}; // id of LPDIRECT3DVERTEXBUFFER9

#define VTXFAN_ELEMS 24

QUATERNIONIC_MATRIX m_transform = {
  1.0,  0.,  0.,  0.,
   0., 1.0,  0.,  0.,
   0.,  0., 1.0,  0.,
   0.,  0.,  0., 1.0};
QUATERNIONIC_MATRIX m_rotation = {
  1.0,         0.,         0.,  0.,
   0.,  1./1.4142,  1./1.4142,  0.,
   0., -1./1.4142,  1./1.4142,  0.,
   0.,         0.,         0., 1.0};
QUATERNIONIC_MATRIX m_scale = {
  1.5,  0.,  0.,  0.,
   0., 1.5,  0.,  0.,
   0.,  0., 1.5,  0.,
   0.,  0.,  0., 1.0};
QUATERNIONIC_MATRIX m_translate = {
  1.0,  0.,  0.,  0.,
   0., 1.0,  0.,  0.,
   0.,  0., 1.0,  0.,
  -4., -1., -2., 1.0};
QQMATRIX qqm = {NULL, NULL, NULL, NULL}; // re-set later
VERTEX_GLYPH vg = {NULL, NULL, NULL, 0}; // re-set later
GLYPH_VTX gv = {NULL, 0, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0}; // re-set later
GLYPH_TBL gt = {NULL, NULL, FNT_FACE, NULL, 6000.f, 25.f, 0, 0, // re-set later
  0, 256, 256, 256, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

// #define TEST_LIST
// #define TEST_STRIP
#define TEST_FAN

BOOL RewriteTexGlyph(RENDERD3DITEMSSTATE *pIS)
{
  D3DLOCKED_RECT dstrc;

  D3DXTXB_RewriteTexture(PPU(pIS, TXDST), PPU(pIS, TXSRC));
  if(pIS->nowTime - pIS->prevTime <= 1000){ // 100 (1000 fast when full screen)
    double t = (75. - 60. * ((pIS->nowTime >> 4) % 256) / 256) * M_PI / 180;
    wchar_t s[1024];

    gt.pIS = pIS;
    qqm.transform = &m_transform;
    qqm.rotation = &m_rotation;
    qqm.scale = &m_scale;
    qqm.translate = &m_translate;
    vg.pQQM = &qqm;
    vg.ppTexture = PPU(pIS, TXDST);
    vg.ppVtxGlyph = PPU(pIS, VtxGlp);
    ReleaseNil(vg.ppVtxGlyph); // (VOID **)GLPPV(&vg)
    vg.szGlyph = 0;
    gt.pVG = &vg;
    // gt.facename = FNT_FACE;
#if 0
#if 0
    _swprintf(s, L"%02d %08X", pIS->fps, pIS->nowTime); // fast 58-60fps
#else
    _swprintf(s, L"%c%02d%s", 0x3042, pIS->fps, L"\u3041\u3046"); // 45-58fps ?
#endif
#else
    _swprintf(s, L"%02d%c%04X%s", pIS->fps, 0x3041, // 0x4E80 \u1F422
      pIS->nowTime & 0x0000FFFF, L"\u3047\u3046"); // fast 58-60fps full screen
#endif
    gt.utxt = s;
    // gt.ratio = 6000.f;
    // gt.angle = 25.f;
    gt.mode = 2 | (((pIS->nowTime >> 12) % 2) ? 4 : 8); // about 4.0sec (>M_PI)
    // gt.mode |= 0x40000000; // no texture
    gt.td = 32 * (((pIS->nowTime >> 4) % 256) + 1); // about 0.8sec
    D3DXTXB_LockRect(PPU(pIS, TXDST), &dstrc);
    gt.rct = (CUSTOMRECT *)&dstrc;
    gt.glyphBmp = D3DXTXB_RenderFontGlyph;
    m_rotation.cc = m_rotation.bb = cos(t);
    m_rotation.bc = - (m_rotation.cb = sin(t));
    Q3M(m_transform, m_rotation, m_scale, m_translate);
    gt.matrix = qqm.transform; // must re-initialize
    gv.col = pIS->fgc;
    gv.bgc = pIS->bgc;
    gt.vtx = &gv; // must re-initialize
    gt.funcs = NULL; // must re-initialize
    gt.glyphContours = D3DXGLP_GlyphContours;
    gt.glyphAlloc = D3DXFT2_GlyphAlloc;
    gt.glyphFree = D3DXFT2_GlyphFree;
    D3DXFT2_GlyphOutline(&gt);
    D3DXTXB_UnlockRect(PPU(pIS, TXDST));
  }
  return TRUE;
}

BOOL InitGeometry(RENDERD3DITEMSSTATE *pIS)
{
#ifdef TEST_LIST
  CUSTOMVERTEX Vertices[] = { // LIST(uv) CULL NONE or CW
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 0
    { 0.0f,  1.0f,  0.1f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.1f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 1
    { 0.0f,  0.9f,  0.2f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.2f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 2
    { 0.0f,  0.8f,  0.3f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.3f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 3
    { 0.0f,  0.7f,  0.4f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.4f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 4
    { 0.0f,  0.6f,  0.5f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.5f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 5
    { 0.0f,  0.5f,  0.6f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.6f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 6
    { 0.0f,  0.4f,  0.7f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.7f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 7
    { 0.0f,  0.3f,  0.8f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.8f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 8
    { 0.0f,  0.2f,  0.9f, 0xFFFFFF80, 0.5f, 0.0f},
    { 0.9f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f},
    { 0.0f,  0.0f,  0.0f, 0xFFFF80FF, 1.0f, 1.0f}, // 9
    { 0.0f,  0.1f,  1.0f, 0xFFFFFF80, 0.5f, 0.0f},
    { 1.0f,  0.0f,  0.0f, 0xFF80FFFF, 0.0f, 1.0f}
  };
#else
#ifndef TEST_FAN
  CUSTOMVERTEX Vertices[] = { // STRIP(uv) CULL NONE or CW
    {-1.0f, -1.0f,  3.0f, 0xFFFF80FF, 0.0f, 1.0f},
    { 0.0f,  1.0f,  1.0f, 0xFFFFFF80, 0.0f, 0.0f},
    { 1.0f, -1.0f,  0.0f, 0x8080FFFF, 0.5f, 1.0f},
    { 0.0f, -3.0f, -1.0f, 0x80FF80FF, 0.5f, 0.0f},
    {-1.0f, -3.0f, -2.0f, 0x00FFFF80, 1.0f, 1.0f},
    {-2.0f, -2.0f, -4.0f, 0x0080FFFF, 1.0f, 0.0f}
  };
#else
  CUSTOMVERTEX Vertices[] = { // FAN(uv) CULL CW or NONE
    {-1.0f, -1.0f,  3.0f, 0xFFFF80FF, 0.0f, 1.0f},
    { 0.0f,  1.0f,  1.0f, 0xFFFFFF80, 0.0f, 0.0f},
    { 1.0f, -1.0f,  0.0f, 0x8080FFFF, 0.5f, 0.0f},
    { 0.0f, -3.0f, -1.0f, 0x80FF80FF, 1.0f, 0.0f},
    {-1.0f, -3.0f, -2.0f, 0x00FFFF80, 1.0f, 0.5f},
    {-2.0f, -2.0f, -4.0f, 0x0080FFFF, 1.0f, 1.0f}
  };
#endif
#endif
  size_t blen = sizeof(Vertices); // N * sizeof(CUSTOMVERTEX)
  if(!DX9_CreateVertexBuffer(FNDD(pIS), blen, PPU(pIS, VtxBuf))){
    MessageBox(NULL, "Can't create vertex buffer.", "Error", MB_OK);
    return FALSE;
  }
  DX9_StoreVertices(PPU(pIS, VtxBuf), blen, Vertices);
  return TRUE;
}

BOOL DrawPoly(RENDERD3DITEMSSTATE *pIS)
{
  DX9_SetTexture(FNDD(pIS), PPU(pIS, TXDST), 0);
#ifdef TEST_LIST
  DX9_RenderVertices(FNDD(pIS), PPU(pIS, VtxBuf), sizeof(CUSTOMVERTEX),
    10, 0x00000060);
#else
#ifndef TEST_FAN
  DX9_RenderVertices(FNDD(pIS), PPU(pIS, VtxBuf), sizeof(CUSTOMVERTEX),
    4, 0x00000020); // 0x00000022 0x00000021
#else
  DX9_RenderVertices(FNDD(pIS), PPU(pIS, VtxBuf), sizeof(CUSTOMVERTEX),
    4, 0x00000041); // 0x00000040
#endif
#endif
  return TRUE;
}

BOOL InitFans(RENDERD3DITEMSSTATE *pIS)
{
  CUSTOMVERTEX Vertices[VTXFAN_ELEMS + 2]; // FAN(uv) CULL CW/CCW or NONE
  size_t blen = sizeof(Vertices); // N * sizeof(CUSTOMVERTEX)
  int n;

  for(n = sizeof(VtxFan) / sizeof(VtxFan[0]); --n >= 0; ){
    int e, d = 1 - 2 * n;
    FLOAT ox = 0.0f, oy = 0.0f, oz = 0.0f;
    FLOAT rx = 1.0f, ry = 1.0f, rz = 0.3f;
    FLOAT ux = 0.5f, uy = 0.5f, ur = 0.5f;
    if(n){ rx /= 2, ry *= 2; }
    // VTX(Vertices[0], ox, oy, oz, 0xFFFF80FF, ux, uy);
    // VTX(Vertices[0], ox - d * 1.5f, oy, oz, 0xFFFF80FF, ux, uy);
    // VTX(Vertices[0], ox, oy - d * 1.5f, oz, 0xFFFF80FF, ux, uy);
    VTX(Vertices[0], ox, oy, oz - d * 1.5f, 0xFFFF80FF, ux, uy);
    for(e = 0; e < VTXFAN_ELEMS/2; ++e){
      FLOAT t = d * M_PI * 2 * e / VTXFAN_ELEMS;
      FLOAT px = rx * cos(t), py = ry * sin(t), pz = d * rz * sin(t);
      FLOAT vx = ur * cos(t), vy = - ur * sin(t);
      DWORD c0 = 0xFFFFFFFF, c1 = 0xFFFFFFFF;
      VTX(Vertices[1+e], ox+px, oy+py, oz+pz, c0, ux+vx, uy+vy);
      VTX(Vertices[1+e+VTXFAN_ELEMS/2], ox-px, oy-py, oz-pz, c1, ux-vx, uy-vy);
    }
    // memcpy(&Vertices[1+VTXFAN_ELEMS], &Vertices[1], sizeof(CUSTOMVERTEX));
    Vertices[1+VTXFAN_ELEMS] = Vertices[1]; // sharedVtx
    if(!DX9_CreateVertexBuffer(FNDD(pIS), blen, PPU(pIS, VtxFan[n]))){
      MessageBox(NULL, "Can't create vertex FAN.", "Error", MB_OK);
      return FALSE;
    }
    DX9_StoreVertices(PPU(pIS, VtxFan[n]), blen, Vertices);
  }
  return TRUE;
}

BOOL DrawFans(RENDERD3DITEMSSTATE *pIS)
{
  int n;

  DX9_SetTexture(FNDD(pIS), PPU(pIS, TXDST), 0);
  for(n = sizeof(VtxFan) / sizeof(VtxFan[0]); --n >= 0; ){
    DX9_RenderVertices(FNDD(pIS), PPU(pIS, VtxFan[1-n]), sizeof(CUSTOMVERTEX),
      VTXFAN_ELEMS, 0x00000040); // 0x00000041
  }
  return TRUE;
}

BOOL InitD3DItems(RENDERD3DITEMSSTATE *pIS)
{
  LPDIRECT3DTEXTURE9 *ppSrc = (LPDIRECT3DTEXTURE9 *)PPU(pIS, TXSRC);
//  D3DXCreateTextureFromFile(FNDD(pIS), IMG_SAMPLE, ppSrc);
//  D3DXCreateTextureFromFile(FNDD(pIS), IMG_COL4, ppSrc);
//  D3DXCreateTextureFromFile(FNDD(pIS), IMG_D_00, ppSrc);
  D3DXCreateTextureFromFile(FNDD(pIS), IMG_D_01, ppSrc);
//  D3DXCreateTextureFromFile(FNDD(pIS), IMG_D_02, ppSrc);
//  D3DXCreateTextureFromFile(FNDD(pIS), IMG_D_go, ppSrc);
  D3DXTXB_CreateTexture(FNDD(pIS), 256, 256, PPU(pIS, TXDST));
  if(!InitGeometry(pIS)) return FALSE;
  if(!InitFans(pIS)) return FALSE;
  return TRUE;
}

BOOL CleanupD3DItems(RENDERD3DITEMSSTATE *pIS)
{
//  ReleaseNilV(...);
  return TRUE;
}

BOOL RenderD3DItems(RENDERD3DITEMSSTATE *pIS)
{
  if(pIS->stat & 0x00008000){
#if 1
    UINT32 c = 0xFFFFFFFF;
    LPDIRECT3DTEXTURE9 *p = (LPDIRECT3DTEXTURE9 *)PPU(pIS, TXDST);

    BltTexture(pIS, c, p, 128, 0, 256, 128, 0.f, 0.f, 0.f, 20.f, 20.f, 0.1f);
    BltTexture(pIS, c, p, 0, 128, 128, 256, 0.f, 0.f, 0.f, 128.f, 64.f, 0.4f);
    BltTexture(pIS, c, p, 0, 0, 128, 128, 0.f, 0.f, 0.f, 64.f, 128.f, 0.9f);
    BltTexture(pIS, c, p, 0, 0, 256, 256, 0.f, 0.f, 0.f, 500.f, 300.f, 1.0f);
#endif
  }else{
#if 1
    D9FOUNDATION *d9f = pIS->d9fnd;
    Q_D3DMATRIX *menv = d9f->pMenv;

    D3DXMatrixRotationY(menv->world, pIS->nowTime * 0.06f * M_PI / 180.0f);
    SetupMatrices(pIS);
    DrawAxis(pIS);
    RewriteTexGlyph(pIS);
    DrawPoly(pIS);
    DrawFans(pIS);
    D3DXGLP_DrawGlyph(&gt);
#endif
  }
  return TRUE;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR sCmdLine, int nShow)
{
  RENDERD3DITEMSSTATE istat = {0};
  istat.width = CLI_WIDTH, istat.height = CLI_HEIGHT;
  istat.bgc = D3DCOLOR_ARGB(192, 240, 192, 32);
  istat.fgc = D3DCOLOR_ARGB(255, 96, 192, 96);
  istat.mode = 1 | 0x0CC00000;
  InitD3DApp(hInst, nShow, szClassName, szAppName,
    &istat, InitD3DItems, CleanupD3DItems, RenderD3DItems);
  return MsgLoop(&istat);
}
