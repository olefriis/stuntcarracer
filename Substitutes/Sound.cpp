
#include "../dxstdafx.h"

HRESULT DirectSoundCreate8( LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN pUnkOuter ) {
  puts("DirectSoundCreate8");

  *ppDS8 = new IDirectSound8();

  return S_OK;
}

HRESULT IDirectSound8::SetCooperativeLevel( HWND hwnd, DWORD dwLevel ) {
  puts("IDirectSound8::SetCooperativeLevel");
  return S_OK;
}

void IDirectSound8::Release() {
  puts("IDirectSound8::Release");

  // Are we supposed to "destroy" the object here?
}
