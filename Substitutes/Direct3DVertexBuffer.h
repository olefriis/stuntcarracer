class IDirect3DVertexBuffer9 {
  public:
  IDirect3DVertexBuffer9(UINT Length);
  HRESULT Lock( UINT  OffsetToLock, UINT  SizeToLock, void  **ppbData, DWORD Flags );
  HRESULT Unlock();
  void Release();

  private:
  void *data;
};
