// Stub functions enabling us to compile the project.

#include "dxstdafx.h"
#include <fstream>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

/*
 * CDXUTTextHelper
 */
CDXUTTextHelper::CDXUTTextHelper( ID3DXFont* pFont, ID3DXSprite* pSprite, int nLineHeight ) : m_clr(D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f )) {
  puts("CDXUTTextHelper constructor");
}
void CDXUTTextHelper::SetInsertionPos( int x, int y ) {
  m_pt.x = x; m_pt.y = y;
}
void CDXUTTextHelper::SetForegroundColor( D3DXCOLOR clr ) {
  m_clr = clr;
}
void CDXUTTextHelper::Begin() {
  puts("CDXUTTextHelper::Begin");
}
HRESULT CDXUTTextHelper::DrawFormattedTextLine( const WCHAR* strMsg, ... ) {
  puts("CDXUTTextHelper::DrawFormattedTextLine");
  return S_OK;
}
HRESULT CDXUTTextHelper::DrawTextLine( const WCHAR* strMsg ) {
  puts("CDXUTTextHelper::DrawTextLine");
  return S_OK;
}
void CDXUTTextHelper::End() {
  puts("CDXUTTextHelper::End");
}

HRESULT D3DXCreateSprite(
  LPDIRECT3DDEVICE9 pDevice,
  LPD3DXSPRITE      *ppSprite
) {
  puts("D3DXCreateSprite");
  return S_OK;
}

HRESULT D3DXCreateTextureFromResource(
  LPDIRECT3DDEVICE9  pDevice,
  HMODULE            hSrcModule,
  LPCTSTR            pSrcResource,
  LPDIRECT3DTEXTURE9 *ppTexture
) {
  puts("D3DXCreateTextureFromResource");
  return S_OK;
}

HWND DXUTGetHWND() {
  puts("DXUTGetHWND");
  return NULL;
}

LPCWSTR DXUTGetDeviceStats() {
  puts("DXUTGetDeviceStats");
  return NULL;
}

LPCWSTR DXUTGetFrameStats( bool bShowFPS ) {
  puts("DXUTGetFrameStats");
  return NULL;
}

IDirect3D9* DXUTGetD3DObject() {
  puts("DXUTGetD3DObject");
  return NULL;
}

double DXUTGetTime() {
  puts("DXUTGetTime");
  return 0;
}

void DXUTDisplaySwitchingToREFWarning() {
  puts("DXUTDisplaySwitchingToREFWarning");
}

HRESULT DXUTReset3DEnvironment() {
  puts("DXUTReset3DEnvironment");
  return S_OK;
}

HRESULT D3DXCreateFont(
  LPDIRECT3DDEVICE9 pDevice,
  INT               Height,
  UINT              Width,
  UINT              Weight,
  UINT              MipLevels,
  BOOL              Italic,
  DWORD             CharSet,
  DWORD             OutputPrecision,
  DWORD             Quality,
  DWORD             PitchAndFamily,
  LPCTSTR           pFacename,
  LPD3DXFONT        *ppFont
) {
  puts("D3DXCreateFont");
  // TODO: Set ppFont to something useful
  *ppFont = NULL;
  return S_OK;
}

int MessageBox(
  HWND    hWnd,
  LPCTSTR lpText,
  LPCTSTR lpCaption,
  UINT    uType
) {
  puts("MessageBox");
  return 0;
}

int DXUTGetExitCode() {
  puts("DXUTGetExitCode");
  return 0;
}

HRESULT DXUTInit( bool bParseCommandLine, bool bHandleDefaultHotkeys, bool bShowMsgBoxOnError, bool bHandleAltEnter ) {
  puts("DXUTInit");
  return S_OK;
}

void DXUTSetCursorSettings( bool bShowCursorWhenFullScreen, bool bClipCursorWhenFullScreen ) {
  puts("DXUTSetCursorSettings");
}

HRESULT DXUTCreateDevice( UINT AdapterOrdinal, bool bWindowed, 
                          int nSuggestedWidth, int nSuggestedHeight,
                          LPDXUTCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable,
                          LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings,
                          void* pUserContext ) {
  puts("DXUTCreateDevice");
  return S_OK;
}

HRESULT StringCchPrintf( LPTSTR pszDest, size_t cchDest, LPCTSTR pszFormat, ... ) {
  puts("StringCchPrintf");
  return S_OK;
}

