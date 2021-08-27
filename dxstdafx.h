#include <math.h>
#include <stdlib.h>
#include <iostream>

#define DEBUG_ENABLED 0
#define ERROR_ENABLED 1

#if DEBUG_ENABLED
#define Debug(x) printf("DEBUG: %s\n", x);
#define DebugPrintf(fmt, ...) printf("DEBUG: "); printf(fmt, ##__VA_ARGS__)
#else
#define Debug(x)
#define DebugPrintf(fmt, ...)
#endif

#if ERROR_ENABLED
#define Error(x) printf("ERROR: %s\n", x);
#define ErrorPrintf(fmt, ...) printf("ERROR: "); printf(fmt, ##__VA_ARGS__)
#else
#define Error(x)
#define ErrorPrintf(fmt, ...)
#endif

// Various typedefs that apparently are Windows-specific, but are used all over the code
// See e.g.:
// * https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/efda8314-6e41-4837-8299-38ba0ee04b92
// * https://docs.microsoft.com/en-us/windows/win32/winprog/windows-data-types
typedef int BOOL;
typedef unsigned char BYTE, *PBYTE, *LPBYTE;
typedef int INT;
typedef unsigned int UINT;
typedef float FLOAT;
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
typedef long LONG, *PLONG, *LPLONG;
typedef wchar_t WCHAR, *PWCHAR;
typedef long LONG_PTR;
typedef LONG HRESULT;
typedef LONG_PTR LRESULT;
typedef const wchar_t* LPCWSTR;
typedef void *PVOID;
typedef PVOID HANDLE;
typedef HANDLE HWND;
typedef const wchar_t *LPCTSTR;
typedef wchar_t *LPTSTR;
typedef unsigned int UINT_PTR;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef unsigned long size_t;
typedef char CHAR;
typedef CHAR *LPSTR;
typedef HANDLE HINSTANCE;
typedef HANDLE HMODULE;
typedef HANDLE HRSRC;
typedef HANDLE HGLOBAL;
typedef void *LPVOID;

#define TRUE 1
#define FALSE 0
#define NULL 0L
#define null 0L
#define DS_OK 0
#define S_OK 0
#define E_FAIL 0x80004005
#define CONST const
#define D3DX_PI (3.14159265358979323846)

// https://ccrma.stanford.edu/software/stk/Misc/dsound.h
#define DSBPAN_LEFT   -10000
#define DSBPAN_CENTER 0
#define DSBPAN_RIGHT  10000
#define DSSCL_NORMAL  0x00000001

// I guess this is OK...?
#define CALLBACK 
#define WINAPI 

#define D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING 524288

// Font stuff
#define FW_BOLD 700
#define DEFAULT_CHARSET 1
#define DEFAULT_QUALITY 0
#define OUT_DEFAULT_PRECIS 0

// MessageBox:
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
#define MB_OK 0
int MessageBox(
  HWND    hWnd,
  LPCTSTR lpText,
  LPCTSTR lpCaption,
  UINT    uType
);

#include "Substitutes/Resources.h"

typedef  enum
 {
   FF_DONTCARE = 0x00
 } FamilyFont;
 typedef  enum
 {
   DEFAULT_PITCH = 0,
 } PitchFont;

// DirectX-specific types
typedef DWORD D3DCOLOR;

typedef struct tagPALETTEENTRY {
  BYTE peRed;
  BYTE peGreen;
  BYTE peBlue;
  BYTE peFlags;
} PALETTEENTRY, *PPALETTEENTRY, *LPPALETTEENTRY;

// Virtual-Key Codes: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
// F keys
#define VK_F1 0x71
#define VK_F2 0x71
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F9 0x78
#define VK_F10 0x79
// Space and Return keys
#define VK_SPACE 0x20
#define VK_RETURN 0x0D

typedef enum D3DDEVTYPE { 
  D3DDEVTYPE_HAL          = 1,
  D3DDEVTYPE_NULLREF      = 4,
  D3DDEVTYPE_REF          = 2,
  D3DDEVTYPE_SW           = 3,
  D3DDEVTYPE_FORCE_DWORD  = 0xffffffff
} D3DDEVTYPE, *LPD3DDEVTYPE;

