#include <math.h>
#include <stdlib.h>
#include <iostream>

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

typedef enum D3DTRANSFORMSTATETYPE { 
  D3DTS_VIEW         = 2,
  D3DTS_PROJECTION   = 3,
  D3DTS_TEXTURE0     = 16,
  D3DTS_TEXTURE1     = 17,
  D3DTS_TEXTURE2     = 18,
  D3DTS_TEXTURE3     = 19,
  D3DTS_TEXTURE4     = 20,
  D3DTS_TEXTURE5     = 21,
  D3DTS_TEXTURE6     = 22,
  D3DTS_TEXTURE7     = 23,

  // I made this up. No idea how it's actually implemented
  D3DTS_WORLD        = 24,

  D3DTS_FORCE_DWORD  = 0x7fffffff
} D3DTRANSFORMSTATETYPE, *LPD3DTRANSFORMSTATETYPE;

typedef enum D3DCULL { 
  D3DCULL_NONE         = 1,
  D3DCULL_CW           = 2,
  D3DCULL_CCW          = 3,
  D3DCULL_FORCE_DWORD  = 0x7fffffff
} D3DCULL, *LPD3DCULL;

typedef enum D3DRENDERSTATETYPE { 
  D3DRS_ZENABLE                     = 7,
  D3DRS_FILLMODE                    = 8,
  D3DRS_SHADEMODE                   = 9,
  D3DRS_ZWRITEENABLE                = 14,
  D3DRS_ALPHATESTENABLE             = 15,
  D3DRS_LASTPIXEL                   = 16,
  D3DRS_SRCBLEND                    = 19,
  D3DRS_DESTBLEND                   = 20,
  D3DRS_CULLMODE                    = 22,
  D3DRS_ZFUNC                       = 23,
  D3DRS_ALPHAREF                    = 24,
  D3DRS_ALPHAFUNC                   = 25,
  D3DRS_DITHERENABLE                = 26,
  D3DRS_ALPHABLENDENABLE            = 27,
  D3DRS_FOGENABLE                   = 28,
  D3DRS_SPECULARENABLE              = 29,
  D3DRS_FOGCOLOR                    = 34,
  D3DRS_FOGTABLEMODE                = 35,
  D3DRS_FOGSTART                    = 36,
  D3DRS_FOGEND                      = 37,
  D3DRS_FOGDENSITY                  = 38,
  D3DRS_RANGEFOGENABLE              = 48,
  D3DRS_STENCILENABLE               = 52,
  D3DRS_STENCILFAIL                 = 53,
  D3DRS_STENCILZFAIL                = 54,
  D3DRS_STENCILPASS                 = 55,
  D3DRS_STENCILFUNC                 = 56,
  D3DRS_STENCILREF                  = 57,
  D3DRS_STENCILMASK                 = 58,
  D3DRS_STENCILWRITEMASK            = 59,
  D3DRS_TEXTUREFACTOR               = 60,
  D3DRS_WRAP0                       = 128,
  D3DRS_WRAP1                       = 129,
  D3DRS_WRAP2                       = 130,
  D3DRS_WRAP3                       = 131,
  D3DRS_WRAP4                       = 132,
  D3DRS_WRAP5                       = 133,
  D3DRS_WRAP6                       = 134,
  D3DRS_WRAP7                       = 135,
  D3DRS_CLIPPING                    = 136,
  D3DRS_LIGHTING                    = 137,
  D3DRS_AMBIENT                     = 139,
  D3DRS_FOGVERTEXMODE               = 140,
  D3DRS_COLORVERTEX                 = 141,
  D3DRS_LOCALVIEWER                 = 142,
  D3DRS_NORMALIZENORMALS            = 143,
  D3DRS_DIFFUSEMATERIALSOURCE       = 145,
  D3DRS_SPECULARMATERIALSOURCE      = 146,
  D3DRS_AMBIENTMATERIALSOURCE       = 147,
  D3DRS_EMISSIVEMATERIALSOURCE      = 148,
  D3DRS_VERTEXBLEND                 = 151,
  D3DRS_CLIPPLANEENABLE             = 152,
  D3DRS_POINTSIZE                   = 154,
  D3DRS_POINTSIZE_MIN               = 155,
  D3DRS_POINTSPRITEENABLE           = 156,
  D3DRS_POINTSCALEENABLE            = 157,
  D3DRS_POINTSCALE_A                = 158,
  D3DRS_POINTSCALE_B                = 159,
  D3DRS_POINTSCALE_C                = 160,
  D3DRS_MULTISAMPLEANTIALIAS        = 161,
  D3DRS_MULTISAMPLEMASK             = 162,
  D3DRS_PATCHEDGESTYLE              = 163,
  D3DRS_DEBUGMONITORTOKEN           = 165,
  D3DRS_POINTSIZE_MAX               = 166,
  D3DRS_INDEXEDVERTEXBLENDENABLE    = 167,
  D3DRS_COLORWRITEENABLE            = 168,
  D3DRS_TWEENFACTOR                 = 170,
  D3DRS_BLENDOP                     = 171,
  D3DRS_POSITIONDEGREE              = 172,
  D3DRS_NORMALDEGREE                = 173,
  D3DRS_SCISSORTESTENABLE           = 174,
  D3DRS_SLOPESCALEDEPTHBIAS         = 175,
  D3DRS_ANTIALIASEDLINEENABLE       = 176,
  D3DRS_MINTESSELLATIONLEVEL        = 178,
  D3DRS_MAXTESSELLATIONLEVEL        = 179,
  D3DRS_ADAPTIVETESS_X              = 180,
  D3DRS_ADAPTIVETESS_Y              = 181,
  D3DRS_ADAPTIVETESS_Z              = 182,
  D3DRS_ADAPTIVETESS_W              = 183,
  D3DRS_ENABLEADAPTIVETESSELLATION  = 184,
  D3DRS_TWOSIDEDSTENCILMODE         = 185,
  D3DRS_CCW_STENCILFAIL             = 186,
  D3DRS_CCW_STENCILZFAIL            = 187,
  D3DRS_CCW_STENCILPASS             = 188,
  D3DRS_CCW_STENCILFUNC             = 189,
  D3DRS_COLORWRITEENABLE1           = 190,
  D3DRS_COLORWRITEENABLE2           = 191,
  D3DRS_COLORWRITEENABLE3           = 192,
  D3DRS_BLENDFACTOR                 = 193,
  D3DRS_SRGBWRITEENABLE             = 194,
  D3DRS_DEPTHBIAS                   = 195,
  D3DRS_WRAP8                       = 198,
  D3DRS_WRAP9                       = 199,
  D3DRS_WRAP10                      = 200,
  D3DRS_WRAP11                      = 201,
  D3DRS_WRAP12                      = 202,
  D3DRS_WRAP13                      = 203,
  D3DRS_WRAP14                      = 204,
  D3DRS_WRAP15                      = 205,
  D3DRS_SEPARATEALPHABLENDENABLE    = 206,
  D3DRS_SRCBLENDALPHA               = 207,
  D3DRS_DESTBLENDALPHA              = 208,
  D3DRS_BLENDOPALPHA                = 209,
  D3DRS_FORCE_DWORD                 = 0x7fffffff
} D3DRENDERSTATETYPE, *LPD3DRENDERSTATETYPE;


