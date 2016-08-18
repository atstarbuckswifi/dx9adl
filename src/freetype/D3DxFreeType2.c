/*
  D3DxFreeType2.dll

  D3DxFreeType2.c

  use windows FreeType2 DLL 2.4.11

  # create dummy header (why used ConditionalMacros.h for MAC when dmc ?)
    >touch ConditionalMacros.h

  # define _DLL for use freetype.dll
    >copy "freetype-2.4.11-custom-windll\objs\win32\vc2010\freetype.dll" .
    >\dm\bin\implib _dmc_freetype.lib _dmc_freetype_partial.def
    >copy _dmc_freetype.lib freetype.lib
    >copy freetype.lib ..

  # make D3DxConsole.dll
    >\dm\bin\dmc -WD -L/implib D3DxConsole.c kernel32.lib
    >copy D3DxConsole.dll ..
    >copy D3DxConsole.lib ..
    >copy D3DxConsole.h ..

  # make D3DxFreeType2.dll and test
    >\dm\bin\dmc -WD -L/implib D3DxFreeType2.c D3DxConsole.lib freetype.lib \
      kernel32.lib \
      -I freetype-2.4.11-custom-windll\include -I . -D_DLL
    >D3DxFreeType2_test.py
    >copy D3DxFreeType2.dll ..
    >copy D3DxFreeType2.lib ..
*/

#define __D3DxFreeType2_MAKE_DLL__
#include "D3DxFreeType2.h"

#include "D3DxConsole.h"

#if 1 // when dmc and windows
FILE *_p_iob(void){ return NULL; } // dummy - not exists on dmc only ?
#else
// dummy
#endif

__inline FLOAT reScale(FLOAT r, FT_Pos d) // signed long
{
  return d / r;
}

__FTPORT int D3DXFT2_MoveToFunc(const FT_Vector *to, void *u)
{
  GLYPH_TBL *gt = (GLYPH_TBL *)u;
  GLYPH_VTX *v = gt->vtx;
  GLYPH_CONTOUR *cont = &v->contour[v->ctc];
  FLOAT r = gt->ratio;
  FLOAT x = reScale(r, to->x), y = reScale(r, to->y), z = reScale(r, v->z);
#ifdef GLYPH_DEBUG
 if(gt->mode & 0x00000001) debugout("move  (%.4f, %.4f)\n", x, y);
#endif
  cont->n = 0, cont->cg.x = 0, cont->cg.y = 0;
  cont->p0.x = v->p.x = to->x, cont->p0.y = v->p.y = to->y, v->pz = 0;
  cont->points = &v->buf[v->btc];
  CUV(cont->points[cont->n], x, y, 0.0f, v->col);
  v->btc++;
  cont->n++;
  v->ctc++;
  return 0; // FT_Outline_Decompose()
}

__FTPORT int D3DXFT2_LineToFunc(const FT_Vector *to, void *u)
{
  GLYPH_TBL *gt = (GLYPH_TBL *)u;
  GLYPH_VTX *v = gt->vtx;
  GLYPH_CONTOUR *cont = &v->contour[v->ctc - 1];
  FLOAT r = gt->ratio;
  FLOAT x = reScale(r, to->x), y = reScale(r, to->y), z = reScale(r, v->z);
#ifdef GLYPH_DEBUG
 if(gt->mode & 0x00000001)
  debugout("line  (%.4f, %.4f) (%.4f, %.4f)\n",
    reScale(r, v->p.x), reScale(r, v->p.y),
    reScale(r, to->x), reScale(r, to->y));
#endif
  v->p.x = to->x, v->p.y = to->y;
  CUV(cont->points[cont->n], x, y, 0.0f, v->col);
  v->btc++;
  cont->n++;
  return 0; // FT_Outline_Decompose()
}

__FTPORT int D3DXFT2_ConicToFunc(
  const FT_Vector *control, const FT_Vector *to, void *u)
{
  GLYPH_TBL *gt = (GLYPH_TBL *)u;
  GLYPH_VTX *v = gt->vtx;
  GLYPH_CONTOUR *cont = &v->contour[v->ctc - 1];
  FLOAT r = gt->ratio;
  FLOAT x = reScale(r, to->x), y = reScale(r, to->y), z = reScale(r, v->z);
  FLOAT xc = reScale(r, control->x), yc = reScale(r, control->y);
#ifdef GLYPH_DEBUG
 if(gt->mode & 0x00000001)
  debugout("conic (%.4f, %.4f) (%.4f, %.4f) (%.4f, %.4f)\n",
    reScale(r, v->p.x), reScale(r, v->p.y),
    reScale(r, control->x), reScale(r, control->y),
    reScale(r, to->x), reScale(r, to->y));
#endif
  v->p.x = to->x, v->p.y = to->y;
if(gt->mode & 0x00008000){ // skip Bézier interpolation
  CUV(cont->points[cont->n], xc, yc, 0.0f, v->col);
  CUV(cont->points[cont->n + 1], x, y, 0.0f, v->col);
  v->btc += 2;
  cont->n += 2;
}else{ // Bézier interpolation
  CUV(cont->points[cont->n], xc, yc, 0.0f, v->col);
  CUV(cont->points[cont->n + 1], x, y, 0.0f, v->col);
  v->btc += 2;
  cont->n += 2;
}
  return 0; // FT_Outline_Decompose()
}

