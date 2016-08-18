/*
  dx9adl.dll

  dx9adl.c

  anti device lost and additional dx9 library

  winmm.lib
  .\FreeType\freetype.lib

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

  # make dx9adl.dll (without FreeType2)
    >\dm\bin\dmc -WD -L/implib dx9adl.c D3DxConsole.lib \
      kernel32.lib winmm.lib _dmc_d3d9.lib _dmc_d3dx9.lib \
      -I dxsdk\include

  # make dx9adl.dll (with FreeType2)
    >\dm\bin\dmc -WD -L/implib dx9adl.c D3DxFreeType2.lib D3DxConsole.lib \
      kernel32.lib winmm.lib _dmc_d3d9.lib _dmc_d3dx9.lib \
      -I dxsdk\include \
      -I FreeType\freetype-2.4.11-custom-windll\include -I FreeType -D_DLL
*/

#define __DX9ADL_MAKE_DLL__
#include "dx9adl.h"

#include "D3DxConsole.h"

D3DMATRIX gm_tmp = {1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0., 0.,0.,0.,1.};
D3DMATRIX gm_world = {1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0., 0.,0.,0.,1.};
D3DMATRIX gm_view = {1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0., 0.,0.,0.,1.};
D3DMATRIX gm_proj = {1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0., 0.,0.,0.,1.};
Q_D3DMATRIX gm_env = {NULL, NULL, NULL, NULL};
D9F_VECS g_d9f_vecs = {5.,5.,-5.,0, 0.,0.,0.,0, 0.,1.,0.,0, 3./4.,1.,1.,100.};
D9FOUNDATION g_d9f = {NULL, NULL, NULL, NULL, NULL, NULL, 0, 0};
RENDERD3DITEMSSTATE *g_pIS = NULL;

CALLBACKINITD3DITEMS CBInitD3DItems = NULL;
CALLBACKRESETD3DITEMS CBResetD3DItems = NULL;
CALLBACKRELEASED3DITEMS CBReleaseD3DItems = NULL;
CALLBACKCLEANUPD3DITEMS CBCleanupD3DItems = NULL;
CALLBACKRENDERD3DITEMS CBRenderD3DItems = NULL;

__inline BOOL ReleaseNil_Inline(VOID **p)
{
  if(*p){
    CIC(Release, *(LPDIRECT3D9 *)p);
    *p = NULL;
  }
  return TRUE;
}

__D9PORT BOOL ReleaseNil(VOID **p)
{
  return ReleaseNil_Inline(p);
}

__D9PORT VOID **PtrPtrS(RENDERD3DITEMSSTATE *pIS, DWORD id)
{
  return PPS(pIS, id);
}

__D9PORT VOID *PtrSO(RENDERD3DITEMSSTATE *pIS, DWORD id)
{
  return PSO(pIS, id);
}

__D9PORT VOID **PtrPtrU(RENDERD3DITEMSSTATE *pIS, DWORD id)
{
  return PPU(pIS, id);
}

__D9PORT VOID *PtrUO(RENDERD3DITEMSSTATE *pIS, DWORD id)
{
  return PUO(pIS, id);
}

BOOL InitDirect3D(RENDERD3DITEMSSTATE *pIS)
{
  D9FOUNDATION *d9f = pIS->d9fnd;
  D3DPRESENT_PARAMETERS d3dpp;
  HRESULT hr;

  // timeBeginPeriod need for D3DPRESENT_INTERVAL_DEFAULT
  if(USE_TIME_BEGIN_PERIOD) timeBeginPeriod(USE_TIME_BEGIN_PERIOD);
  if(NULL == (d9f->pD3D = Direct3DCreate9(D3D_SDK_VERSION))){
    MessageBox(pIS->hWnd, "Can't create Direct3D.", "Error", MB_OK);
    return FALSE;
  }
  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  // d3dpp.FullScreen_refreshRateInHz = 75;
  d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; // rs.InVBlank
  d3dpp.BackBufferCount = 1;
  d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; // D3DFMT_X8R8G8B8 D3DFMT_UNKNOWN
  d3dpp.BackBufferWidth = pIS->width;
  d3dpp.BackBufferHeight = pIS->height;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  hr = CI(CreateDevice, d9f->pD3D,
    D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pIS->hWnd,
    D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, FNDPD(pIS));
  if(FAILED(hr)){
    hr = CI(CreateDevice, d9f->pD3D,
      D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pIS->hWnd,
      D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, FNDPD(pIS));
    if(FAILED(hr)){
      MessageBox(pIS->hWnd, "Can't create Device.", "Error", MB_OK);
      return FALSE;
    }
  }
  d9f->pD3Dpp = (D3DPRESENT_PARAMETERS *)malloc(sizeof(D3DPRESENT_PARAMETERS));
  if(!d9f->pD3Dpp){
    MessageBox(pIS->hWnd, "Can't create PresentParameters.", "Error", MB_OK);
    CleanupDirect3D(pIS);
    return FALSE;
  }
  memcpy(d9f->pD3Dpp, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));
  pIS->sysChain = (VOID **)malloc(pIS->smx * sizeof(VOID *));
  if(!pIS->sysChain){
    MessageBox(pIS->hWnd, "Can't create SysChain.", "Error", MB_OK);
    CleanupDirect3D(pIS);
    return FALSE;
  }else{
    int i;
    for(i = 0; i < pIS->smx; ++i) PSO(pIS, i) = NULL;
  }
  pIS->usrChain = (VOID **)malloc(pIS->umx * sizeof(VOID *));
  if(!pIS->usrChain){
    MessageBox(pIS->hWnd, "Can't create UsrChain.", "Error", MB_OK);
    CleanupDirect3D(pIS);
    return FALSE;
  }else{
    int i;
    for(i = 0; i < pIS->umx; ++i) PUO(pIS, i) = NULL;
  }
