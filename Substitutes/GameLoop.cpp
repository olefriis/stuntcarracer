#include <emscripten.h>
#include <emscripten/html5.h>

#include "../dxstdafx.h"

static LPDXUTCALLBACKDEVICERESET deviceResetCallback;
static LPDXUTCALLBACKFRAMEMOVE frameMoveCallback;
static LPDXUTCALLBACKFRAMERENDER frameRenderCallback;
static LPDXUTCALLBACKKEYBOARD keyboardCallback;

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
	keyboardCallback = pCallbackKeyboard;
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
	setCurrentTime(time);
	updateCanvasSize();

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

UINT mapKeyCodeToDirectXChar(int keyCode) {
	// Map arrow keys to S (left), D (right), Enter (up)
	if (keyCode == 37) { // Left
		return 'S';
	}
	if (keyCode == 39) { // Right
		return 'D';
	}
	if (keyCode == 38) { // Up
		return 0x0D;
	}

	// Maybe a little weird: Map space to "hash key", which gives a backwards boost when used together with the down arrow.
	// At the same time, map arrow down to space. Somehow, this makes sense regarding the steering.
	if (keyCode == 40) { // Down
		return ' ';
	}
	if (keyCode == 32) { // Space
		return 0xDE; // "Hash key"
	}

	// TODO: Map something to F1-F10 (VK_F1-VK_F10)

	// A-Z, the enter key, and the space key are mapped to the same key code
	return keyCode;
}

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
	if (keyboardCallback) {
		UINT directXKeyCode = mapKeyCodeToDirectXChar(e->keyCode);
		if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
			keyboardCallback(directXKeyCode, true, e->altKey, NULL);
		} else if (eventType == EMSCRIPTEN_EVENT_KEYUP) {
			keyboardCallback(directXKeyCode, false, e->altKey, NULL);
		}
	} else {
		Error("No keyboard callback set");
	}
	return 0;
}

EM_JS(void, call_alert, (), {
	specialHTMLTargets["!canvas"] = Module.canvas;
	alert('hello world!');
});

int main() {
	Debug("Main function!");
	if (WinMain( NULL, NULL, NULL, NULL ) == FALSE) {
		Error("WinMain failed");
		return 0;
	}

	SetUpD3DDevice();

	if (emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback) != EMSCRIPTEN_RESULT_SUCCESS) {
		Error("emscripten_set_keydown_callback failed");
	}
  	if (emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback) != EMSCRIPTEN_RESULT_SUCCESS) {
		Error("emscripten_set_keyup_callback failed");
	}

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
