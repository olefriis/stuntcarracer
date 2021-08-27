#include <emscripten.h>
#include <emscripten/html5.h>

#include "../dxstdafx.h"

static LPDXUTCALLBACKDEVICERESET deviceResetCallback;
static LPDXUTCALLBACKFRAMEMOVE frameMoveCallback;
static LPDXUTCALLBACKFRAMERENDER frameRenderCallback;

void DXUTSetCallbackDeviceCreated( LPDXUTCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext ) {
	Debug("DXUTSetCallbackDeviceCreated");
}

void DXUTSetCallbackDeviceReset( LPDXUTCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext ) {
	Debug("DXUTSetCallbackDeviceReset");
	deviceResetCallback = pCallbackDeviceReset;
}

void DXUTSetCallbackDeviceLost( LPDXUTCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext ) {
	Debug("DXUTSetCallbackDeviceLost");
}

void DXUTSetCallbackDeviceDestroyed( LPDXUTCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext ) {
	Debug("DXUTSetCallbackDeviceDestroyed");
}

void DXUTSetCallbackDeviceChanging( LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings, void* pUserContext ) {
	Debug("DXUTSetCallbackDeviceChanging");
}

void DXUTSetCallbackFrameMove( LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext ) {
	Debug("DXUTSetCallbackFrameMove");
	frameMoveCallback = pCallbackFrameMove;
}

void DXUTSetCallbackFrameRender( LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext ) {
	Debug("DXUTSetCallbackFrameRender");
	frameRenderCallback = pCallbackFrameRender;
}

void DXUTSetCallbackKeyboard( LPDXUTCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext ) {
	Debug("DXUTSetCallbackKeyboard");
}

void DXUTSetCallbackMouse( LPDXUTCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove, void* pUserContext ) {
	Debug("DXUTSetCallbackMouse");
}

void DXUTSetCallbackMsgProc( LPDXUTCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext ) {
	Debug("DXUTSetCallbackMsgProc");
}


// Our "main loop" function. This callback receives the current time as
// reported by the browser, and the user data we provide in the call to
// emscripten_request_animation_frame_loop().
EM_BOOL one_iter(double time, void* userData) {
	// Can render to the screen here, etc.
	Debug("One iteration");

	if (frameMoveCallback) {
		frameMoveCallback(DXUTGetD3DDevice(), time, time, null);
	}

	if (frameRenderCallback) {
		frameRenderCallback(DXUTGetD3DDevice(), time, time, null);
	}

	// Return true to keep the loop running.
	return EM_TRUE;
	//puts("Iteration done");
	//return EM_FALSE;
}


// Implemented in StuntCarRacer.cpp
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int );

int main() {
	Debug("Main function!");
	if (WinMain( NULL, NULL, NULL, NULL ) == FALSE) {
		Error("WinMain failed");
		return 0;
	}

	SetUpD3DDevice();

	if (deviceResetCallback) {
		Debug("Resetting device");

		HRESULT resetDeviceResult = deviceResetCallback(DXUTGetD3DDevice(), DXUTGetBackBufferSurfaceDesc(), null);
		if (resetDeviceResult != S_OK) {
			ErrorPrintf("Device reset callback returned failure code %ld\n", resetDeviceResult);
			return resetDeviceResult;
		}
	}

	// Receives a function to call and some user data to provide it.
	emscripten_request_animation_frame_loop(one_iter, 0);
}