typedef enum D3DSHADEMODE { 
  D3DSHADE_FLAT         = 1,
  D3DSHADE_GOURAUD      = 2,
  D3DSHADE_PHONG        = 3,
  D3DSHADE_FORCE_DWORD  = 0x7fffffff
} D3DSHADEMODE, *LPD3DSHADEMODE;

typedef enum D3DTEXTURESTAGESTATETYPE { 
  D3DTSS_COLOROP                = 1,
  D3DTSS_COLORARG1              = 2,
  D3DTSS_COLORARG2              = 3,
  D3DTSS_ALPHAOP                = 4,
  D3DTSS_ALPHAARG1              = 5,
  D3DTSS_ALPHAARG2              = 6,
  D3DTSS_BUMPENVMAT00           = 7,
  D3DTSS_BUMPENVMAT01           = 8,
  D3DTSS_BUMPENVMAT10           = 9,
  D3DTSS_BUMPENVMAT11           = 10,
  D3DTSS_TEXCOORDINDEX          = 11,
  D3DTSS_BUMPENVLSCALE          = 22,
  D3DTSS_BUMPENVLOFFSET         = 23,
  D3DTSS_TEXTURETRANSFORMFLAGS  = 24,
  D3DTSS_COLORARG0              = 26,
  D3DTSS_ALPHAARG0              = 27,
  D3DTSS_RESULTARG              = 28,
  D3DTSS_CONSTANT               = 32,
  D3DTSS_FORCE_DWORD            = 0x7fffffff
} D3DTEXTURESTAGESTATETYPE, *LPD3DTEXTURESTAGESTATETYPE;

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

// A bunch of types I've just made up to make stuff compile a bit.
// They for sure won't work, but their use will have to be changed to something more HTML-friendly anyway.

typedef struct _D3DMATRIX {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
        float m[4][4];
    };
} D3DMATRIX;

class IDirect3DVertexBuffer9 {
  public:
  virtual HRESULT Lock( UINT  OffsetToLock, UINT  SizeToLock, void  **ppbData, DWORD Flags );
  virtual HRESULT Unlock();
  virtual void Release();
};

