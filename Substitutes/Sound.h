class IDirectSound8 {
  public:
  HRESULT SetCooperativeLevel(
         HWND hwnd,
         DWORD dwLevel
  );
  void Release();
};
typedef IDirectSound8 *LPDIRECTSOUND8;

HRESULT DirectSoundCreate8(
    LPCGUID lpcGuidDevice,
    LPDIRECTSOUND8 * ppDS8,
    LPUNKNOWN pUnkOuter
);
