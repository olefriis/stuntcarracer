struct XyzrhwDiffuseOriginal {
	FLOAT x, y, z, w;
	DWORD color;
};

struct XyzrhwDiffuseConverted {
	FLOAT x, y, z, w;
	FLOAT red, green, blue, alpha;
};

struct XyzDiffuseTextureOriginal {
	FLOAT x, y, z;
	DWORD color;
	FLOAT tu,tv;
};

struct XyzDiffuseTextureConverted {
	FLOAT x, y, z;
	FLOAT red, green, blue, alpha;
	FLOAT tu, tv;
};

class IDirect3DVertexBuffer9 {
  public:
  IDirect3DVertexBuffer9(UINT Length);
  HRESULT Lock( UINT  OffsetToLock, UINT  SizeToLock, void  **ppbData, DWORD Flags );
  HRESULT Unlock();
  void Release();

  // Should probably be behind accessor, or be friend properties...
  UINT originalLength;
  void *originalData;
  void *convertedData;
  unsigned int vbo;
  UINT dirtySize;
  void PrepareForForXyzDiffuseTexture(UINT stride);
  void PrepareForXyzrhwDiffuse(UINT stride);
};