__FTPORT int D3DXFT2_CubicToFunc(
  const FT_Vector *control1, const FT_Vector *control2,
  const FT_Vector *to, void *u)
{
  GLYPH_TBL *gt = (GLYPH_TBL *)u;
  GLYPH_VTX *v = gt->vtx;
  GLYPH_CONTOUR *cont = &v->contour[v->ctc - 1];
  FLOAT r = gt->ratio;
  FLOAT x = reScale(r, to->x), y = reScale(r, to->y), z = reScale(r, v->z);
  FLOAT xc2 = reScale(r, control2->x), yc2 = reScale(r, control2->y);
  FLOAT xc1 = reScale(r, control1->x), yc1 = reScale(r, control1->y);
#ifdef GLYPH_DEBUG
 if(gt->mode & 0x00000001)
  debugout("cubic (%.4f, %.4f) (%.4f, %.4f) (%.4f, %.4f) (%.4f, %.4f)\n",
    reScale(r, v->p.x), reScale(r, v->p.y),
    reScale(r, control1->x), reScale(r, control1->y),
    reScale(r, control2->x), reScale(r, control2->y),
    reScale(r, to->x), reScale(r, to->y));
#endif
  v->p.x = to->x, v->p.y = to->y;
if(gt->mode & 0x00008000){ // skip Bézier interpolation
  CUV(cont->points[cont->n], xc1, yc1, 0.0f, v->col);
  CUV(cont->points[cont->n + 1], xc2, yc2, 0.0f, v->col);
  CUV(cont->points[cont->n + 2], x, y, 0.0f, v->col);
  v->btc += 3;
  cont->n += 3;
}else{ // Bézier interpolation
  CUV(cont->points[cont->n], xc1, yc1, 0.0f, v->col);
  CUV(cont->points[cont->n + 1], xc2, yc2, 0.0f, v->col);
  CUV(cont->points[cont->n + 2], x, y, 0.0f, v->col);
  v->btc += 3;
  cont->n += 3;
}
  return 0; // FT_Outline_Decompose()
}

__FTPORT BOOL D3DXFT2_GlyphContours(GLYPH_TBL *gt)
{
  GLYPH_VTX *v = gt->vtx;

  debugout("called D3DXFT2_GlyphContours: %d, %d\n", v->ctc, v->btc);
  return TRUE;
}

__FTPORT BOOL D3DXFT2_GlyphAlloc(GLYPH_TBL *gt, size_t cmx, size_t bmx)
{
  GLYPH_VTX *gv = gt->vtx;

  if(!gv) return FALSE;
  gv->contour = (GLYPH_CONTOUR *)malloc(cmx * sizeof(GLYPH_CONTOUR));
  gv->cmx = cmx, gv->ctc = 0;
  gv->buf = (CUSTOMCUV *)malloc(bmx * sizeof(CUSTOMCUV));
  gv->bmx = bmx, gv->btc = 0;
  gv->p.x = 0, gv->p.y = 0, gv->pz = 0, gv->z = gt->td;
  if(!gv->col && !gv->bgc) gv->col = 0xFFC0C0C0;
  return TRUE;
}

__FTPORT BOOL D3DXFT2_GlyphFree(GLYPH_TBL *gt)
{
  GLYPH_VTX *gv = gt->vtx;

  if(!gv) return FALSE;
  if(gv->buf){ free(gv->buf); gv->buf = NULL; }
  if(gv->contour){ free(gv->contour); gv->contour = NULL; }
  return TRUE;
}

