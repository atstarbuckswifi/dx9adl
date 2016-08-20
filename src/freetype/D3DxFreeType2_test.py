#!/usr/local/bin/python
# -*- coding: utf-8 -*-
'''d3dxfreetype2_test
'''

import sys, os
import ctypes
from ctypes import cast, POINTER, c_void_p, byref, pointer, CFUNCTYPE
from ctypes import c_double, c_float, c_long, c_int, c_char_p, c_wchar_p

tdll = ctypes.cdll
dllname0 = 'D3DxConsole.dll'
dllm0 = tdll.LoadLibrary(dllname0)
debugalloc = dllm0.debugalloc
debugout = dllm0.debugout
debugfree = dllm0.debugfree
dllname1 = 'D3DxFreeType2.dll'
dllm1 = tdll.LoadLibrary(dllname1)
D3DXFT2_GlyphShowBmp = dllm1.D3DXFT2_GlyphShowBmp
D3DXFT2_GlyphOutline = dllm1.D3DXFT2_GlyphOutline
D3DXFT2_Status = dllm1.D3DXFT2_Status

class CustomRect(ctypes.Structure):
  _fields_ = [('Pitch', c_int), ('pBits', c_void_p)]

class GlyphTbl(ctypes.Structure):
  _fields_ = [('pIS', c_void_p), ('pVG', c_void_p),
    ('facename', c_char_p), ('utxt', c_wchar_p),
    ('ratio', c_float), ('angle', c_float),
    ('reserved0', c_int), ('reserved1', c_int),
    ('mode', c_int), ('td', c_int), ('tw', c_int), ('th', c_int),
    ('rct', c_void_p), ('glyphBmp', c_void_p),
    ('matrix', c_void_p), ('vtx', c_void_p), ('funcs', c_void_p),
    ('glyphContours', c_void_p),
    ('glyphAlloc', c_void_p),
    ('glyphFree', c_void_p)]

class ParamArgs(ctypes.Structure):
  _fields_ = [('a0', c_char_p), ('a1', c_char_p), ('a2', c_char_p)]

def main():
  debugout('test\n')
  # debugout('%20.13f\n', 123456.98765432109994) # BAD can't convert
  debugout('%20.13f\n', c_float(123456.98765432109994)) # BAD value 0
  debugout('%20.13f\n', c_double(123456.98765432109994)) # 9994->994
  debugout('%08x\n', 0x12345678)
  debugout('%d, %u\n', 0xFFFFFFFF, 0xFFFFFFFF)
  td, tw, th = 1, 64, 48
  buf = ctypes.create_string_buffer(th * tw)
  rct = CustomRect(tw, cast(pointer(buf), c_void_p))
  face = r'..\..\res\mikaP.ttf'
  gt = GlyphTbl(0, 0, face, u'3\u30422\u3041\u3045', 1000., 25., 0, 0,
    3, td, tw, th, cast(pointer(rct), c_void_p), 0, 0, 0, 0, 0, 0, 0)
#   argc, argv = 3, ParamArgs('thisfile', face, 'abcXYZ')
#   r = D3DXFT2_GlyphOutline(argc, POINTER(ParamArgs)(argv)) # OK but old
#   # r = D3DXFT2_GlyphOutline(argc, byref(argv)) # OK but old
  r = D3DXFT2_GlyphOutline(byref(gt)) # OK new
  if r and (D3DXFT2_Status(byref(gt)) & 0x00000002):
    D3DXFT2_GlyphShowBmp(byref(gt))

if __name__ == '__main__':
  debugalloc()
  main()
  debugfree()
