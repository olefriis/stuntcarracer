#include "../dxstdafx.h"

static ALCdevice *device;
static ALCcontext *context;

void checkError(char *description) {
	// Let's just check in general if anything bad happened. Not super nice, I know.
	switch (alGetError()) {
	case AL_NO_ERROR:
		break;
	case AL_INVALID_NAME:
		ErrorPrintf("%s: AL Invalid name\n", description);
		break;
	case AL_INVALID_ENUM:
		ErrorPrintf("%s: AL Invalid enum\n", description);
		break;
	case AL_INVALID_VALUE:
		ErrorPrintf("%s: AL Invalid value\n", description);
		break;
	case AL_INVALID_OPERATION:
		ErrorPrintf("%s: AL Invalid operation\n", description);
		break;
	case AL_OUT_OF_MEMORY:
		ErrorPrintf("%s: AL out of memory\n", description);
		break;
	}
}

void SetUpDirectSound() {
	device = alcOpenDevice(NULL);
	if (!device) {
        Error("Could not create sound device");
		return;
	}

	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		Error("Could not create sound context");
		return;
	}

	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);

	ALfloat listenerOrientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, listenerOrientation);

	checkError("SetUpDirectSound");
}

HRESULT DirectSoundCreate8(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN pUnkOuter) {
	Debug("DirectSoundCreate8");
	*ppDS8 = new IDirectSound8();
	return S_OK;
}

HRESULT IDirectSound8::SetCooperativeLevel(HWND hwnd, DWORD dwLevel) {
	Debug("IDirectSound8::SetCooperativeLevel");
	return S_OK;
}

HRESULT IDirectSound8::CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter) {
	Debug("IDirectSound8::CreateSoundBuffer");
	*ppDSBuffer = new IDirectSoundBuffer8(pcDSBufferDesc);
	return S_OK;
}

void IDirectSound8::Release() {
	Debug("IDirectSound8::Release");
	// Just ignore
}

IDirectSoundBuffer8::IDirectSoundBuffer8(LPCDSBUFFERDESC pcDSBufferDesc) {
	this->bufferSize = pcDSBufferDesc->dwBufferBytes;
	this->buffer = (void*)malloc(this->bufferSize);
	this->originalSamplesPerSecond = pcDSBufferDesc->lpwfxFormat->nSamplesPerSec;
	alGenSources((ALuint)1, &(this->alSource));
	checkError("Sound buffer initialization alGenSources");
	alGenBuffers((ALuint)1, &(this->alBuffer));
	checkError("Sound buffer initialization alGenBuffers");

	alSourcef(this->alSource, AL_PITCH, 1);
	alSourcef(this->alSource, AL_GAIN, 1);
	alSource3f(this->alSource, AL_POSITION, 0, 0, 0);
	alSource3f(this->alSource, AL_VELOCITY, 0, 0, 0);

	checkError("Sound buffer initialization setting up stuff");
}

HRESULT IDirectSoundBuffer8::SetPan(LONG lPan) {
	Debug("IDirectSoundBuffer8::SetPan");
	// Not critical to implement. The Stunt Car Racer code attempts to imitate the Amiga sound channels
	// (two channels are for the left speaker, the other two channels are for the right speaker), but
	// we can probably do without that.
	return S_OK;
}

HRESULT IDirectSoundBuffer8::SetVolume(LONG lVolume) {
	Debug("IDirectSoundBuffer8::SetVolume");
	// Valid values range from 0 (original volume) to –10000 (silence). I have no idea how that maps to
	// the "gain" property of the source, but let's try to just scale it.
	alSourcef(this->alSource, AL_GAIN, (ALfloat)(lVolume + 10000) / 10000.0);
	checkError("IDirectSoundBuffer8::SetFrequency");
	return S_OK;
}

HRESULT IDirectSoundBuffer8::SetFrequency(DWORD dwFrequency) {
	Debug("IDirectSoundBuffer8::SetFrequency");
	// We cannot explicitly set the frequency on the source, but we can set the pitch. So set the
	// pitch relative to the default frequency to get the same effect.
	alSourcef(this->alSource, AL_PITCH, (ALfloat)dwFrequency / (ALfloat)this->originalSamplesPerSecond);
	checkError("IDirectSoundBuffer8::SetFrequency");
	return S_OK;
}

