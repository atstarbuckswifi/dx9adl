#!/usr/local/bin/python
# -*- coding: utf-8 -*-
'''pyDx9adl
'''

import ctypes
from ctypes import cast, POINTER, c_void_p, byref, pointer, CFUNCTYPE
from ctypes import c_double, c_float, c_long, c_int, c_char_p, c_wchar_p

dllnames = ['d3d9', 'd3dx9', 'D3DxConsole', 'D3DxFreeType2',
  'D3DxTextureBmp', 'dx9adl', 'D3DxGlyph']
dllm = {}
for i, n in enumerate(dllnames):
  dllm[n] = (ctypes.windll if i < 2 else ctypes.cdll).LoadLibrary('%s.dll' % n)

Direct3DCreate9 = dllm['d3d9'].Direct3DCreate9

D3DXVec3Project = dllm['d3dx9'].D3DXVec3Project
D3DXCreateFontIndirectA = dllm['d3dx9'].D3DXCreateFontIndirectA
D3DXCreateFontA = dllm['d3dx9'].D3DXCreateFontA
D3DXCreateSprite = dllm['d3dx9'].D3DXCreateSprite
D3DXCreateTextureFromFileA = dllm['d3dx9'].D3DXCreateTextureFromFileA
D3DXMatrixRotationX = dllm['d3dx9'].D3DXMatrixRotationX
D3DXMatrixRotationY = dllm['d3dx9'].D3DXMatrixRotationY
D3DXMatrixRotationZ = dllm['d3dx9'].D3DXMatrixRotationZ
D3DXMatrixLookAtLH = dllm['d3dx9'].D3DXMatrixLookAtLH
D3DXMatrixPerspectiveFovLH = dllm['d3dx9'].D3DXMatrixPerspectiveFovLH

debugalloc = dllm['D3DxConsole'].debugalloc
debugout = dllm['D3DxConsole'].debugout
debugfree = dllm['D3DxConsole'].debugfree

D3DXFT2_GlyphAlloc = dllm['D3DxFreeType2'].D3DXFT2_GlyphAlloc
D3DXFT2_GlyphFree = dllm['D3DxFreeType2'].D3DXFT2_GlyphFree
D3DXFT2_GlyphBmp = dllm['D3DxFreeType2'].D3DXFT2_GlyphBmp
D3DXFT2_GlyphShowBmp = dllm['D3DxFreeType2'].D3DXFT2_GlyphShowBmp
D3DXFT2_GlyphOutline = dllm['D3DxFreeType2'].D3DXFT2_GlyphOutline
D3DXFT2_Status = dllm['D3DxFreeType2'].D3DXFT2_Status

D3DXTXB_CreateTexture = dllm['D3DxTextureBmp'].D3DXTXB_CreateTexture
D3DXTXB_LockRect = dllm['D3DxTextureBmp'].D3DXTXB_LockRect
D3DXTXB_UnlockRect = dllm['D3DxTextureBmp'].D3DXTXB_UnlockRect
D3DXTXB_RenderFontGlyph = dllm['D3DxTextureBmp'].D3DXTXB_RenderFontGlyph
D3DXTXB_CopyTexture = dllm['D3DxTextureBmp'].D3DXTXB_CopyTexture
D3DXTXB_RewriteTexture = dllm['D3DxTextureBmp'].D3DXTXB_RewriteTexture

ReleaseNil = dllm['dx9adl'].ReleaseNil
PtrPtrS = dllm['dx9adl'].PtrPtrS
PtrSO = dllm['dx9adl'].PtrSO
PtrPtrU = dllm['dx9adl'].PtrPtrU
PtrUO = dllm['dx9adl'].PtrUO
SetupMatrices = dllm['dx9adl'].SetupMatrices
DrawAxis = dllm['dx9adl'].DrawAxis
DX9_CreateVertexBuffer = dllm['dx9adl'].DX9_CreateVertexBuffer
DX9_Lock = dllm['dx9adl'].DX9_Lock
DX9_Unlock = dllm['dx9adl'].DX9_Unlock
DX9_StoreVertices = dllm['dx9adl'].DX9_StoreVertices
DX9_LoadVertices = dllm['dx9adl'].DX9_LoadVertices
DX9_SetTexture = dllm['dx9adl'].DX9_SetTexture
DX9_RenderVertices = dllm['dx9adl'].DX9_RenderVertices
BltString = dllm['dx9adl'].BltString
BltTexture = dllm['dx9adl'].BltTexture
InitD3DApp = dllm['dx9adl'].InitD3DApp
MsgLoop = dllm['dx9adl'].MsgLoop
DX9_Mode = dllm['dx9adl'].DX9_Mode
DX9_Status = dllm['dx9adl'].DX9_Status

D3DXGLP_InitFont = dllm['D3DxGlyph'].D3DXGLP_InitFont
D3DXGLP_CleanupFont = dllm['D3DxGlyph'].D3DXGLP_CleanupFont
D3DXGLP_GlyphContours = dllm['D3DxGlyph'].D3DXGLP_GlyphContours
D3DXGLP_DrawGlyph = dllm['D3DxGlyph'].D3DXGLP_DrawGlyph

class D3DMatrix(ctypes.Structure):
  _fields_ = [
    ('aa', c_float), ('ba', c_float), ('ca', c_float), ('da', c_float),
    ('ab', c_float), ('bb', c_float), ('cb', c_float), ('db', c_float),
    ('ac', c_float), ('bc', c_float), ('cc', c_float), ('dc', c_float),
    ('ad', c_float), ('bd', c_float), ('cd', c_float), ('dd', c_float)]

class Q_D3DMatrix(ctypes.Structure):
  _fields_ = [ # 4 x pointer to D3DMatrix
    ('tmp', c_void_p),
    ('world', c_void_p), ('view', c_void_p), ('proj', c_void_p)]

class D3DVector(ctypes.Structure):
  _fields_ = [('x', c_float), ('y', c_float), ('z', c_float)]

class CustomCor(ctypes.Structure):
  _fields_ = [('v', D3DVector), ('color', c_int)]

class CustomVertex(ctypes.Structure):
  _fields_ = [('v', D3DVector), ('color', c_int),
    ('tu', c_float), ('tv', c_float)]

class D9F_Vecs(ctypes.Structure):
  _fields_ = [
    ('eyePt', D3DVector), ('reserved0', c_int),
    ('lookatPt', D3DVector), ('reserved1', c_int),
    ('upVec', D3DVector), ('reserved2', c_int),
    ('fovY', c_float), ('aspect', c_float), ('zn', c_float), ('zf', c_float)]

class D9Foundation(ctypes.Structure):
  _fields_ = [('pD3Dpp', c_void_p), ('pD3D', c_void_p), ('pDev', c_void_p),
    ('pMenv', c_void_p), # pointer to Q_D3DMatrix
    ('pVecs', c_void_p), # pointer to D9F_Vecs
    ('imstring', c_char_p), ('imw', c_int), ('imh', c_int)]

class RenderD3DItemsState(ctypes.Structure):
  _fields_ = [('parent', c_void_p), ('d9fnd', c_void_p),
    ('sysChain', c_void_p), ('usrChain', c_void_p),
    ('smx', c_int), ('umx', c_int),
    ('width', c_int), ('height', c_int), ('bgc', c_int), ('fgc', c_int),
    ('mode', c_int), ('stat', c_int), ('hWnd', c_long),
    ('fps', c_int), ('prevTime', c_int), ('nowTime', c_int)]

# CBFunc = CFUNCTYPE(c_int, c_void_p)
CBFunc = CFUNCTYPE(c_int, POINTER(RenderD3DItemsState))
