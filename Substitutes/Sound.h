#include <AL/al.h>
#include <AL/alc.h>

typedef struct tWAVEFORMATEX {
  WORD  wFormatTag;
  WORD  nChannels;
  DWORD nSamplesPerSec;
  DWORD nAvgBytesPerSec;
  WORD  nBlockAlign;
  WORD  wBitsPerSample;
  WORD  cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;

typedef struct DSBUFFERDESC {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    LPWAVEFORMATEX lpwfxFormat;
    //GUID guid3DAlgorithm;
} DSBUFFERDESC;

typedef struct waveformat_tag {
  WORD  wFormatTag;
  WORD  nChannels;
  DWORD nSamplesPerSec;
  DWORD nAvgBytesPerSec;
  WORD  nBlockAlign;
} WAVEFORMAT;

typedef struct DSBUFFERDESC *LPCDSBUFFERDESC;

// No idea about the actual values here
#define DSBCAPS_CTRLFREQUENCY   0x00000010
#define DSBCAPS_CTRLPAN         0x00000020
#define DSBCAPS_CTRLVOLUME      0x00000040

class IDirectSoundBuffer8 {
	public:
	IDirectSoundBuffer8(LPCDSBUFFERDESC pcDSBufferDesc);
	HRESULT SetPan(LONG lPan);
	HRESULT SetVolume(LONG lVolume);
	HRESULT SetFrequency(DWORD dwFrequency);
	HRESULT SetCurrentPosition(DWORD dwNewPosition);
	HRESULT Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
	HRESULT Stop();
	HRESULT GetCurrentPosition(LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor);
	HRESULT Lock(
        DWORD dwOffset,
        DWORD dwBytes,
        LPVOID *ppvAudioPtr1,
        LPDWORD pdwAudioBytes1,
        LPVOID *ppvAudioPtr2,
        LPDWORD pdwAudioBytes2,
        DWORD dwFlags
	);
	HRESULT Unlock(
        LPVOID pvAudioPtr1,
        DWORD dwAudioBytes1,
        LPVOID pvAudioPtr2,
        DWORD dwAudioBytes2
	);
	void Release();

	private:
	DWORD bufferSize;
	void* buffer;
	DWORD originalSamplesPerSecond;
	ALuint alSource;
	ALuint alBuffer;
};

typedef struct IDirectSoundBuffer8 *LPDIRECTSOUNDBUFFER;
typedef struct IDirectSoundBuffer8 *LPDIRECTSOUNDBUFFER8;

class IDirectSound8 {
  public:
  HRESULT SetCooperativeLevel(
        HWND hwnd,
        DWORD dwLevel
  );
  HRESULT CreateSoundBuffer(
        LPCDSBUFFERDESC pcDSBufferDesc,
        LPDIRECTSOUNDBUFFER *ppDSBuffer,
        LPUNKNOWN pUnkOuter
  );
  void Release();
};
typedef IDirectSound8 *LPDIRECTSOUND8;

#define DSBPLAY_LOOPING 1L

HRESULT DirectSoundCreate8(
    LPCGUID lpcGuidDevice,
    LPDIRECTSOUND8 * ppDS8,
    LPUNKNOWN pUnkOuter
);

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif
#ifndef mmioFOURCC
#define mmioFOURCC(ch0, ch1, ch2, ch3) MAKEFOURCC(ch0, ch1, ch2, ch3)
#endif

void SetUpDirectSound();
