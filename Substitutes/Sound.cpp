
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

HRESULT IDirectSoundBuffer8::SetPan( LONG lPan ) {
  return S_OK;
}

HRESULT IDirectSoundBuffer8::SetVolume( LONG lVolume ) {
  return S_OK;
}

HRESULT IDirectSoundBuffer8::SetFrequency( DWORD dwFrequency ) {
  return S_OK;
}

HRESULT IDirectSoundBuffer8::SetCurrentPosition( DWORD dwNewPosition ) {
  return S_OK;
}

HRESULT IDirectSoundBuffer8::Play( DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags ) {
  return S_OK;
}

HRESULT IDirectSoundBuffer8::Stop() {
  return S_OK;
}

HRESULT IDirectSoundBuffer8::GetCurrentPosition( LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor ) {
  return S_OK;
}

void IDirectSoundBuffer8::Release() {
  puts("IDirectSoundBuffer8::Release");

  // Are we supposed to "destroy" the object here?
}