__FTPORT BOOL D3DXFT2_GlyphBmp(GLYPH_TBL *gt,
  FT_Bitmap *bmp, FT_Int x, FT_Int y)
{
  int tw = gt->tw, th = gt->th;
  CUSTOMRECT *rct = gt->rct;
  FT_Int i, j, p, q;
  FT_Int x_max = x + bmp->width;
  FT_Int y_max = y + bmp->rows;

  for(j = y, q = 0; j < y_max; j++, q++){
    unsigned char *imline = (unsigned char *)rct->pBits + j * rct->Pitch;
    for(i = x, p = 0; i < x_max; i++, p++){
      if(i < 0 || j < 0 || i >= tw || j >= th) continue;
      imline[i] |= bmp->buffer[q * bmp->width + p];
    }
  }
  return TRUE;
}

__FTPORT BOOL D3DXFT2_GlyphShowBmp(GLYPH_TBL *gt)
{
  int tw = gt->tw, th = gt->th;
  CUSTOMRECT *rct = gt->rct;
  int i, j;

  for(j = 0; j < th; j++){
    unsigned char *imline = (unsigned char *)rct->pBits + j * rct->Pitch;
    for(i = 0; i < tw; i++)
      debugout("%c", imline[i] == 0 ? ' ' : imline[i] < 128 ? '+' : '*');
    debugout("\n");
  }
  return TRUE;
}

