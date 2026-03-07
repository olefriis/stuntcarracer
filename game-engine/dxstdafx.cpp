// Stub functions enabling us to compile the project.

#include "dxstdafx.h"
#include <fstream>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <stdarg.h>
#include <wchar.h>

HWND DXUTGetHWND() {
  Debug("DXUTGetHWND");
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
  va_list args;
  va_start(args, pszFormat);
  vswprintf(pszDest, cchDest, pszFormat, args);
  va_end(args);
  return S_OK;
}
