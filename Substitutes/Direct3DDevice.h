typedef enum D3DTRANSFORMSTATETYPE { 
	D3DTS_VIEW         = 2,
	D3DTS_PROJECTION   = 3,

	// I made this up. No idea how it's actually implemented
	D3DTS_WORLD        = 24,

	D3DTS_FORCE_DWORD  = 0x7fffffff
} D3DTRANSFORMSTATETYPE, *LPD3DTRANSFORMSTATETYPE;

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

// D3DTA: Texture argument constants are used as values for the following members of the D3DTEXTURESTAGESTATETYPE enumerated type.
// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dta
#define D3DTA_TEXTURE 1
#define D3DTA_DIFFUSE 2

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
	D3DPT_LINELIST       = 2,
	D3DPT_LINESTRIP      = 3,
	D3DPT_TRIANGLELIST   = 4,
	D3DPT_TRIANGLESTRIP  = 5,
	D3DPT_TRIANGLEFAN    = 6,
} D3DPRIMITIVETYPE, *LPD3DPRIMITIVETYPE;

class IDirect3DDevice9 {
  public:
  HRESULT SetTransform(
    D3DTRANSFORMSTATETYPE State,
    const D3DMATRIX       *pMatrix
  );
  HRESULT SetRenderState(
    D3DRENDERSTATETYPE State,
    DWORD              Value
  );
  HRESULT SetTextureStageState(
    DWORD                    Stage,
    D3DTEXTURESTAGESTATETYPE Type,
    DWORD                    Value
  );
  HRESULT SetTexture(
    DWORD                 Stage,
    IDirect3DBaseTexture9 *pTexture
  );
  HRESULT BeginScene();
  HRESULT EndScene();
  HRESULT Clear(
    DWORD         Count,
    const D3DRECT *pRects,
    DWORD         Flags,
    D3DCOLOR      Color,
    float         Z,
    DWORD         Stencil
  );
  HRESULT CreateVertexBuffer(
    UINT                   Length,
    DWORD                  Usage,
    DWORD                  FVF,
    D3DPOOL                Pool,
    IDirect3DVertexBuffer9 **ppVertexBuffer,
    HANDLE                 *pSharedHandle
  );
  HRESULT SetFVF(
    DWORD FVF
  );
  HRESULT SetStreamSource(
    UINT                   StreamNumber,
    IDirect3DVertexBuffer9 *pStreamData,
    UINT                   OffsetInBytes,
    UINT                   Stride
  );
  HRESULT DrawPrimitive(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT             StartVertex,
    UINT             PrimitiveCount
  );

  private:
  D3DXMATRIX viewMatrix;
  D3DXMATRIX worldMatrix;
  D3DXMATRIX projectionMatrix;
  IDirect3DVertexBuffer9 *currentStreamSource;
  UINT currentStride;
  DWORD currentFvf;
};
typedef IDirect3DDevice9 *LPDIRECT3DDEVICE9;
typedef IDirect3DDevice9 *PDIRECT3DDEVICE9;

// Created for our own use
void SetUpD3DDevice();

IDirect3DDevice9* DXUTGetD3DDevice();
