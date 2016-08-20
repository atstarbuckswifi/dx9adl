#!/usr/local/bin/python
# -*- coding: utf-8 -*-
'''dx9adl_test
'''

import sys, os
import ctypes
from ctypes import cast, POINTER, c_void_p, byref, pointer, CFUNCTYPE
from ctypes import c_double, c_float, c_long, c_int, c_char_p, c_wchar_p
import math

from pyDx9adl import *

PSO_D3D, PSO_DEV, PSO_SPRITE, PSO_FONT, PSO_STRING, PSO_STRVBUF = range(6)
TXSRC, TXDST = range(2)
IMGSRC = '..\\res\\_col_4.png'

def initD3DItems(pIS):
  debugout('test callback initD3DItems\n')
  q = cast(pIS, POINTER(RenderD3DItemsState))[0]
  d9f = cast(q.d9fnd, POINTER(D9Foundation))[0]
  D3DXCreateTextureFromFileA(d9f.pDev, IMGSRC, PtrPtrU(pIS, TXSRC))
  D3DXTXB_CreateTexture(d9f.pDev, 256, 256, PtrPtrU(pIS, TXDST))
  debugout('pTextureSrc: %08X\n', PtrUO(pIS, TXSRC))
  debugout('pTexture: %08X\n', PtrUO(pIS, TXDST))
  return 1

def cleanupD3DItems(pIS):
  debugout('test callback cleanupD3DItems\n')
  debugout('pTexture: %08X\n', PtrUO(pIS, TXDST))
  debugout('pTextureSrc: %08X\n', PtrUO(pIS, TXSRC))
  return 1

def renderD3DItems(pIS):
  # p = cast(pIS, POINTER(RenderD3DItemsState))
  # q = p[0]
  q = pIS[0]
  d9f = cast(q.d9fnd, POINTER(D9Foundation))[0]
  if q.stat & 0x00008000:
    pSprite = PtrSO(pIS, PSO_SPRITE)
    # debugout('pSprite: %08X\n', pSprite)
    # debugout('pTextureSrc: %08X\n', PtrUO(pIS, TXSRC))
    # debugout('pTexture: %08X\n', PtrUO(pIS, TXDST))
    D3DXTXB_RewriteTexture(PtrPtrU(pIS, TXDST), PtrPtrU(pIS, TXSRC))
    BltTexture(pIS, 0xFFFFFFFF, PtrPtrU(pIS, TXDST), 0, 0, 256, 256,
      c_double(0.), c_double(0.), c_double(0.),
      c_double(10.), c_double(100.), c_double(1.))
    # ppTexture = pointer(cast(d9f.pSprite, c_void_p)) # ok but obsoleted
    # ppTexture = pointer(PtrSO(pIS, PSO_STRING)) # ok but another pointer
    BltTexture(pIS, 0xFFFFFFFF, PtrPtrS(pIS, PSO_STRING), 0, 0, 512, 512,
      c_double(0.), c_double(0.), c_double(0.),
      c_double(10.), c_double(10.), c_double(.5))
    BltString(pIS, 0xFF808080, 'BLTSTRING', 3, 16, 128, c_float(0.1))
  else:
    menv = cast(d9f.pMenv, POINTER(Q_D3DMatrix))[0]
    D3DXMatrixRotationY(menv.world, c_float(q.nowTime * -.06 * math.pi / 180))
    SetupMatrices(pIS)
    DrawAxis(pIS)
    if q.nowTime - q.prevTime < 5:
      debugout('test callback renderD3DItems: %02d %08X\n', q.fps, q.nowTime)
      D3DXGLP_GlyphContours(0) # dummy
      D3DXGLP_DrawGlyph(0) # dummy
  return 1

def main():
  debugout('adl_test\n')
  istat = RenderD3DItemsState(0, 0, 0, 0, 0, 0, 352, 198, 0, 0,
    0x0CC00000, 0, 0, 0, 0, 0)
  hInst = ctypes.windll.kernel32.GetModuleHandleA(0)
  nShow = 1 # 1: SW_SHOWNORMAL or 5: SW_SHOW
  className = ctypes.create_string_buffer('clsDx9ADLtest')
  appName = ctypes.create_string_buffer('appDx9ADLtest')
  InitD3DApp(hInst, nShow, byref(className), byref(appName), byref(istat),
    CBFunc(initD3DItems), CBFunc(cleanupD3DItems), CBFunc(renderD3DItems))
  print MsgLoop(byref(istat))
  InitD3DApp(hInst, nShow, byref(className), byref(appName), byref(istat),
    CBFunc(initD3DItems), CBFunc(cleanupD3DItems), CBFunc(renderD3DItems))
  print MsgLoop(byref(istat))

if __name__ == '__main__':
  debugalloc()
  main()
  debugfree()
