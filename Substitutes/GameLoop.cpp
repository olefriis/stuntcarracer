#include <emscripten.h>
#include <emscripten/html5.h>
#include <cstring>

#include "../dxstdafx.h"
#include "TouchControls.h"

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
static double lastTime = 0;

EM_BOOL one_iter(double time, void* userData) {
	// Can render to the screen here, etc.
	Debug("One iteration");
	setCurrentTime(time);
	updateCanvasSize();

	// Calculate elapsed time in seconds (emscripten_request_animation_frame_loop passes milliseconds)
	double elapsedMs = (lastTime > 0) ? (time - lastTime) : (1000.0 / 60.0);
	lastTime = time;
	// Clamp to avoid huge jumps (e.g., when tab is backgrounded)
	if (elapsedMs > 100.0) elapsedMs = 100.0;
	float elapsedSeconds = (float)(elapsedMs / 1000.0);

	if (frameMoveCallback) {
		frameMoveCallback(DXUTGetD3DDevice(), time, elapsedSeconds, null);
	}

	if (frameRenderCallback) {
		frameRenderCallback(DXUTGetD3DDevice(), time, elapsedSeconds, null);
	}

	updateTouchControls();

	// Return true to keep the loop running.
	return EM_TRUE;
	//puts("Iteration done");
	//return EM_FALSE;
}


// Implemented in StuntCarRacer.cpp
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int );

UINT mapKeyToDirectXChar(const char *key, int keyCode) {
	// Use the modern 'key' string field (reliable in all browsers)
	if (key[0] != '\0') {
		// Arrow keys
		if (strcmp(key, "ArrowLeft") == 0)  return 'S';
		if (strcmp(key, "ArrowRight") == 0) return 'D';
		if (strcmp(key, "ArrowUp") == 0)    return 0x26;   // VK_UP
		if (strcmp(key, "ArrowDown") == 0)  return 0x28;   // VK_DOWN
		if (strcmp(key, "Shift") == 0)      return 0x10;   // VK_SHIFT

		// Single-character keys: A-Z, digits, space
		if (key[1] == '\0') {
			char c = key[0];
			if (c >= 'a' && c <= 'z') return c - 32;  // uppercase
			return (UINT)(unsigned char)c;
		}

		// Named keys
		if (strcmp(key, "Enter") == 0)      return 0x0D;   // VK_RETURN
		if (strcmp(key, "Backspace") == 0)   return 0x08;   // VK_BACK
		if (strcmp(key, "Escape") == 0)      return 0x1B;   // VK_ESCAPE
		if (strcmp(key, "F1") == 0)          return 0x70;   // VK_F1
		if (strcmp(key, "F2") == 0)          return 0x71;
		if (strcmp(key, "F3") == 0)          return 0x72;
		if (strcmp(key, "F4") == 0)          return 0x73;
		if (strcmp(key, "F5") == 0)          return 0x74;
		if (strcmp(key, "F6") == 0)          return 0x75;
		if (strcmp(key, "F7") == 0)          return 0x76;
		if (strcmp(key, "F8") == 0)          return 0x77;
		if (strcmp(key, "F9") == 0)          return 0x78;
		if (strcmp(key, "F10") == 0)         return 0x79;
	}

	// Fallback to deprecated keyCode for anything unrecognised
	return keyCode;
}

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
	if (keyboardCallback) {
		UINT directXKeyCode = mapKeyToDirectXChar(e->key, e->keyCode);
		bool isDown = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
		keyboardCallback(directXKeyCode, isDown, e->altKey, NULL);
	} else {
		Error("No keyboard callback set");
	}
	// Suppress default browser action for game keys (prevents page scroll etc.)
	UINT mapped = mapKeyToDirectXChar(e->key, e->keyCode);
	if (mapped == 0x26 || mapped == 0x28 || mapped == 'S' || mapped == 'D' ||
	    mapped == 0x10 || mapped == ' ')
		return EM_TRUE;
	return EM_FALSE;
}

EM_JS(void, call_alert, (), {
	specialHTMLTargets["!canvas"] = Module.canvas;
	alert('hello world!');
});

int main() {
	Debug("Main function!");
	SetUpDirectSound();

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

	initTouchControls();

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