#if 1
  if(S_OK != D3DXCreateFont(FNDD(pIS), 48, 24,
    FW_NORMAL,
    D3DX_DEFAULT, // miplevels
    FALSE, // Italic
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    FIXED_PITCH | FF_DONTCARE, // FIXED_PITCH | FF_MODERN,
    "Lucida Console", FNDPF(pIS))){
    MessageBox(pIS->hWnd, "Can't create Font.", "Error", MB_OK);
    return FALSE;
  }
#else
  {
    D3DXFONT_DESC D3DFD;
    D3DFD.Height = 48;
    D3DFD.Width = 24;
    D3DFD.Weight = FW_NORMAL;
    D3DFD.MipLevels = D3DX_DEFAULT; // miplevels
    D3DFD.Italic = FALSE; // Italic
    D3DFD.CharSet = DEFAULT_CHARSET;
    D3DFD.OutputPrecision = OUT_DEFAULT_PRECIS;
    D3DFD.Quality = DEFAULT_QUALITY;
    D3DFD.PitchAndFamily = FIXED_PITCH | FF_DONTCARE; // | FF_MODERN;
    memset(D3DFD.FaceName, 0, sizeof(D3DFD.FaceName)); // LF_FACESIZE
    if(S_OK != D3DXCreateFontIndirect(FNDD(pIS), &D3DFD, FNDPF(pIS))){
      MessageBox(pIS->hWnd, "Can't create Font.", "Error", MB_OK);
      return FALSE;
    }
  }
#endif
  return ResetDirect3D(pIS);
}

BOOL ResetDirect3D(RENDERD3DITEMSSTATE *pIS)
{
  D9FOUNDATION *d9f = pIS->d9fnd;
  // zenable=D3DZB_FALSE
  // cull=D3DCULL_CW, D3DCULL_CCW, D3DCULL_FORCE_DWORD, D3DCULL_NONE
  DWORD zenable=TRUE, lighting=FALSE, cull=D3DCULL_CCW;

  D3DXCreateSprite(FNDD(pIS), FNDPS(pIS));
  D3DXCreateTextureFromFileA(FNDD(pIS), d9f->imstring,
    (LPDIRECT3DTEXTURE9 *)PPS(pIS, PSO_STRING));
  CI(SetRenderState, FNDD(pIS), D3DRS_ZENABLE, zenable);
  CI(SetRenderState, FNDD(pIS), D3DRS_LIGHTING, lighting);
  CI(SetRenderState, FNDD(pIS), D3DRS_CULLMODE, cull);
  if(CBResetD3DItems(pIS)){ // CBInitD3DItems(pIS)
    pIS->stat &= 0x7FFFFFFF;
    return TRUE;
  }else{
    pIS->stat |= 0x80000000;
    return FALSE;
  }
}

BOOL ReleaseDirect3D(RENDERD3DITEMSSTATE *pIS)
{
  D9FOUNDATION *d9f = pIS->d9fnd;

  CBReleaseD3DItems(pIS); // CBCleanupD3DItems(pIS);
  CI(SetRenderState, FNDD(pIS), D3DRS_CULLMODE, D3DCULL_NONE);
  CI(SetRenderState, FNDD(pIS), D3DRS_LIGHTING, FALSE);
  CI(SetRenderState, FNDD(pIS), D3DRS_ZENABLE, FALSE);
  if(pIS->usrChain){
    int i;
    for(i = pIS->umx; --i >= 0; ) ReleaseNil_Inline(PPU(pIS, i));
  }
  if(pIS->sysChain){ // skip 0: pD3D, 1: pDev
    int i;
    for(i = pIS->smx; --i >= 2; ) ReleaseNil_Inline(PPS(pIS, i));
  }
  return TRUE;
}

BOOL CleanupDirect3D(RENDERD3DITEMSSTATE *pIS)
{
  D9FOUNDATION *d9f = pIS->d9fnd;

  ReleaseDirect3D(pIS);
  if(pIS->usrChain){ free(pIS->usrChain); pIS->usrChain = NULL; }
  if(pIS->sysChain){ free(pIS->sysChain); pIS->sysChain = NULL; }
  if(d9f->pD3Dpp){ free(d9f->pD3Dpp); d9f->pD3Dpp = NULL; }
  ReleaseNilVI(FNDD(pIS));
  ReleaseNilVI(d9f->pD3D);
  if(USE_TIME_BEGIN_PERIOD) timeEndPeriod(USE_TIME_BEGIN_PERIOD);
  return TRUE;
}

