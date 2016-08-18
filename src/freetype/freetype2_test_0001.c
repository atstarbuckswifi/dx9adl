/*
  freetype2_test_0001.c
  test use windows DLL 2.4.11 (from freetype2_tutorial_example1.c)

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

  # make D3DXFreeType2.dll and test
    >\dm\bin\dmc -WD -L/implib D3DxFreeType2.c D3DxConsole.lib freetype.lib \
      kernel32.lib \
      -I freetype-2.4.11-custom-windll\include -I . -D_DLL
    >D3DxFreeType2_test.py
    >copy D3DxFreeType2.dll ..
    >copy D3DxFreeType2.lib ..

  # compile
    >\dm\bin\dmc freetype2_test_0001.c D3DxFreeType2.lib D3DxConsole.lib \
      -I freetype-2.4.11-custom-windll\include -I . -D_DLL
    >freetype2_test_0001 mikaP.ttf abcXYZ
    >freetype2_test_0001 SourceCodePro-Regular.ttf abcXYZ
*/

#include "D3DxFreeType2.h"
#include "D3DxConsole.h"

#define DEPTH 1
#define WIDTH 64 // 640
#define HEIGHT 48 // 480

unsigned char g_image[HEIGHT][WIDTH];

int main(int argc, char **argv)
{
  DWORD r;
  CUSTOMRECT rct = {sizeof(g_image[0]), g_image};
  GLYPH_TBL gt = {
    NULL, NULL, "mikaP.ttf", L"3\u30422\u3041\u3045", 1000.f, 25.f, 0, 0,
    7, DEPTH, WIDTH, HEIGHT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

  debugalloc();
  debugout("test %d %s %08x %20.13f\n", 3, "xYabZ", 257, 1.234f);
  gt.rct = &rct;
  r = D3DXFT2_GlyphOutline(&gt);
  if(r && (D3DXFT2_Status(&gt) & 0x00000002)) D3DXFT2_GlyphShowBmp(&gt);
  debugfree();
  return r ? 0 : 255;
}
