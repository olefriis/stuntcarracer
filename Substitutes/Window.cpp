#include "../dxstdafx.h"

// This stuff is copied from the "glbook" code from emscripten's source code repo
#include "esUtil.h"
static ESContext esContext;

HRESULT DXUTCreateWindow( const WCHAR* strWindowTitle ) {
  puts("DXUTCreateWindow");

  esCreateWindow(&esContext, "Stunt Car Racer", 320, 240, ES_WINDOW_RGB);
  return S_OK;
}

int getCurrentWindowWidth() {
	return esContext.width;
}

int getCurrentWindowHeight() {
	return esContext.height;
}
