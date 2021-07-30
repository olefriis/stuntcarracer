class IDirectSound8 {
  public:
  HRESULT SetCooperativeLevel(
         HWND hwnd,
         DWORD dwLevel
  );
  void Release();
};
typedef IDirectSound8 *LPDIRECTSOUND8;

#define DSBPLAY_LOOPING 1L

class IDirectSoundBuffer8 {
  public:
  HRESULT SetPan( LONG lPan );
  HRESULT SetVolume( LONG lVolume );
  HRESULT SetFrequency( DWORD dwFrequency );
  HRESULT SetCurrentPosition( DWORD dwNewPosition );
  HRESULT Play( DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags );
  HRESULT Stop();
  HRESULT GetCurrentPosition( LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor );
  void Release();
};

HRESULT DirectSoundCreate8(
    LPCGUID lpcGuidDevice,
    LPDIRECTSOUND8 * ppDS8,
    LPUNKNOWN pUnkOuter
);
