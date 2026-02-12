// Text rendering via a 2D canvas overlay on top of the WebGL canvas.

typedef struct POINT {
  FLOAT x;
  FLOAT y;
} POINT;

class ID3DXFont {
  public:
  INT m_height;
  ID3DXFont(INT height) : m_height(height) {}
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
    int m_nLineHeight;
    ID3DXFont* m_pFont;
};

HRESULT D3DXCreateSprite(
  LPDIRECT3DDEVICE9 pDevice,
  LPD3DXSPRITE      *ppSprite
);

LPCWSTR DXUTGetFrameStats( bool bIncludeFPS = false );
LPCWSTR DXUTGetDeviceStats();

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