BOOL SetupMatrices(RENDERD3DITEMSSTATE *pIS)
{
  D9FOUNDATION *d9f = pIS->d9fnd;
  Q_D3DMATRIX *menv = d9f->pMenv;

  CI(SetTransform, FNDD(pIS), D3DTS_WORLD, menv->world);
  CI(SetTransform, FNDD(pIS), D3DTS_VIEW, menv->view);
  CI(SetTransform, FNDD(pIS), D3DTS_PROJECTION, menv->proj);
  return TRUE;
}

BOOL DrawAxis(RENDERD3DITEMSSTATE *pIS)
{
  CUSTOMCOR xyz[] = {
    {0., 0., 0., 0xFFC00000}, {4., 0., 0., 0xFFC00000},
    {0., 0., 0., 0xFF00C000}, {0., 4., 0., 0xFF00C000},
    {0., 0., 0., 0xFF0000C0}, {0., 0., 4., 0xFF0000C0}
  };

#if 0
  CI(SetRenderState, FNDD(pIS), D3DRS_ALPHABLENDENABLE, FALSE);
  CI(SetFVF, FNDD(pIS), D3DFVF_CUSTOMCOR);
  CI(DrawPrimitiveUP, FNDD(pIS), D3DPT_LINELIST, 3, xyz, sizeof(CUSTOMCOR));
#else
  VOID *cor = xyz;

  DX9_RenderVertices(FNDD(pIS), &cor, sizeof(CUSTOMCOR), 3, 0x0000008C);
#endif
  return TRUE;
}

// LPDIRECT3DDEVICE9, LPDIRECT3DVERTEXBUFFER9 *
__D9PORT BOOL DX9_CreateVertexBuffer(VOID *pDev, size_t sz, VOID **ppVtxBuf)
{
  HRESULT hr = CI(CreateVertexBuffer, (LPDIRECT3DDEVICE9)pDev,
    sz, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT,
    (LPDIRECT3DVERTEXBUFFER9 *)ppVtxBuf, NULL);
  if(FAILED(hr)) return FALSE;
  return TRUE;
}

// LPDIRECT3DVERTEXBUFFER9 *, VOID **
__D9PORT BOOL DX9_Lock(VOID **ppVtxBuf, size_t sz, VOID **ppLckVertices)
{
  CI(Lock, *(LPDIRECT3DVERTEXBUFFER9 *)ppVtxBuf, 0, sz, ppLckVertices, 0);
  return TRUE;
}

// LPDIRECT3DVERTEXBUFFER9 *
__D9PORT BOOL DX9_Unlock(VOID **ppVtxBuf)
{
  CIC(Unlock, *(LPDIRECT3DVERTEXBUFFER9 *)ppVtxBuf);
  return TRUE;
}

// LPDIRECT3DVERTEXBUFFER9 *, CUSTOMVERTEX *
__D9PORT BOOL DX9_StoreVertices(VOID **ppVtxBuf, size_t sz, VOID *pVertices)
{
  VOID *pLckVertices;
  DX9_Lock(ppVtxBuf, sz, &pLckVertices);
  memcpy(pLckVertices, pVertices, sz);
  DX9_Unlock(ppVtxBuf);
  return TRUE;
}

// CUSTOMVERTEX *, LPDIRECT3DVERTEXBUFFER9 *
__D9PORT BOOL DX9_LoadVertices(VOID *pVertices, size_t sz, VOID **ppVtxBuf)
{
  VOID *pLckVertices;
  DX9_Lock(ppVtxBuf, sz, &pLckVertices);
  memcpy(pVertices, pLckVertices, sz);
  DX9_Unlock(ppVtxBuf);
  return TRUE;
}

