#include <emscripten.h>
#include <emscripten/html5.h>

#include "../dxstdafx.h"

static LPDXUTCALLBACKDEVICERESET deviceResetCallback;
static LPDXUTCALLBACKFRAMEMOVE frameMoveCallback;
static LPDXUTCALLBACKFRAMERENDER frameRenderCallback;

void DXUTSetCallbackDeviceCreated( LPDXUTCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext ) {
	puts("DXUTSetCallbackDeviceCreated");
}

void DXUTSetCallbackDeviceReset( LPDXUTCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext ) {
	puts("DXUTSetCallbackDeviceReset");
	deviceResetCallback = pCallbackDeviceReset;
}

void DXUTSetCallbackDeviceLost( LPDXUTCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext ) {
	puts("DXUTSetCallbackDeviceLost");
}

void DXUTSetCallbackDeviceDestroyed( LPDXUTCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext ) {
	puts("DXUTSetCallbackDeviceDestroyed");
}

void DXUTSetCallbackDeviceChanging( LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings, void* pUserContext ) {
	puts("DXUTSetCallbackDeviceChanging");
}

void DXUTSetCallbackFrameMove( LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext ) {
	puts("DXUTSetCallbackFrameMove");
	frameMoveCallback = pCallbackFrameMove;
}

void DXUTSetCallbackFrameRender( LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext ) {
	puts("DXUTSetCallbackFrameRender");
	frameRenderCallback = pCallbackFrameRender;
}

void DXUTSetCallbackKeyboard( LPDXUTCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext ) {
	puts("DXUTSetCallbackKeyboard");
}

void DXUTSetCallbackMouse( LPDXUTCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove, void* pUserContext ) {
	puts("DXUTSetCallbackMouse");
}

void DXUTSetCallbackMsgProc( LPDXUTCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext ) {
	puts("DXUTSetCallbackMsgProc");
}


// Our "main loop" function. This callback receives the current time as
// reported by the browser, and the user data we provide in the call to
// emscripten_request_animation_frame_loop().
EM_BOOL one_iter(double time, void* userData) {
	// Can render to the screen here, etc.
	puts("One iteration");

	if (frameMoveCallback) {
		frameMoveCallback(DXUTGetD3DDevice(), time, time, null);
	}

	if (frameRenderCallback) {
		frameRenderCallback(DXUTGetD3DDevice(), time, time, null);
	}

	// Return true to keep the loop running.
	//return EM_TRUE;
	// Well, return false right now to allow for some debugging
	puts("Iteration done");
	return EM_FALSE;
}


// Implemented in StuntCarRacer.cpp
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int );

int main() {
	puts("Main function!");
	if (WinMain( NULL, NULL, NULL, NULL ) == FALSE) {
		puts("WinMain failed");
		return 0;
	}

	SetUpD3DDevice();

	if (deviceResetCallback) {
		puts("Resetting device");

		HRESULT resetDeviceResult = deviceResetCallback(DXUTGetD3DDevice(), DXUTGetBackBufferSurfaceDesc(), null);
		if (resetDeviceResult != S_OK) {
			printf("Device reset callback returned failure code %ld\n", resetDeviceResult);
			return resetDeviceResult;
		}
	}

	// Receives a function to call and some user data to provide it.
	emscripten_request_animation_frame_loop(one_iter, 0);
}