typedef struct D3DCAPS9 {
  D3DDEVTYPE DeviceType;
  UINT       AdapterOrdinal;
} D3DCAPS9;
typedef struct D3DFORMAT {
  FLOAT something;
} D3DFORMAT;
typedef struct D3DRECT {
  LONG x1;
  LONG y1;
  LONG x2;
  LONG y2;
} D3DRECT;
typedef enum D3DPOOL { 
  D3DPOOL_DEFAULT      = 0,
  D3DPOOL_MANAGED      = 1,
  D3DPOOL_SYSTEMMEM    = 2,
  D3DPOOL_SCRATCH      = 3,
  D3DPOOL_FORCE_DWORD  = 0x7fffffff
} D3DPOOL, *LPD3DPOOL;

// No idea what the real value is
#define D3DCLEAR_TARGET  0x00000001
#define D3DCLEAR_ZBUFFER 0x00000002

// Vertex data flags. See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dfvf
// Just setting them to bogus values here.
// D3DFVF_XYZRHW: float, float, float, float.
#define D3DFVF_XYZRHW 1
// D3DFVF_DIFFUSE: DWORD in ARGB order.
#define D3DFVF_DIFFUSE 2
// D3DFVF_XYZ: float, float, float.
#define D3DFVF_XYZ 4
// D3DFVF_TEX0 - D3DFVF_TEX8: Number of texture coordinate sets for this vertex. The actual values for these flags are not sequential.
#define D3DFVF_TEX1 8

// D3DUSAGE: https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dusage
#define D3DUSAGE_WRITEONLY 1

typedef enum D3DPRIMITIVETYPE { 
  D3DPT_POINTLIST      = 1,
  D3DPT_LINELIST       = 2,
  D3DPT_LINESTRIP      = 3,
  D3DPT_TRIANGLELIST   = 4,
  D3DPT_TRIANGLESTRIP  = 5,
  D3DPT_TRIANGLEFAN    = 6,
  D3DPT_FORCE_DWORD    = 0x7fffffff
} D3DPRIMITIVETYPE, *LPD3DPRIMITIVETYPE;

// D3DTA: Texture argument constants are used as values for the following members of the D3DTEXTURESTAGESTATETYPE enumerated type.
// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dta
#define D3DTA_TEXTURE 1
#define D3DTA_DIFFUSE 2

class IDirect3DBaseTexture9 {
  public:
};
class IDirect3DTexture9 : public IDirect3DBaseTexture9 {
  public:
  virtual void Release();
};
typedef IDirect3DTexture9 *LPDIRECT3DTEXTURE9;

class IDirect3DDevice9 {
  public:
  virtual HRESULT SetTransform(
    D3DTRANSFORMSTATETYPE State,
    const D3DMATRIX       *pMatrix
  );
  virtual HRESULT SetRenderState(
    D3DRENDERSTATETYPE State,
    DWORD              Value
  );
  virtual HRESULT SetTextureStageState(
    DWORD                    Stage,
    D3DTEXTURESTAGESTATETYPE Type,
    DWORD                    Value
  );
  virtual HRESULT SetTexture(
    DWORD                 Stage,
    IDirect3DBaseTexture9 *pTexture
  );
  virtual HRESULT BeginScene();
  virtual HRESULT EndScene();
  virtual HRESULT Clear(
    DWORD         Count,
    const D3DRECT *pRects,
    DWORD         Flags,
    D3DCOLOR      Color,
    float         Z,
    DWORD         Stencil
  );
  virtual HRESULT CreateVertexBuffer(
    UINT                   Length,
    DWORD                  Usage,
    DWORD                  FVF,
    D3DPOOL                Pool,
    IDirect3DVertexBuffer9 **ppVertexBuffer,
    HANDLE                 *pSharedHandle
  );
  virtual HRESULT SetFVF(
    DWORD FVF
  );
  virtual HRESULT SetStreamSource(
    UINT                   StreamNumber,
    IDirect3DVertexBuffer9 *pStreamData,
    UINT                   OffsetInBytes,
    UINT                   Stride
  );
  virtual HRESULT DrawPrimitive(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT             StartVertex,
    UINT             PrimitiveCount
  );
};
typedef IDirect3DDevice9 *LPDIRECT3DDEVICE9;
typedef IDirect3DDevice9 *PDIRECT3DDEVICE9;
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
typedef struct D3DVECTOR {
  FLOAT x;
  FLOAT y;
  FLOAT z;
} D3DVECTOR, *LPD3DXVECTOR3;
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
typedef struct D3DXVECTOR3 : public D3DVECTOR
{
public:
    D3DXVECTOR3() {};
    D3DXVECTOR3( FLOAT x, FLOAT y, FLOAT z ) {};
} D3DXVECTOR3;
typedef struct D3DXMATRIX : public D3DMATRIX
{
public:
    D3DXMATRIX() {};
} D3DXMATRIX, *LPD3DXMATRIX;

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