HRESULT IDirectSoundBuffer8::GetCurrentPosition(LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor) {
	Debug("IDirectSoundBuffer8::GetCurrentPosition");
	ALint offset;
	alGetSourcei(this->alSource, AL_BYTE_OFFSET, &offset);
	checkError("IDirectSoundBuffer8::SetCurrentPosition");
	*pdwCurrentWriteCursor = (DWORD) offset;
	return S_OK;
}

HRESULT IDirectSoundBuffer8::SetCurrentPosition(DWORD dwNewPosition) {
	Debug("IDirectSoundBuffer8::SetCurrentPosition");
	// For some reason, the code below crashes a lot. Since it's probably not critical, let's leave it out for now
	/*
	// Ensure the position is actually within the buffer limits
	dwNewPosition %= this->bufferSize;
	alSourcei(this->alSource, AL_BYTE_OFFSET, dwNewPosition);
	checkError("IDirectSoundBuffer8::SetCurrentPosition");*/
	return S_OK;
}

HRESULT IDirectSoundBuffer8::Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) {
	Debug("IDirectSoundBuffer8::Play");
	if (dwFlags == DSBPLAY_LOOPING) {
		alSourcei(this->alSource, AL_LOOPING, AL_TRUE);
	} else if (dwFlags == NULL) {
		alSourcei(this->alSource, AL_LOOPING, AL_FALSE);
	} else {
		ErrorPrintf("Unknown dwFlags: %ld\n", dwFlags);
	}
	alSourcePlay(this->alSource);

	checkError("IDirectSoundBuffer8::Play");
	return S_OK;
}

HRESULT IDirectSoundBuffer8::Stop() {
	Debug("IDirectSoundBuffer8::Stop");
	alSourceStop(this->alSource);
	checkError("IDirectSoundBuffer8::Stop");
	return S_OK;
}

HRESULT IDirectSoundBuffer8::Lock(
        DWORD dwOffset,
        DWORD dwBytes,
        LPVOID *ppvAudioPtr1,
        LPDWORD pdwAudioBytes1,
        LPVOID *ppvAudioPtr2,
        LPDWORD pdwAudioBytes2,
        DWORD dwFlags) {
	Debug("IDirectSoundBuffer8::Lock");
	// From the description of dwBytes in the docs:
	//
	// Size, in bytes, of the portion of the buffer to lock. The buffer is conceptually circular, so this number can
	// exceed the number of bytes between dwOffset and the end of the buffer.
	//
	// Let's not implement that now, and let's only support locking the whole buffer.
	if (dwOffset != 0 || dwBytes != this->bufferSize) {
		ErrorPrintf("Only locking the whole sound buffer from the start is supported. Got dwOffset %ld, dwBytes %ld\n", dwOffset, dwBytes);
		return E_FAIL;
	}
	if (dwFlags != 0) {
		ErrorPrintf("No flags are supported when locking the sound buffer. Got %ld\n", dwFlags);
		return E_FAIL;
	}

	*ppvAudioPtr1 = this->buffer;
	*pdwAudioBytes1 = dwBytes;
	return S_OK;
}

HRESULT IDirectSoundBuffer8::Unlock(
        LPVOID pvAudioPtr1,
        DWORD dwAudioBytes1,
        LPVOID pvAudioPtr2,
        DWORD dwAudioBytes2) {
	Debug("IDirectSoundBuffer8::Unlock");
	// Just guessing at the format. We should probably be able to get that from somewhere...?
	// Also, the frequency should be set elsewhere.
	alBufferData(this->alBuffer, AL_FORMAT_MONO8, this->buffer, this->bufferSize, this->originalSamplesPerSecond);
	// Bind the source to this buffer
	alSourcei(this->alSource, AL_BUFFER, this->alBuffer);

	checkError("IDirectSoundBuffer8::Unlock");
	return S_OK;
}

void IDirectSoundBuffer8::Release() {
	Debug("IDirectSoundBuffer8::Release");
	// Just ignore
}
