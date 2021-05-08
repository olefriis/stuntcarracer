// Stub functions enabling us to compile the project.

#include "dxstdafx.h"

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

HRESULT DirectSoundCreate8(
         LPCGUID lpcGuidDevice,
         LPDIRECTSOUND8 * ppDS8,
         LPUNKNOWN pUnkOuter
) {
  puts("DirectSoundCreate8");
  return S_OK;
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

IDirect3DDevice9* DXUTGetD3DDevice() {
  puts("DXUTGetD3DDevice");
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

D3DXMATRIX* D3DXMatrixPerspectiveFovLH(
  D3DXMATRIX *pOut,
  FLOAT      fovy,
  FLOAT      Aspect,
  FLOAT      zn,
  FLOAT      zf
) {
  puts("D3DXMatrixPerspectiveFovLH");
  return NULL;
}

D3DXMATRIX* D3DXMatrixIdentity(
  D3DXMATRIX *pOut
) {
  puts("D3DXMatrixIdentity");
  return NULL;
}

D3DXMATRIX* D3DXMatrixRotationX(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
  puts("D3DXMatrixRotationX");
  return NULL;
}

D3DXMATRIX* D3DXMatrixRotationY(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
  puts("D3DXMatrixRotationY");
  return NULL;
}

D3DXMATRIX* D3DXMatrixRotationZ(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
  puts("D3DXMatrixRotationZ");
  return NULL;
}

D3DXMATRIX* D3DXMatrixTranslation(
  D3DXMATRIX *pOut,
  FLOAT      x,
  FLOAT      y,
  FLOAT      z
) {
  puts("D3DXMatrixTranslation");
  return NULL;
}

D3DXMATRIX* D3DXMatrixLookAtLH(
  D3DXMATRIX  *pOut,
  const D3DXVECTOR3 *pEye,
  const D3DXVECTOR3 *pAt,
  const D3DXVECTOR3 *pUp
) {
  puts("D3DXMatrixLookAtLH");
  return NULL;
}

D3DXMATRIX* D3DXMatrixMultiply(
  D3DXMATRIX *pOut,
  const D3DXMATRIX *pM1,
  const D3DXMATRIX *pM2
) {
  puts("D3DXMatrixMultiply");
  return NULL;
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

void DXUTSetCallbackDeviceCreated( LPDXUTCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext ) {
  puts("DXUTSetCallbackDeviceCreated");
}
void DXUTSetCallbackDeviceReset( LPDXUTCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext ) {
  puts("DXUTSetCallbackDeviceReset");
}
void DXUTSetCallbackDeviceLost( LPDXUTCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext ) {
  puts("DXUTSetCallbackDeviceLost");
}
void DXUTSetCallbackDeviceDestroyed( LPDXUTCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext ) {
  puts("DXUTSetCallbackDeviceDestroyed");
}
void DXUTSetCallbackDeviceChanging( LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings, void* pUserContext ) {
  puts("DXUTSetCallbackDeviceChanging");
}

void DXUTSetCallbackFrameMove( LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext ) {
  puts("DXUTSetCallbackFrameMove");
}
void DXUTSetCallbackFrameRender( LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext ) {
  puts("DXUTSetCallbackFrameRender");
}

void DXUTSetCallbackKeyboard( LPDXUTCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext ) {
  puts("DXUTSetCallbackKeyboard");
}
void DXUTSetCallbackMouse( LPDXUTCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove, void* pUserContext ) {
  puts("DXUTSetCallbackMouse");
}
void DXUTSetCallbackMsgProc( LPDXUTCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext ) {
  puts("DXUTSetCallbackMsgProc");
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

HRESULT DXUTCreateWindow( const WCHAR* strWindowTitle ) {
  puts("DXUTCreateWindow");
  return S_OK;
}

HRESULT DXUTCreateDevice( UINT AdapterOrdinal, bool bWindowed, 
                          int nSuggestedWidth, int nSuggestedHeight,
                          LPDXUTCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable,
                          LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings,
                          void* pUserContext ) {
  puts("DXUTCreateDevice");
  return S_OK;
}

// Maybe this should have been a macro?
D3DCOLOR D3DCOLOR_XRGB( int r, int g, int b ) {
  puts("D3DCOLOR_XRGB");
  return 0;
}

CONST D3DSURFACE_DESC * DXUTGetBackBufferSurfaceDesc() {
  puts("DXUTGetBackBufferSurfaceDesc");
  return 0;
}


/*
 * Resource loading. See https://docs.microsoft.com/en-us/windows/win32/menurc/using-resources
 */
HRSRC FindResource( HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType ) {
  puts("FindResource");
  return null;
}

HGLOBAL LoadResource( HMODULE hModule, HRSRC hResInfo ) {
  puts("LoadResource");
  return null;
}

LPVOID LockResource( HGLOBAL hResData ) {
  puts("LockResource");
  return null;
}


HRESULT StringCchPrintf( LPTSTR pszDest, size_t cchDest, LPCTSTR pszFormat, ... ) {
  puts("StringCchPrintf");
  return S_OK;
}


HRESULT DXUTMainLoop() {
  puts("DXUTMainLoop");
  return S_OK;
}

// Implemented in StuntCarRacer.cpp
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int );

int main() {
  puts("Main function!");
  WinMain( NULL, NULL, NULL, NULL );
	return 0;
}
