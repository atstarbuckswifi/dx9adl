/*
  D3DxFT2_types.h
*/

#ifndef __D3DXFT2_TYPES_H__
#define __D3DXFT2_TYPES_H__

typedef struct _CUSTOMRECT_ { // fake d3d9.h struct _D3DLOCKED_RECT
  INT Pitch;
  void *pBits;
} CUSTOMRECT;

#ifndef VTX
typedef struct _CUSTOMVERTEX_ { // fake d3d9.h d3dx9.h
  struct { FLOAT x, y, z; } v; // D3DVECTOR v; // D3DXVECTOR3 v;
  DWORD color;
  FLOAT tu, tv; // texture
} CUSTOMVERTEX;

#define VTX(r, vx, vy, vz, c, ptu, ptv) do{ \
  r.v.x = (vx), r.v.y = (vy), r.v.z = (vz), r.color = (c); \
  r.tu = (ptu), r.tv = (ptv); \
}while(0)
#endif

#ifndef COR
typedef struct _CUSTOMCOR_ { // fake d3d9.h d3dx9.h
  struct { FLOAT x, y, z; } v; // D3DVECTOR v; // D3DXVECTOR3 v;
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

typedef struct _GLYPH_CONTOUR_ {
//int atr; // attribute 0: moveto, 1: lineto, 2: conicto, 3: cubicto, 4: divBz
  int n; // (for divBz)
  FT_Vector cg; // sum and center of gravity
  FT_Vector p0; // first point (start with moveto, detect end with same point)
  CUSTOMCUV *points; // top of buffer (n x CUSTOMCUV)
//struct _GLYPH_CONTOUR_ prev;
//struct _GLYPH_CONTOUR_ next;
} GLYPH_CONTOUR;

typedef struct _GLYPH_VTX_ {
  GLYPH_CONTOUR *contour;
  int cmx; // max length of buffer GLYPH_CONTOUR
  int ctc; // total count of GLYPH_CONTOUR
  CUSTOMCUV *buf;
  int bmx; // max length of buffer CUSTOMCUV
  int btc; // total count of CUSTOMCUV (x 2) (+ N x 2) (add count of moveto)
  FT_Vector p; // vector of current position {FT_Pos x, y;} / 0:G last:G (FAN)
  FT_Pos pz, z;
  DWORD col, bgc;
} GLYPH_VTX;

typedef struct _GLYPH_TBL_ {
  VOID *pIS; // not included yet RENDERD3DITEMSSTATE * ../dx9adl.h
  VOID *pVG; // not included yet VERTEX_GLYPH * ../D3DxGlyph.h
  char *facename; // font file path
  wchar_t *utxt;
  FLOAT ratio, angle;
  DWORD reserved0, reserved1;
  DWORD mode;
  int td, tw, th; // target depth, width, height
  CUSTOMRECT *rct;
  BOOL (*glyphBmp)(struct _GLYPH_TBL_ *gt, FT_Bitmap *bmp, FT_Int x, FT_Int y);
  VOID *matrix; // QUOTANIONIC_MATRIX *
  GLYPH_VTX *vtx;
  FT_Outline_Funcs *funcs;
  BOOL (*glyphContours)(struct _GLYPH_TBL_ *gt);
  BOOL (*glyphAlloc)(struct _GLYPH_TBL_ *gt, size_t cmx, size_t bmx);
  BOOL (*glyphFree)(struct _GLYPH_TBL_ *gt);
} GLYPH_TBL;

#endif // __D3DXFT2_TYPES_H__