__FTPORT DWORD D3DXFT2_GlyphOutline(GLYPH_TBL *gt)
{
  FT_Library library;
  FT_Face face;
  FT_GlyphSlot slot;
  FT_Matrix matrix;
  FT_Vector pen;
  FT_Error error;
//  wchar_t *text; // char *text;
//  wchar_t utxt[1024];
  int n, num_chars;
  double radius = 0.f;
//  wchar_t *u = L"あ"; // why set utf-8 (e38182) ? (must compile -DUNICODE ?)
  wchar_t *u = L"\u3042"; // u3042 -> index of mikaP.ttf = 0x07bd
  *u = L"\u3041"[0]; // re write it (anti optimize)
  DWORD stat = D3DXFT2_Status(gt);
  BOOL test = stat & 0x00000001;

#ifdef GLYPH_DEBUG
 if(test) debugout("start\n");
#endif
  if(!gt){
    debugout("\ano address GLYPH_TBL\n");
    return FALSE;
  }
  if((stat & 0x00000002) && !gt->rct){
    debugout("\ano address CUSTOMRECT\n");
    return FALSE;
  }
#ifdef GLYPH_DEBUG
 if(test){
  debugout("facename: %s\n", gt->facename);
  // How to debugout wchar_t * ? gt->utxt
  debugout("r, a: %f, %f\n", gt->ratio, gt->angle);
  debugout("mode: %08X\n", gt->mode);
  debugout("td, tw, th: %d, %d, %d\n", gt->td, gt->tw, gt->th);
 }
#endif
  if((stat & 0x0000000C) && (gt->ratio == 0.f)){
    debugout("\ainvalid ratio == 0.0\n");
    return FALSE;
  }
#if 0
  text = L"\u3042\u3044\u3046";
  num_chars = wcslen(text); // strlen(text); // when char *text;
#else
//  _swprintf(gt->utxt, L"%d%c%d%s", 3, 0x3042, 2, L"\u3041\u3045"); // o.w.
  num_chars = wcslen(gt->utxt);
#endif
  // error handling omitted
  error = FT_Init_FreeType(&library);
  error = FT_New_Face(library, gt->facename, 0, &face); // FT_New_Memory_Face
  // use 16pt(test) or 48pt(release) at 100dpi
  error = FT_Set_Char_Size(face, (test ? 16 : 48) * 64, 0, 100, 0);
  slot = face->glyph;
  radius = gt->angle * M_PI / 180;
  matrix.xx = (FT_Fixed)(cos(radius) * 0x10000L);
  matrix.xy = (FT_Fixed)(-sin(radius) * 0x10000L);
  matrix.yx = (FT_Fixed)(sin(radius) * 0x10000L);
  matrix.yy = (FT_Fixed)(cos(radius) * 0x10000L);
 if(stat & 0x00000002){ // converted to bitmap
  if(!gt->glyphBmp) gt->glyphBmp = D3DXFT2_GlyphBmp;
  // the pen position in 26.6 cartesian space coordinates;
  // start at (300, 200) relative to the upper left corner
  // pen.x = 300 * 64;
  // pen.y = (gt->th - 200) * 64;
  pen.x = (test ? 5 : 80) * 64; // ?
  pen.y = (gt->th - (test ? 5 : 180)) * (test ? 1 : 64); // ?
  for(n = 0; n < num_chars; n++){
    FT_Set_Transform(face, &matrix, &pen);
    //error = FT_Load_Char(face, text[n], FT_LOAD_RENDER); // != FT_Load_Glyph
    //error = FT_Load_Char(face, L"\u3042"[0], FT_LOAD_RENDER); // OK unicode
    //error = FT_Load_Glyph(face, 0x07bd, FT_LOAD_RENDER); // あ u3042 OK index
    //debugout("%08x\n", (unsigned int)u[0]); // e3 ?
    //debugout("%08x\n", (unsigned int)u[1]); // 81 ?
    //debugout("%08x\n", (unsigned int)u[2]); // 82 ?
    //debugout("%08x\n", (unsigned int)*u); // 00003042
    //debugout("%08x\n", (unsigned int)FT_Get_Char_Index(face, *u)); // 0x07bd
#if 0
    error = FT_Load_Glyph(face, FT_Get_Char_Index(face, *u), FT_LOAD_RENDER);
    ++*u;
#else
    error = FT_Load_Char(face, gt->utxt[n], FT_LOAD_RENDER); // fast ! cached ?
#endif
    if(error) continue; // ignore errors
#ifdef GLYPH_DEBUG
    if(slot->format == FT_GLYPH_FORMAT_OUTLINE) debugout("%d outline\n", n);
#endif
    if(slot->format == FT_GLYPH_FORMAT_BITMAP){
#ifdef GLYPH_DEBUG
 if(test) debugout("%d bitmap\n", n);
#endif
      gt->glyphBmp(gt, &slot->bitmap,
        slot->bitmap_left, gt->th - slot->bitmap_top);
    }
    pen.x += slot->advance.x;
    pen.y += slot->advance.y;
  }
 }
 if(stat & 0x0000000C){ // through outline
  GLYPH_VTX d_vtx = {NULL, 0, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0};
  FT_Outline_Funcs d_funcs = {
    D3DXFT2_MoveToFunc,
    D3DXFT2_LineToFunc,
    D3DXFT2_ConicToFunc, // second-order Bézier arc
    D3DXFT2_CubicToFunc, // third-order Bézier arc
    0, // int shift
    0}; // FT_Pos delta

  // if(!gt->matrix) gt->matrix = NULL; // nothing to do
  if(!gt->vtx) gt->vtx = &d_vtx;
  if(!gt->funcs) gt->funcs = &d_funcs;
  if(!gt->glyphContours) gt->glyphContours = D3DXFT2_GlyphContours;
  if(!gt->glyphAlloc) gt->glyphAlloc = D3DXFT2_GlyphAlloc;
  if(!gt->glyphFree) gt->glyphFree = D3DXFT2_GlyphFree;
  gt->glyphAlloc(gt, GLYPH_CMX, GLYPH_BMX); // must calc cmx and bmx
 if(gt->vtx->contour && gt->vtx->buf){
  // re-initialize the pen position drifted by drawing bmp
  pen.x = (test ? 5 : 80) * 64; // ?
  pen.y = (gt->th - (test ? 5 : 180)) * (test ? 1 : 64); // ?
  for(n = 0; n < num_chars; n++){
    FT_Set_Transform(face, &matrix, &pen);
    error = FT_Load_Char(face, gt->utxt[n], FT_LOAD_NO_BITMAP); //|FT_LOAD_NO_SCALE
    if(error) continue; // ignore errors
#ifdef GLYPH_DEBUG
    if(slot->format == FT_GLYPH_FORMAT_BITMAP) debugout("%d bitmap\n", n);
#endif
    if(slot->format == FT_GLYPH_FORMAT_OUTLINE){
#ifdef GLYPH_DEBUG
 if(test) debugout("%d outline\n", n);
#endif
      error = FT_Outline_Decompose(&slot->outline, gt->funcs, gt);
    }
    pen.x += slot->advance.x;
    pen.y += slot->advance.y;
  }
  gt->glyphContours(gt);
 }
  gt->glyphFree(gt);
 }
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return TRUE;
}

__FTPORT DWORD D3DXFT2_Status(GLYPH_TBL *gt)
{
  DWORD s = gt->mode;

  // 0x00000001; // test debug
  // 0x00000002; // glyphBitmap
  // 0x00000004; // glyphContours side TRIANGLESTRIP (z-width)
  // 0x00000008; // glyphContours face LINESTRIP contours (0-width)
  // 0x00000400; // glyphContours face tessellation TRIANGLELIST (fill)
  // 0x00000800; // glyphContours face tessellation LINELIST (0-width beams)
  // 0x00004000; // glyphContours skip tessellation
  // 0x00008000; // glyphContours skip Bézier interpolation
  // 0x40000000; // glyphContours no texture
#ifdef GLYPH_DEBUG
  s |= 0x80000000; // implement with debugout
#endif
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