typedef enum D3DRESOURCETYPE { 
  D3DRTYPE_SURFACE        = 1,
  D3DRTYPE_VOLUME         = 2,
  D3DRTYPE_TEXTURE        = 3,
  D3DRTYPE_VOLUMETEXTURE  = 4,
  D3DRTYPE_CubeTexture    = 5,
  D3DRTYPE_VERTEXBUFFER   = 6,
  D3DRTYPE_INDEXBUFFER    = 7,
  D3DRTYPE_FORCE_DWORD    = 0x7fffffff
} D3DRESOURCETYPE, *LPD3DRESOURCETYPE;

typedef enum D3DCULL { 
  D3DCULL_NONE         = 1,
  D3DCULL_CW           = 2,
  D3DCULL_CCW          = 3,
  D3DCULL_FORCE_DWORD  = 0x7fffffff
} D3DCULL, *LPD3DCULL;


typedef enum D3DSHADEMODE { 
  D3DSHADE_FLAT         = 1,
  D3DSHADE_GOURAUD      = 2,
  D3DSHADE_PHONG        = 3,
  D3DSHADE_FORCE_DWORD  = 0x7fffffff
} D3DSHADEMODE, *LPD3DSHADEMODE;

typedef enum D3DTEXTUREOP { 
  D3DTOP_DISABLE                    = 1,
  D3DTOP_SELECTARG1                 = 2,
  D3DTOP_SELECTARG2                 = 3,
  D3DTOP_MODULATE                   = 4,
  D3DTOP_MODULATE2X                 = 5,
  D3DTOP_MODULATE4X                 = 6,
  D3DTOP_ADD                        = 7,
  D3DTOP_ADDSIGNED                  = 8,
  D3DTOP_ADDSIGNED2X                = 9,
  D3DTOP_SUBTRACT                   = 10,
  D3DTOP_ADDSMOOTH                  = 11,
  D3DTOP_BLENDDIFFUSEALPHA          = 12,
  D3DTOP_BLENDTEXTUREALPHA          = 13,
  D3DTOP_BLENDFACTORALPHA           = 14,
  D3DTOP_BLENDTEXTUREALPHAPM        = 15,
  D3DTOP_BLENDCURRENTALPHA          = 16,
  D3DTOP_PREMODULATE                = 17,
  D3DTOP_MODULATEALPHA_ADDCOLOR     = 18,
  D3DTOP_MODULATECOLOR_ADDALPHA     = 19,
  D3DTOP_MODULATEINVALPHA_ADDCOLOR  = 20,
  D3DTOP_MODULATEINVCOLOR_ADDALPHA  = 21,
  D3DTOP_BUMPENVMAP                 = 22,
  D3DTOP_BUMPENVMAPLUMINANCE        = 23,
  D3DTOP_DOTPRODUCT3                = 24,
  D3DTOP_MULTIPLYADD                = 25,
  D3DTOP_LERP                       = 26,
  D3DTOP_FORCE_DWORD                = 0x7fffffff
} D3DTEXTUREOP, *LPD3DTEXTUREOP;

#include "Substitutes/Direct3DVertexBuffer.h"

typedef struct D3DCAPS9 {
  D3DDEVTYPE DeviceType;
  UINT       AdapterOrdinal;
} D3DCAPS9;
typedef struct D3DFORMAT {
  FLOAT something;
} D3DFORMAT;

class IDirect3DBaseTexture9 {
  public:
};
class IDirect3DTexture9 : public IDirect3DBaseTexture9 {
  public:
  virtual void Release();
};
typedef IDirect3DTexture9 *LPDIRECT3DTEXTURE9;

typedef struct D3DVECTOR {
  FLOAT x;
  FLOAT y;
  FLOAT z;
} D3DVECTOR, *LPD3DXVECTOR3;

typedef struct D3DXVECTOR3 : public D3DVECTOR
{
public:
    D3DXVECTOR3() {};
    D3DXVECTOR3( FLOAT x, FLOAT y, FLOAT z ) {
		this->x = x;
		this->y = y;
		this->z = z;
	};
} D3DXVECTOR3;

#include "Substitutes/Matrix.h"
#include "Substitutes/Direct3DDevice.h"
typedef struct DXUTDeviceSettings {
  D3DDEVTYPE DeviceType;
} DXUTDeviceSettings;
class IDirect3D9 {
  public:
  virtual HRESULT CheckDeviceFormat(
    UINT            Adapter,
    D3DDEVTYPE      DeviceType,
    D3DFORMAT       AdapterFormat,
    DWORD           Usage,
    D3DRESOURCETYPE RType,
    D3DFORMAT       CheckFormat
  );
};

