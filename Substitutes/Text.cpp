// Text rendering via a 2D canvas overlay on top of the WebGL canvas.

#include "../dxstdafx.h"
#include <emscripten.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

/*
 * Text overlay using a 2D canvas on top of the WebGL canvas.
 * We create the overlay canvas from C++ on first use, then draw text via EM_JS calls.
 */

// Initialize the text overlay canvas (called once)
EM_JS(void, initTextOverlay, (), {
  if (window._textOverlayReady) return;

  var glCanvas = document.getElementById('canvas');

  // Wrap the WebGL canvas in a container so the overlay can be positioned on top
  var container = document.createElement('div');
  container.style.cssText = 'position:relative;width:100%;height:100%;';
  glCanvas.parentNode.insertBefore(container, glCanvas);
  container.appendChild(glCanvas);

  var overlay = document.createElement('canvas');
  overlay.id = 'textOverlay';
  overlay.style.position = 'absolute';
  overlay.style.left = '0';
  overlay.style.top = '0';
  overlay.style.width = '100%';
  overlay.style.height = '100%';
  overlay.style.pointerEvents = 'none';
  overlay.width = glCanvas.width;
  overlay.height = glCanvas.height;

  container.appendChild(overlay);

  window._textOverlay = overlay;
  window._textOverlayCtx = overlay.getContext('2d');
  window._textOverlayReady = true;
});

// Clear the text overlay canvas and sync its size with the WebGL canvas
EM_JS(void, clearTextOverlay, (), {
  if (!window._textOverlayReady) return;
  var glCanvas = document.getElementById('canvas');
  var overlay = window._textOverlay;
  if (overlay.width !== glCanvas.width || overlay.height !== glCanvas.height) {
    overlay.width = glCanvas.width;
    overlay.height = glCanvas.height;
    overlay.style.width = glCanvas.style.width;
    overlay.style.height = glCanvas.style.height;
  }
  window._textOverlayCtx.clearRect(0, 0, overlay.width, overlay.height);
});

// Draw a text string on the overlay canvas
// x, y: pixel position (y is top of the text line)
// r, g, b, a: color components 0.0-1.0
// fontHeight: pixel size of the font
// text: the string to draw (passed as a UTF-8 C string)
EM_JS(void, drawTextOnOverlay, (int x, int y, float r, float g, float b, float a, int fontHeight, const char* text), {
  if (!window._textOverlayReady) return;
  // On touch devices, the touch controls replace the text overlay
  if (window._isTouchDevice) return;
  var ctx = window._textOverlayCtx;
  var str = UTF8ToString(text);
  ctx.font = 'bold ' + fontHeight + 'px Arial, sans-serif';
  ctx.fillStyle = 'rgba(' +
    Math.round(r * 255) + ',' +
    Math.round(g * 255) + ',' +
    Math.round(b * 255) + ',' +
    a + ')';
  ctx.fillText(str, x, y + fontHeight);  // Canvas 2D y is baseline, so offset by fontHeight
});

// Track the last frame time to know when to clear the overlay (once per frame)
static double lastClearedFrameTime = -1.0;

/*
 * ID3DXFont / ID3DXSprite
 */
HRESULT ID3DXFont::OnResetDevice() { return S_OK; }
HRESULT ID3DXFont::OnLostDevice() { return S_OK; }
void ID3DXFont::Release() {}
void ID3DXSprite::Release() {}

/*
 * CDXUTTextHelper
 */