D3DXMATRIX* D3DXMatrixPerspectiveFovLH(
  D3DXMATRIX *pOut,
  FLOAT      fovy,
  FLOAT      Aspect,
  FLOAT      zn,
  FLOAT      zf
);

D3DXMATRIX* D3DXMatrixIdentity(
  D3DXMATRIX *pOut
);

D3DXMATRIX* D3DXMatrixRotationX(
  D3DXMATRIX *pOut,
  FLOAT      Angle
);

D3DXMATRIX* D3DXMatrixRotationY(
  D3DXMATRIX *pOut,
  FLOAT      Angle
);

D3DXMATRIX* D3DXMatrixRotationZ(
  D3DXMATRIX *pOut,
  FLOAT      Angle
);

D3DXMATRIX* D3DXMatrixTranslation(
  D3DXMATRIX *pOut,
  FLOAT      x,
  FLOAT      y,
  FLOAT      z
);

D3DXMATRIX* D3DXMatrixLookAtLH(
  D3DXMATRIX  *pOut,
  const D3DXVECTOR3 *pEye,
  const D3DXVECTOR3 *pAt,
  const D3DXVECTOR3 *pUp
);

D3DXMATRIX* D3DXMatrixMultiply(
  D3DXMATRIX *pOut,
  const D3DXMATRIX *pM1,
  const D3DXMATRIX *pM2
);

HWND DXUTGetHWND();

LPCWSTR DXUTGetFrameStats( bool bIncludeFPS = false );
LPCWSTR DXUTGetDeviceStats();

CONST D3DSURFACE_DESC * DXUTGetBackBufferSurfaceDesc();
D3DCOLOR D3DCOLOR_XRGB(
   int r,
   int g,
   int b
);

IDirect3D9* DXUTGetD3DObject();

IDirect3DDevice9* DXUTGetD3DDevice();

double DXUTGetTime();

void DXUTDisplaySwitchingToREFWarning();

HRESULT DXUTReset3DEnvironment();

//--------------------------------------------------------------------------------------
// Callback registration 
//--------------------------------------------------------------------------------------
typedef bool    (CALLBACK *LPDXUTCALLBACKISDEVICEACCEPTABLE)( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
typedef bool    (CALLBACK *LPDXUTCALLBACKMODIFYDEVICESETTINGS)( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
typedef bool    (CALLBACK *LPDXUTCALLBACKMODIFYDEVICESETTINGS)( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
typedef HRESULT (CALLBACK *LPDXUTCALLBACKDEVICECREATED)( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
typedef HRESULT (CALLBACK *LPDXUTCALLBACKDEVICERESET)( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
typedef void    (CALLBACK *LPDXUTCALLBACKDEVICEDESTROYED)( void* pUserContext );
typedef void    (CALLBACK *LPDXUTCALLBACKDEVICELOST)( void* pUserContext );

// Device callbacks
void DXUTSetCallbackDeviceCreated( LPDXUTCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext = NULL );
void DXUTSetCallbackDeviceReset( LPDXUTCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext = NULL );
void DXUTSetCallbackDeviceLost( LPDXUTCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext = NULL );
void DXUTSetCallbackDeviceDestroyed( LPDXUTCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext = NULL );
void DXUTSetCallbackDeviceChanging( LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings, void* pUserContext = NULL );

typedef void    (CALLBACK *LPDXUTCALLBACKFRAMEMOVE)( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
typedef void    (CALLBACK *LPDXUTCALLBACKFRAMERENDER)( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );

// Frame callbacks
void DXUTSetCallbackFrameMove( LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext = NULL );
void DXUTSetCallbackFrameRender( LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext = NULL );

typedef void    (CALLBACK *LPDXUTCALLBACKKEYBOARD)( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
typedef void    (CALLBACK *LPDXUTCALLBACKMOUSE)( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext );
typedef LRESULT (CALLBACK *LPDXUTCALLBACKMSGPROC)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );

// Message callbacks
void DXUTSetCallbackKeyboard( LPDXUTCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext = NULL );
void DXUTSetCallbackMouse( LPDXUTCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove = false, void* pUserContext = NULL );
void DXUTSetCallbackMsgProc( LPDXUTCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext = NULL );

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

HRESULT DXUTCreateDevice( UINT AdapterOrdinal = D3DADAPTER_DEFAULT, bool bWindowed = true, 
                          int nSuggestedWidth = 0, int nSuggestedHeight = 0,
                          LPDXUTCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable = NULL,
                          LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings = NULL, 
                          void* pUserContext = NULL );

HRESULT DXUTCreateWindow( const WCHAR* strWindowTitle );

HRESULT DXUTMainLoop();

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