// LPDIRECT3DDEVICE9, LPDIRECT3DTEXTURE9 *
__D9PORT BOOL DX9_SetTexture(VOID *pDev, VOID **ppTexture, int mode)
{
  LPDIRECT3DDEVICE9 dev = (LPDIRECT3DDEVICE9)pDev;

  CI(SetTexture, dev, 0,
    (IDirect3DBaseTexture9 *)(ppTexture ? *ppTexture : NULL));
  // (op=D3DTOP_MODULATE)->(arg1=D3DTA_TEXTURE)=(arg2=D3DTA_DIFFUSE)
  // (op=D3DTOP_SELECTARG1)->(arg1=D3DTA_TEXTURE)=(arg2=D3DTA_CURRENT)
  // (op=D3DTOP_SELECTARG1)->(arg1=D3DTA_TEXTURE)=(arg2=D3DTA_DIFFUSE)
  //CI(SetTextureStageState, dev, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
  //CI(SetTextureStageState, dev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  //CI(SetTextureStageState, dev, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  // (op=D3DTOP_MODULATE)->(arg1=D3DTA_TEXTURE)=(arg2=D3DTA_CURRENT)
  // (op=D3DTOP_MODULATE)->(arg1=D3DTA_TEXTURE)=(arg2=D3DTA_DIFFUSE)
  CI(SetTextureStageState, dev, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  CI(SetTextureStageState, dev, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  CI(SetTextureStageState, dev, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
  return TRUE;
}

// LPDIRECT3DDEVICE9, LPDIRECT3DVERTEXBUFFER9 *
__D9PORT BOOL DX9_RenderVertices(VOID *pDev, VOID **ppVtxBuf, size_t sz,
  int num, int mode)
{
  LPDIRECT3DDEVICE9 dev = (LPDIRECT3DDEVICE9)pDev;
/* mode
  31-24: 0000 0000
  23-16: 0000 0000
  15-08: 0000 0000
     07: 1 CUSTOMCOR, 0 CUSTOMVERTEX
  06-04:  11- D3DPT_TRIANGLELIST
          10- D3DPT_TRIANGLEFAN
          01- D3DPT_TRIANGLESTRIP
          001 D3DPT_LINESTRIP
          000 D3DPT_LINELIST
     03:      1 DrawPrimitiveUp, 0 DrawPrimitive
     02:       1 overwrite, 0 alpha blend
  01-00:        1- D3DCULL_CW
                01 D3DCULL_CCW
                00 D3DCULL_NONE
*/
  DWORD c = mode & 0x00000002 ? D3DCULL_CW :
    (mode & 0x00000001 ? D3DCULL_CCW : D3DCULL_NONE);
  DWORD m = (mode & 0x00000060) >> 4;
  DWORD o = m ? (m == 4 ? D3DPT_TRIANGLEFAN :
    (m == 2 ? D3DPT_TRIANGLESTRIP : D3DPT_TRIANGLELIST)) :
    (mode & 0x00000010 ? D3DPT_LINESTRIP : D3DPT_LINELIST);

  if(mode & 0x00000004){
    CI(SetRenderState, dev, D3DRS_ALPHABLENDENABLE, FALSE);
  }else{
    CI(SetRenderState, dev, D3DRS_ALPHABLENDENABLE, TRUE);
    CI(SetRenderState, dev, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    CI(SetRenderState, dev, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  }
  CI(SetRenderState, dev, D3DRS_CULLMODE, c);
  CI(SetFVF, dev, mode & 0x00000080 ? D3DFVF_CUSTOMCOR : D3DFVF_CUSTOMVERTEX);
  if(mode & 0x00000008){
    CI(DrawPrimitiveUP, dev, o, num, *ppVtxBuf, sz);
  }else{
    CI(SetStreamSource, dev, 0, *(LPDIRECT3DVERTEXBUFFER9 *)ppVtxBuf, 0, sz);
    CI(DrawPrimitive, dev, o, 0, num);
  }
  return TRUE;
}

__inline DWORD ArgbMul(DWORD s0, DWORD s1)
{
  DWORD a, r, g, b;

  a = ((s0 >> 24) & 0x0FF) * ((s1 >> 24) & 0x0FF) / 255;
  r = ((s0 >> 16) & 0x0FF) * ((s1 >> 16) & 0x0FF) / 255;
  g = ((s0 >> 8) & 0x0FF) * ((s1 >> 8) & 0x0FF) / 255;
  b = (s0 & 0x0FF) * (s1 & 0x0FF) / 255;
  a = a > 0x0FF ? 0x0FF : a;
  r = r > 0x0FF ? 0x0FF : r;
  g = g > 0x0FF ? 0x0FF : g;
  b = b > 0x0FF ? 0x0FF : b;
  return (a << 24) | (r << 16) | (g << 8) | b;
}

int CharGeometries(RENDERD3DITEMSSTATE *pIS, DWORD col, char *s, int n,
  float sx, float sy, float sz, float ox, float oy, float oz)
{
  D9FOUNDATION *d9f = pIS->d9fnd;
  CUSTOMVERTEX *vertices = NULL;
  size_t blen;
  int i, len = strlen(s);
  DWORD fWidth = 8, fHeight = 16;
  float sWidth = d9f->imw, sHeight = d9f->imh;

  if(!len) return 0;
  ReleaseNilVI(PSO(pIS, PSO_STRVBUF));
  blen = len * 6 * sizeof(CUSTOMVERTEX);
  if(!DX9_CreateVertexBuffer(FNDD(pIS), blen, PPS(pIS, PSO_STRVBUF))){
    MessageBox(NULL, "Can't create string vertex buffer.", "Error", MB_OK);
    return 0;
  }
  vertices = (CUSTOMVERTEX *)malloc(blen);
  if(!vertices) return 0;
  for(i = 0; i < len; ++i){
    // LIST(uv) or STRIP(uv) - CULL NONE or CW
    char c = s[i];
    int x = i, y = i, z = i, p = i * 6;
    float vxl = ox + x * sx;
    float vxr = ox + (x + 1) * sx;
    float vyl = oy + (y / 3.0f + 1) * sy;
    float vyr = oy + y / 3.0f * sy;
    float vzl = oz + z * sz;
    float vzr = oz + (z + 1) * sz;
    DWORD clt = ArgbMul(0x40FF80FF, col), crt = ArgbMul(0x8080FFFF, col);
    DWORD clb = ArgbMul(0x80FFFF80, col), crb = ArgbMul(0xFFFF80FF, col);
    float tul = ((c-' ') * fWidth + 1) / sWidth;
    float tur = (((c+1)-' ') * fWidth - 1) / sWidth;
    float tvl = (n * fHeight + 0) / sHeight;
    float tvr = ((n+1) * fHeight - 1) / sHeight;
#if 0
#ifdef USE_TRIANGLE_LIST
    vertices[p+1].v.x = vertices[p+0].v.x = vxl;
    vertices[p+5].v.x = vertices[p+2].v.x = vxr;
    vertices[p+2].v.y = vertices[p+0].v.y = vyl;
    vertices[p+5].v.y = vertices[p+1].v.y = vyr;
    vertices[p+5].v.z = vertices[p+0].v.z = vzl;
    vertices[p+2].v.z = vertices[p+1].v.z = vzr;
    vertices[p+0].color = clt;
    vertices[p+1].color = clb;
    vertices[p+2].color = crt;
    vertices[p+5].color = crb;
    vertices[p+1].tu = vertices[p+0].tu = tul;
    vertices[p+5].tu = vertices[p+2].tu = tur;
    vertices[p+2].tv = vertices[p+0].tv = tvl;
    vertices[p+5].tv = vertices[p+1].tv = tvr;
    memcpy(&vertices[p+3], &vertices[p+2], sizeof(CUSTOMVERTEX)); // sharedVtx
    memcpy(&vertices[p+4], &vertices[p+1], sizeof(CUSTOMVERTEX)); // sharedVtx
#else // USE_TRIANGLE_STRIP
    vertices[p+2].v.x = vertices[p+1].v.x = vxl;
    vertices[p+4].v.x = vertices[p+3].v.x = vxr;
    vertices[p+3].v.y = vertices[p+1].v.y = vyl;
    vertices[p+4].v.y = vertices[p+2].v.y = vyr;
    vertices[p+4].v.z = vertices[p+1].v.z = vzl;
    vertices[p+3].v.z = vertices[p+2].v.z = vzr;
    vertices[p+1].color = clt;
    vertices[p+2].color = clb;
    vertices[p+3].color = crt;
    vertices[p+4].color = crb;
    vertices[p+2].tu = vertices[p+1].tu = tul;
    vertices[p+4].tu = vertices[p+3].tu = tur;
    vertices[p+3].tv = vertices[p+1].tv = tvl;
    vertices[p+4].tv = vertices[p+2].tv = tvr;
    memcpy(&vertices[p+0], &vertices[p+1], sizeof(CUSTOMVERTEX)); // shrinkedTr
    memcpy(&vertices[p+5], &vertices[p+4], sizeof(CUSTOMVERTEX)); // shrinkedTr
#endif
#else
#ifdef USE_TRIANGLE_LIST
    VTX(vertices[p+0], vxl, vyl, vzl, clt, tul, tvl);
    VTX(vertices[p+1], vxl, vyr, vzr, clb, tul, tvr);
    VTX(vertices[p+2], vxr, vyl, vzr, crt, tur, tvl);
    VTX(vertices[p+5], vxr, vyr, vzl, crb, tur, tvr);
    vertices[p+3] = vertices[p+2];
    vertices[p+4] = vertices[p+1];
#else // USE_TRIANGLE_STRIP
    VTX(vertices[p+1], vxl, vyl, vzl, clt, tul, tvl);
    VTX(vertices[p+2], vxl, vyr, vzr, clb, tul, tvr);
    VTX(vertices[p+3], vxr, vyl, vzr, crt, tur, tvl);
    VTX(vertices[p+4], vxr, vyr, vzl, crb, tur, tvr);
    vertices[p+0] = vertices[p+1];
    vertices[p+5] = vertices[p+4];
#endif
#endif
  }
  DX9_StoreVertices(PPS(pIS, PSO_STRVBUF), blen, vertices);
  free(vertices);
  return len;
}

__D9PORT BOOL DrawString(RENDERD3DITEMSSTATE *pIS, DWORD col, char *s, int n,
  float sx, float sy, float sz, float ox, float oy, float oz)
{
  D9FOUNDATION *d9f = pIS->d9fnd;
  int len;

  len = CharGeometries(pIS, col, s, n, sx, sy, sz, ox, oy, oz);
  if(!len) return FALSE;
  DX9_SetTexture(FNDD(pIS), PPS(pIS, PSO_STRING), 0);
#ifdef USE_TRIANGLE_LIST
  DX9_RenderVertices(FNDD(pIS), PPS(pIS, PSO_STRVBUF), sizeof(CUSTOMVERTEX),
    2 * len, 0x00000060);
#else // USE_TRIANGLE_STRIP
  DX9_RenderVertices(FNDD(pIS), PPS(pIS, PSO_STRVBUF), sizeof(CUSTOMVERTEX),
    6 * len - 2, 0x00000020);
#endif
  return TRUE;
}

__inline HRESULT BltTexture_Inline(RENDERD3DITEMSSTATE *pIS, DWORD col,
  LPDIRECT3DTEXTURE9 *ppTexture, int left, int top, int right, int bottom,
  double cx, double cy, double cz, double px, double py, double pz)
{
  RECT rc;
  D3DXVECTOR3 center, pos;

  rc.left = left, rc.top = top, rc.right = right, rc.bottom = bottom;
  center.x = cx, center.y = cy, center.z = cz;
  pos.x = px, pos.y = py, pos.z = pz;
  return CI(Draw, FNDS(pIS), *ppTexture, &rc, &center, &pos, col);
}

__D9PORT BOOL BltString(RENDERD3DITEMSSTATE *pIS, DWORD col, char *s, int n,
  int ox, int oy, float oz)
{
  D9FOUNDATION *d9f = pIS->d9fnd;
  DWORD fWidth = 8, fHeight = 16;
  int i;

  for(i = 0; i < strlen(s); ++i){
    HRESULT hr = BltTexture_Inline(pIS, col,
      (LPDIRECT3DTEXTURE9 *)PPS(pIS, PSO_STRING),
      (s[i] - ' ') * fWidth + 1, // left
      n * fHeight + 0, // top
      ((s[i] + 1) - ' ') * fWidth - 0, // right
      (n + 1) * fHeight - 1, // bottom
      0.0f, 0.0f, 0.0f, ox + i * fWidth, oy, oz);
    if(hr != S_OK) return FALSE;
  }
  return TRUE;
}

__D9PORT BOOL BltTexture(RENDERD3DITEMSSTATE *pIS, DWORD col,
  LPDIRECT3DTEXTURE9 *ppTexture, int left, int top, int right, int bottom,
  double cx, double cy, double cz, double px, double py, double pz)
{
  HRESULT hr = BltTexture_Inline(pIS, col, ppTexture, left, top, right, bottom,
    cx, cy, cz, px, py, pz);
  return (hr != S_OK) ? FALSE : TRUE;
}

BOOL RenderDirect3D(RENDERD3DITEMSSTATE *pIS)
{
  D9FOUNDATION *d9f = pIS->d9fnd;

  CI(Clear, FNDD(pIS), 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
    pIS->bgc, 1.0f, 0);
  if(SUCCEEDED(CIC(BeginScene, FNDD(pIS)))){
    char s[1024];
    RECT rc;
    DWORD ff = DT_LEFT | DT_TOP | DT_NOCLIP; // | DT_CALCRECT;
    D3DCOLOR fc0 = D3DCOLOR_ARGB(255, 128, 255, 128);
    D3DCOLOR fc1 = D3DCOLOR_ARGB(255, 128, 128, 255);
    char *txt = "ABC\ndef";

    if(pIS->mode & 0x80000000){
      rc.left = 384, rc.top = 384, rc.right = 448, rc.bottom = 448;
      CI(DrawTextA, FNDF(pIS), NULL, txt, -1, &rc, ff, fc0);
    }
#if 1
    pIS->stat |= 0x00008000;
    CI(Begin, FNDS(pIS), D3DXSPRITE_ALPHABLEND); // or (unsigned long)NULL
#ifdef FONT_WITH_DRAWTEXT
    if(pIS->mode & 0x10000000){
      rc.left = 0, rc.top = 0, rc.right = 128, rc.bottom = 128;
      CI(DrawTextA, FNDF(pIS), FNDS(pIS), txt, -1, &rc, ff, fc1);
    }
    if(pIS->mode & 0x20000000){
      rc.left = 320, rc.top = 320, rc.right = 384, rc.bottom = 384;
      CI(DrawTextA, FNDF(pIS), FNDS(pIS), txt, -1, &rc, ff, fc0);
    }
#endif
    if(!(pIS->stat & 0x80000000)) CBRenderD3DItems(pIS);
    if(pIS->mode & 0x08000000){
      sprintf(s, "T %08X", pIS->nowTime);
      BltString(pIS, 0xFF808080, s, 0, 256, 96, 0.1f);
    }
    if(pIS->mode & 0x04000000){
      sprintf(s, "Q %02d FPS", pIS->fps);
      BltString(pIS, 0xFF808080, s, 1, 256, 128, 0.1f);
    }
    CIC(End, FNDS(pIS));
    pIS->stat &= 0xFFFF7FFF;
#endif
#if 1
    // SetupMatrices(pIS); // called from CBRenderD3DItems
    // DrawAxis(pIS); // called from CBRenderD3DItems
    if(!(pIS->stat & 0x80000000)) CBRenderD3DItems(pIS);
    if(pIS->mode & 0x00800000){
      sprintf(s, "T %08X", pIS->nowTime);
      DrawString(pIS, pIS->fgc, s, 3, 0.45f, 0.6f, -0.1f, -2.0f, -2.5f, 1.0f);
    }
    if(pIS->mode & 0x00400000){
      sprintf(s, "Q %02d FPS", pIS->fps);
      DrawString(pIS, pIS->fgc, s, 4, 0.75f, 1.0f, -0.25f, -2.0f, -2.0f, 2.0f);
    }
#endif
#ifdef FONT_WITH_DRAWTEXT
    if(pIS->mode & 0x40000000){
#if 0
      // after SetupMatrices()
      D9FOUNDATION *d9f = pIS->d9fnd;
      Q_D3DMATRIX *menv = d9f->pMenv;
      D3DVIEWPORT9 viewPort;
      D3DVECTOR fvec, wvec; // D3DXVECTOR3 fvec, wvec;
      viewPort.X = 0, viewPort.Y = 0;
      viewPort.Width = pIS->width, viewPort.Height = pIS->height;
      viewPort.MinZ = 0.0f, viewPort.MaxZ = 1.0f;
      wvec.x = 0, wvec.y = 0, wvec.z = 0;
      D3DXVec3Project(&fvec, &wvec, &viewPort,
        menv->proj, menv->view, menv->world);
      rc.left = (int)fvec.x, rc.top = (int)fvec.y;
      rc.right = (int)fvec.x + 200, rc.bottom = (int)fvec.y + 64;
      CI(DrawTextA, FNDF(pIS), NULL, txt, -1, &rc, ff, fc1);
#else
      rc.left = 4, rc.top = 4, rc.right = pIS->width, rc.bottom = pIS->height;
      CI(DrawTextA, FNDF(pIS), NULL, txt, -1, &rc, ff, fc1);
#endif
    }
#endif
    CIC(EndScene, FNDD(pIS));
    if(D3DERR_DEVICELOST == CI(Present, FNDD(pIS), NULL, NULL, NULL, NULL)){
      // TestCooperativeLevel
      //  D3D_OK : ready to rendering
      //  D3DERR_DEVICELOST
      //  D3DERR_DEVICENOTRESET
      //  D3DERR_DRIVERINTERNALERROR
      // D3DPOOL_DEFAULT, D3DPOOL_MANAGED, D3DPOOL_SYSTEMMEM, D3DPOOL_SCRATCH
      // *** release before reset, rebuild after reset ***
      // volume map texture       IDirect3DVolumeTexture9     D3DPOOL_DEFAULT
      // texture                  IDirect3DTexture9           D3DPOOL_DEFAULT
      // cube map texture         IDirect3DCubeTexture9       D3DPOOL_DEFAULT
      // surface                  IDirect3DSurface9           D3DPOOL_DEFAULT
      // depth stencil            IDirect3DSurface9
      // redering target surface  IDirect3DSurface9
      // vertex buffer            IDirect3DVertexBuffer9      D3DPOOL_DEFAULT
      // index buffer             IDirect3DIndexBuffer9       D3DPOOL_DEFAULT
      // *** must rebuild D3D resources with D3D Presentation Parameters ***
      // not use D3DERR_DEVICELOST != CIC(TestCooperativeLevel, FNDD(pIS))
      pIS->stat |= 0x80000000;
#if 0
      ReleaseDirect3D(pIS); // through cleared and will be called over twice
      if(D3DERR_DEVICENOTRESET == CIC(TestCooperativeLevel, FNDD(pIS))){
        if(D3D_OK != CI(Reset, FNDD(pIS), d9f->pD3Dpp)) return FALSE;
        ResetDirect3D(pIS);
      }
#else
#if 0
      if(D3DERR_DEVICENOTRESET == CIC(TestCooperativeLevel, FNDD(pIS))){
        ReleaseDirect3D(pIS); // through cleared and will be called over twice
        if(D3D_OK == CI(Reset, FNDD(pIS), d9f->pD3Dpp))
          ResetDirect3D(pIS);
      }
#else
      if(D3DERR_DEVICENOTRESET == CIC(TestCooperativeLevel, FNDD(pIS))){
        ReleaseDirect3D(pIS); // through cleared and will be called over twice
        if(D3D_OK == CI(Reset, FNDD(pIS), d9f->pD3Dpp)) ResetDirect3D(pIS);
        else{ CleanupDirect3D(pIS); InitDirect3D(pIS); }
      }
#endif
#endif
    }
  }
  return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
  switch(msg){
  case WM_DESTROY:
    CleanupDirect3D(g_pIS);
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, msg, wp, lp);
  }
  return 0L;
}

__D9PORT BOOL InitD3DApp(HINSTANCE hInst, int nShow,
  char *szClassName, char *szAppName,
  RENDERD3DITEMSSTATE *pIS,
  CALLBACKINITD3DITEMS cbInitD3DItems,
  CALLBACKCLEANUPD3DITEMS cbCleanupD3DItems,
  CALLBACKRENDERD3DITEMS cbRenderD3DItems)
{
  WNDCLASS wc;
  RECT r;

  g_pIS = pIS;
  if(!pIS) return FALSE;
  pIS->stat = 0x80000000;
  if(!pIS->d9fnd){
    pIS->d9fnd = &g_d9f;
    pIS->stat |= 0x00000001;
  }
  pIS->sysChain = NULL;
  pIS->usrChain = NULL;
  if(!pIS->smx) pIS->smx = OCHAIN_SMX;
  if(!pIS->umx) pIS->umx = OCHAIN_UMX;
  if(!pIS->bgc && !pIS->fgc) pIS->fgc = D3DCOLOR_ARGB(255, 192, 192, 192);
  if(pIS->mode & 0x00000001) debugalloc();
  pIS->hWnd = NULL;
  pIS->fps = 0, pIS->prevTime = 0, pIS->nowTime = 0;
  if(!pIS->d9fnd->pMenv) pIS->d9fnd->pMenv = &gm_env;
  do{
    Q_D3DMATRIX *menv = pIS->d9fnd->pMenv;
    if(!menv->tmp) menv->tmp = &gm_tmp;
    if(!menv->world) menv->world = &gm_world;
    if(!menv->view) menv->view = &gm_view;
    if(!menv->proj) menv->proj = &gm_proj;
    if(!pIS->d9fnd->pVecs) pIS->d9fnd->pVecs = &g_d9f_vecs;
    do{
      D9F_VECS *v = pIS->d9fnd->pVecs;
      D3DXMatrixIdentity(menv->world);
      D3DXMatrixLookAtLH(menv->view, &v->eyePt, &v->lookatPt, &v->upVec);
      D3DXMatrixPerspectiveFovLH(menv->proj, v->fovY, v->aspect, v->zn, v->zf);
    }while(0);
  }while(0);
  if(!pIS->d9fnd->imstring){
    pIS->d9fnd->imstring = IMG_STRING;
    pIS->d9fnd->imw = IMG_STRING_W, pIS->d9fnd->imh = IMG_STRING_H;
  }
  // wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInst;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL; // (HBRUSH)GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = szClassName;
  if(!RegisterClass(&wc)) return FALSE;
  r.left = 0, r.top = 0, r.right = pIS->width, r.bottom = pIS->height;
  AdjustWindowRectEx(&r,
    WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION,
    FALSE, 0);
  pIS->hWnd = CreateWindow(szClassName, szAppName, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
    NULL, NULL, hInst, NULL);
  if(!pIS->hWnd) return FALSE;
  ShowWindow(pIS->hWnd, nShow);
  UpdateWindow(pIS->hWnd);
  CBInitD3DItems = cbInitD3DItems;
  CBResetD3DItems = cbInitD3DItems;
  CBReleaseD3DItems = cbCleanupD3DItems;
  CBCleanupD3DItems = cbCleanupD3DItems;
  CBRenderD3DItems = cbRenderD3DItems;
  if(!InitDirect3D(pIS)) return FALSE;
  pIS->stat &= 0x7FFFFFFF;
  return TRUE;
}

__D9PORT DWORD MsgLoop(RENDERD3DITEMSSTATE *pIS)
{
  MSG msg;

  if(!pIS || (pIS->stat & 0x80000000) || !pIS->hWnd) return 1;
  while(msg.message != WM_QUIT){
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0){
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }else{
      D3DRASTER_STATUS rs;
//      if(D3D_OK != CI(GetRasterStatus, FNDD(pIS), 0, &rs))
//        rs.InVBlank = TRUE; // failure to get VSync Status (is it correct?)
//      if(rs.InVBlank){
        static DWORD cfps = 0, fps = 0, prevTime = 0, nowTime = 0;
        pIS->fps = fps, pIS->prevTime = prevTime, pIS->nowTime = nowTime;
        if(!RenderDirect3D(pIS)){ // draw while VSync
          CleanupDirect3D(pIS);
          PostQuitMessage(1);
          continue;
        }
        ++cfps;
        nowTime = timeGetTime();
        if(nowTime - prevTime >= 1000){
#ifdef FONT_WITH_GDI32
          char sfps[1024];
          HDC hdc = GetDC(hWnd);
          SetBkMode(hdc, TRANSPARENT);
#endif
          prevTime = nowTime;
          fps = cfps;
#ifdef FONT_WITH_GDI32
          sprintf(sfps, "%d", fps);
          TextOut(hdc, 0, 0, sfps, strlen(sfps));
          ReleaseDC(hWnd, hdc);
#endif
          cfps = 0;
        }
//      }
    }
    // Sleep(1);
  }
  if(pIS->mode & 0x00000001) debugfree();
  return msg.wParam;
}

__D9PORT DWORD DX9_Mode(RENDERD3DITEMSSTATE *pIS)
{
  DWORD m = pIS->mode;

  // 0x00000001; // call debugalloc() debugfree()

  // 0x00400000; // DrawString Q fps
  // 0x00800000; // DrawString T nowTime
  // 0x04000000; // BltString Q fps
  // 0x08000000; // BltString T nowTime
  // 0x10000000; // DrawTextA 3 fc1 Sprite // FONT_WITH_DRAWTEXT
  // 0x20000000; // DrawTextA 2 fc0 Sprite // FONT_WITH_DRAWTEXT
  // 0x40000000; // DrawTextA 1 fc1 no Sprite // FONT_WITH_DRAWTEXT
  // 0x80000000; // DrawTextA 0 fc0 no Sprite // always
  return m;
}

__D9PORT DWORD DX9_Status(RENDERD3DITEMSSTATE *pIS)
{
  DWORD s = pIS->stat;

  return s;
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