CDXUTTextHelper::CDXUTTextHelper( ID3DXFont* pFont, ID3DXSprite* pSprite, int nLineHeight ) : m_clr(D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f )), m_nLineHeight(nLineHeight), m_pFont(pFont) {
  m_pt.x = 0;
  m_pt.y = 0;
  initTextOverlay();
}
void CDXUTTextHelper::SetInsertionPos( int x, int y ) {
  m_pt.x = x; m_pt.y = y;
}
void CDXUTTextHelper::SetForegroundColor( D3DXCOLOR clr ) {
  m_clr = clr;
}
void CDXUTTextHelper::Begin() {
  // Only clear the overlay once per frame. Multiple CDXUTTextHelper instances
  // may call Begin() in the same frame (e.g. normal font + large font).
  double now = DXUTGetTime();
  if (now != lastClearedFrameTime) {
    clearTextOverlay();
    lastClearedFrameTime = now;
  }
}
HRESULT CDXUTTextHelper::DrawFormattedTextLine( const WCHAR* strMsg, ... ) {
  // The original game code uses MSVC conventions where %s in a wide format string
  // means wchar_t*. Standard C (used by Clang/Emscripten) requires %ls for wide
  // strings. Convert %s -> %ls and %c -> %lc in the format string.
  WCHAR fixedFmt[512];
  int i = 0, j = 0;
  while (strMsg[i] && j < 510) {
    if (strMsg[i] == L'%') {
      fixedFmt[j++] = strMsg[i++];
      // Skip flags: -, +, 0, space, #
      while (strMsg[i] == L'-' || strMsg[i] == L'+' || strMsg[i] == L'0' ||
             strMsg[i] == L' ' || strMsg[i] == L'#') {
        fixedFmt[j++] = strMsg[i++];
      }
      // Skip width
      while (strMsg[i] >= L'0' && strMsg[i] <= L'9') {
        fixedFmt[j++] = strMsg[i++];
      }
      // Skip precision
      if (strMsg[i] == L'.') {
        fixedFmt[j++] = strMsg[i++];
        while (strMsg[i] >= L'0' && strMsg[i] <= L'9') {
          fixedFmt[j++] = strMsg[i++];
        }
      }
      // Check for length modifier 'l' already present
      if (strMsg[i] == L'l') {
        fixedFmt[j++] = strMsg[i++];
        fixedFmt[j++] = strMsg[i++]; // copy the specifier as-is
      } else if (strMsg[i] == L's' || strMsg[i] == L'c') {
        // Insert 'l' before 's' or 'c' to make %ls / %lc
        fixedFmt[j++] = L'l';
        fixedFmt[j++] = strMsg[i++];
      } else {
        // Other specifiers (d, x, f, etc.) - copy as-is
        fixedFmt[j++] = strMsg[i++];
      }
    } else {
      fixedFmt[j++] = strMsg[i++];
    }
  }
  fixedFmt[j] = L'\0';

  WCHAR buf[512];
  va_list args;
  va_start(args, strMsg);
  vswprintf(buf, 512, fixedFmt, args);
  va_end(args);

  return DrawTextLine(buf);
}
HRESULT CDXUTTextHelper::DrawTextLine( const WCHAR* strMsg ) {
  if (strMsg == NULL) return S_OK;

  // Convert wide string to UTF-8 for JavaScript
  // Calculate needed buffer size
  int len = wcslen(strMsg);
  // Worst case: 4 bytes per wchar + null terminator
  char* utf8buf = (char*)malloc(len * 4 + 1);
  wcstombs(utf8buf, strMsg, len * 4 + 1);

  int fontHeight = m_nLineHeight;
  if (m_pFont && m_pFont->m_height > 0) {
    fontHeight = m_pFont->m_height;
  }

  drawTextOnOverlay((int)m_pt.x, (int)m_pt.y, m_clr.r, m_clr.g, m_clr.b, m_clr.a, fontHeight, utf8buf);
  free(utf8buf);

  // Advance Y position by line height
  m_pt.y += m_nLineHeight;

  return S_OK;
}
void CDXUTTextHelper::End() {
  // Nothing to do - text is already drawn on the overlay
}

HRESULT D3DXCreateSprite(
  LPDIRECT3DDEVICE9 pDevice,
  LPD3DXSPRITE      *ppSprite
) {
  Debug("D3DXCreateSprite");
  *ppSprite = new ID3DXSprite();
  return S_OK;
}

LPCWSTR DXUTGetDeviceStats() {
  Debug("DXUTGetDeviceStats");
  return L"WebGL (Emscripten)";
}

LPCWSTR DXUTGetFrameStats( bool bShowFPS ) {
  Debug("DXUTGetFrameStats");
  return L"Stunt Car Racer";
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
  *ppFont = new ID3DXFont(Height);
  return S_OK;
}