typedef struct POINT {
  FLOAT x;
  FLOAT y;
} POINT;
class ID3DXFont {
  public:
  virtual HRESULT OnResetDevice();
  virtual HRESULT OnLostDevice();
  virtual void Release();
};
typedef ID3DXFont *LPD3DXFONT;
class ID3DXSprite {
  public:
  virtual void Release();
};
typedef ID3DXSprite *LPD3DXSPRITE;
typedef struct tagRECT { 
    LONG    left;    // This is the upper-left corner x-coordinate.
    LONG    top;     // The upper-left corner y-coordinate.
    LONG    right;   // The lower-right corner x-coordinate.
    LONG    bottom;  // The lower-right corner y-coordinate.
} RECT, *PRECT;
typedef struct D3DXCOLOR
{
public:
    D3DXCOLOR( FLOAT r, FLOAT g, FLOAT b, FLOAT a ) : r(r), g(g), b(b), a(a) {};
    FLOAT r, g, b, a;
} D3DXCOLOR, *LPD3DXCOLOR;

class CDXUTTextHelper
{
public:
    CDXUTTextHelper( ID3DXFont* pFont, ID3DXSprite* pSprite, int nLineHeight );

    void SetInsertionPos( int x, int y );
    void SetForegroundColor( D3DXCOLOR clr );

    void Begin();
    HRESULT DrawFormattedTextLine( const WCHAR* strMsg, ... );
    HRESULT DrawTextLine( const WCHAR* strMsg );
    void End();

protected:
    D3DXCOLOR m_clr;
    POINT m_pt;
};

// Bogus types
typedef unsigned int LPCGUID;
typedef unsigned int LPUNKNOWN;
typedef struct D3DSURFACE_DESC {
  UINT Width;
  UINT Height;
} D3DSURFACE_DESC;

// DirectX functions
#include "Substitutes/Sound.h"

HRESULT D3DXCreateSprite(
  LPDIRECT3DDEVICE9 pDevice,
  LPD3DXSPRITE      *ppSprite
);

HRESULT D3DXCreateTextureFromResource(
  LPDIRECT3DDEVICE9  pDevice,
  HMODULE            hSrcModule,
  LPCTSTR            pSrcResource,
  LPDIRECT3DTEXTURE9 *ppTexture
);

HWND DXUTGetHWND();

LPCWSTR DXUTGetFrameStats( bool bIncludeFPS = false );
LPCWSTR DXUTGetDeviceStats();

#define D3DCOLOR_XRGB(r, g, b) r << 16 | g << 8 | b

IDirect3D9* DXUTGetD3DObject();

double DXUTGetTime();

void DXUTDisplaySwitchingToREFWarning();

HRESULT DXUTReset3DEnvironment();

//--------------------------------------------------------------------------------------
// Initialization
//--------------------------------------------------------------------------------------
HRESULT DXUTInit( bool bParseCommandLine = true, bool bHandleDefaultHotkeys = true, bool bShowMsgBoxOnError = true, bool bHandleAltEnter = true );

//--------------------------------------------------------------------------------------
// State Retrieval  
//--------------------------------------------------------------------------------------
int                     DXUTGetExitCode();

//--------------------------------------------------------------------------------------
// Common Tasks 
//--------------------------------------------------------------------------------------
void    DXUTSetCursorSettings( bool bShowCursorWhenFullScreen, bool bClipCursorWhenFullScreen );

typedef enum AdapterOrdinal {
  D3DADAPTER_DEFAULT = 0
} AdapterOrdinal;

#include "Substitutes/GameLoop.h"

HRESULT DXUTCreateDevice( UINT AdapterOrdinal = D3DADAPTER_DEFAULT, bool bWindowed = true, 
                          int nSuggestedWidth = 0, int nSuggestedHeight = 0,
                          LPDXUTCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable = NULL,
                          LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings = NULL, 
                          void* pUserContext = NULL );

#include "Substitutes/Window.h"

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
);

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif
#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif
#ifndef V
#define V(x)           { hr = x; }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
#endif

// From the Windows API: https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/time-time32-time64?view=msvc-160
// Also see https://en.cppreference.com/w/c/chrono/time_t
typedef long time_t;
time_t time( time_t *destTime );
void srand(unsigned int seed);

HRESULT StringCchPrintf( LPTSTR pszDest, size_t cchDest, LPCTSTR pszFormat, ... );
