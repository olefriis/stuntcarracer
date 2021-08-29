// Stub functions enabling us to compile the project.

#include "dxstdafx.h"
#include <fstream>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

/*
 * CDXUTTextHelper
 */
CDXUTTextHelper::CDXUTTextHelper( ID3DXFont* pFont, ID3DXSprite* pSprite, int nLineHeight ) : m_clr(D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f )) {
  Debug("CDXUTTextHelper constructor");
}
void CDXUTTextHelper::SetInsertionPos( int x, int y ) {
  m_pt.x = x; m_pt.y = y;
}
void CDXUTTextHelper::SetForegroundColor( D3DXCOLOR clr ) {
  m_clr = clr;
}
void CDXUTTextHelper::Begin() {
  Debug("CDXUTTextHelper::Begin");
}
HRESULT CDXUTTextHelper::DrawFormattedTextLine( const WCHAR* strMsg, ... ) {
  Debug("CDXUTTextHelper::DrawFormattedTextLine");
  return S_OK;
}
HRESULT CDXUTTextHelper::DrawTextLine( const WCHAR* strMsg ) {
  Debug("CDXUTTextHelper::DrawTextLine");
  return S_OK;
}
void CDXUTTextHelper::End() {
  Debug("CDXUTTextHelper::End");
}

HRESULT D3DXCreateSprite(
  LPDIRECT3DDEVICE9 pDevice,
  LPD3DXSPRITE      *ppSprite
) {
  Debug("D3DXCreateSprite");
  return S_OK;
}

HRESULT D3DXCreateTextureFromResource(
  LPDIRECT3DDEVICE9  pDevice,
  HMODULE            hSrcModule,
  LPCTSTR            pSrcResource,
  LPDIRECT3DTEXTURE9 *ppTexture
) {
  Debug("D3DXCreateTextureFromResource");
  return S_OK;
}

HWND DXUTGetHWND() {
  Debug("DXUTGetHWND");
  return NULL;
}

LPCWSTR DXUTGetDeviceStats() {
  Debug("DXUTGetDeviceStats");
  return NULL;
}

LPCWSTR DXUTGetFrameStats( bool bShowFPS ) {
  Debug("DXUTGetFrameStats");
  return NULL;
}

IDirect3D9* DXUTGetD3DObject() {
  Debug("DXUTGetD3DObject");
  return NULL;
}

static double currentTime = 0;

double DXUTGetTime() {
  Debug("DXUTGetTime");
  return currentTime;
}

// For our own use
void setCurrentTime(double time) {
	currentTime = time;
}

void DXUTDisplaySwitchingToREFWarning() {
  Debug("DXUTDisplaySwitchingToREFWarning");
}

HRESULT DXUTReset3DEnvironment() {
  Debug("DXUTReset3DEnvironment");
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
  Debug("D3DXCreateFont");
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
  Debug("MessageBox");
  return 0;
}

int DXUTGetExitCode() {
  Debug("DXUTGetExitCode");
  return 0;
}

HRESULT DXUTInit( bool bParseCommandLine, bool bHandleDefaultHotkeys, bool bShowMsgBoxOnError, bool bHandleAltEnter ) {
  Debug("DXUTInit");
  return S_OK;
}

void DXUTSetCursorSettings( bool bShowCursorWhenFullScreen, bool bClipCursorWhenFullScreen ) {
  Debug("DXUTSetCursorSettings");
}

HRESULT DXUTCreateDevice( UINT AdapterOrdinal, bool bWindowed, 
                          int nSuggestedWidth, int nSuggestedHeight,
                          LPDXUTCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable,
                          LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings,
                          void* pUserContext ) {
  Debug("DXUTCreateDevice");
  return S_OK;
}

HRESULT StringCchPrintf( LPTSTR pszDest, size_t cchDest, LPCTSTR pszFormat, ... ) {
  Debug("StringCchPrintf");
  return S_OK;
}
