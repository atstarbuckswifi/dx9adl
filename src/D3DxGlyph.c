/*
  D3DxGlyph.dll

  D3DxGlyph.c

    >\dm\bin\dmc -WD -L/implib D3DxGlyph.c D3DxConsole.lib dx9adl.lib \
      kernel32.lib _dmc_d3d9.lib _dmc_d3dx9.lib \
      -I dxsdk\include \
      -I FreeType\freetype-2.4.11-custom-windll\include -I FreeType -D_DLL
*/

#include "dx9adl.h"
#include "quaternion.h"

#define __D3DXGLYPH_MAKE_DLL__
#include "D3DxGlyph.h"
#include "D3DxConsole.h"

__inline FLOAT getNthUV(int n)
{
  return (n % 9) / 8.f; // (n % 5) / 4.f; // must be regulated 0.f < uv < 1.f
}

__DGPORT BOOL D3DXGLP_InitFont()
{
  return TRUE;
}

__DGPORT BOOL D3DXGLP_CleanupFont()
{
  return TRUE;
}

__DGPORT BOOL D3DXGLP_GlyphContours(GLYPH_TBL *gt)
{
 if(!gt){
  debugout("D3DXGLP_GlyphContours: gt==NULL\n");
  return FALSE;
 }else{
  RENDERD3DITEMSSTATE *st = gt->pIS;
  VERTEX_GLYPH *g = gt->pVG;
  GLYPH_VTX *v = gt->vtx;
  void *Vertices;
  int m, n;
  QUATERNION qd, qs;
  QUATERNIONIC_MATRIX *mat = (QUATERNIONIC_MATRIX *)gt->matrix;
  CUSTOMCUV *p;

  if(gt->mode & 0x00000004)
    GLPS(g) = (v->btc + v->ctc) * 2 * sizeof(CUSTOMVERTEX);
  if(gt->mode & 0x00000008)
    GLPS(g) = (v->btc + 2 * v->ctc) * sizeof(CUSTOMCOR);
  if(NULL == (Vertices = (void *)malloc(GLPS(g)))) return FALSE;
  // use union CUSTOMCUV to handle member CUSTOMCOR and CUSTOMVERTEX same way
  p = (CUSTOMCUV *)Vertices;
  for(m = 0; m < v->ctc; ++m){
    GLYPH_CONTOUR *cont = &v->contour[m];
    for(n = 0; n < cont->n; ++n){
      if(!n){
        // duplicate dot last point
        CUSTOMCUV *q = m ? &v->contour[m - 1].points[v->contour[m - 1].n - 1]
          : &cont->points[n];
        memcpy(p, q, sizeof(CUSTOMCOR));
        if(gt->mode & 0x00000004){
          p->c.v.z = v->z / gt->ratio;
          p->v.tv = p->v.tu = 0.0f;
        }
        if(mat){ QFV(qs, p->c.v); QXM(qd, qs, *mat); QTV(p->c.v, qd); }
        p->c.color = v->bgc;
        if(gt->mode & 0x00000004){ ++(CUSTOMVERTEX *)p; }
        if(gt->mode & 0x00000008){ ++(CUSTOMCOR *)p; }
        // duplicate dot first point
        memcpy(p, &cont->points[n], sizeof(CUSTOMCOR));
        if(gt->mode & 0x00000004){ p->v.tv = p->v.tu = 0.0f; }
        if(mat){ QFV(qs, p->c.v); QXM(qd, qs, *mat); QTV(p->c.v, qd); }
        p->c.color = v->bgc;
        if(gt->mode & 0x00000004){ ++(CUSTOMVERTEX *)p; }
        if(gt->mode & 0x00000008){ ++(CUSTOMCOR *)p; }
      }
      // contour point
      memcpy(p, &cont->points[n], sizeof(CUSTOMCOR));
      if(gt->mode & 0x00000004){ p->v.tu = getNthUV(n), p->v.tv = 0.0f; }
      if(mat){ QFV(qs, p->c.v); QXM(qd, qs, *mat); QTV(p->c.v, qd); }
      if(gt->mode & 0x00000004){ ++(CUSTOMVERTEX *)p; }
      if(gt->mode & 0x00000008){ ++(CUSTOMCOR *)p; }
      // expand z depth
      if(gt->mode & 0x00000004){
        memcpy(p, &cont->points[n], sizeof(CUSTOMCOR));
        p->c.v.z = v->z / gt->ratio;
        p->v.tu = getNthUV(n), p->v.tv = 1.0f;
        if(mat){ QFV(qs, p->c.v); QXM(qd, qs, *mat); QTV(p->c.v, qd); }
        ++(CUSTOMVERTEX *)p;
      }
    }
  }
#ifdef GLYPH_DEBUG
 if(gt->mode & 0x00000001)
  debugout("called D3DXGLP_GlyphContours: %d %d\n", v->ctc, v->btc);
#endif
  if(!DX9_CreateVertexBuffer(FNDD(st), GLPS(g), (VOID **)GLPPV(g))){
    MessageBox(NULL, "Can't create vertex Glyph.", "Error", MB_OK);
    return FALSE;
  }
  DX9_StoreVertices((VOID **)GLPPV(g), GLPS(g), Vertices);
  free(Vertices);
  return TRUE;
 }
}

__DGPORT BOOL D3DXGLP_DrawGlyph(GLYPH_TBL *gt)
{
 if(!gt){
  debugout("D3DXGLP_DrawGlyph: gt==NULL\n");
  return FALSE;
 }else{
  RENDERD3DITEMSSTATE *st = gt->pIS;
  VERTEX_GLYPH *g = gt->pVG;

  if(!GLPPV(g) || !GLPV(g)) return FALSE;
 if(gt->mode & 0x00000004){
  DX9_SetTexture(FNDD(st),
    ((gt->mode & 0x40000000) || !GLPPT(g) || !GLPT(g)) ? NULL: GLPPT(g), 0);
  DX9_RenderVertices(FNDD(st), (VOID **)GLPPV(g), sizeof(CUSTOMVERTEX),
    (GLPS(g) / sizeof(CUSTOMVERTEX)) - 2, 0x00000020); // 0x00000022 0x00000021
 }
 if(gt->mode & 0x00000008){
#if 0
  DX9_RenderVertices(FNDD(st), (VOID **)GLPPV(g), sizeof(CUSTOMCOR),
    (GLPS(g) / sizeof(CUSTOMCOR)) - 1, 0x00000090); // alpha
#else
  DX9_RenderVertices(FNDD(st), (VOID **)GLPPV(g), sizeof(CUSTOMCOR),
    (GLPS(g) / sizeof(CUSTOMCOR)) - 1, 0x00000094); // overwrite
#endif
 }
  return TRUE;
 }
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
