#include "../dxstdafx.h"

// This stuff is copied from the "glbook" code from emscripten's source code repo
#include "esUtil.h"
static ESContext esContext;
static D3DSURFACE_DESC surfaceDescription;

HRESULT DXUTCreateWindow( const WCHAR* strWindowTitle ) {
	Debug("DXUTCreateWindow");

	esCreateWindow(&esContext, "Stunt Car Racer", 320, 240, ES_WINDOW_RGB);

	surfaceDescription.Width = esContext.width;
	surfaceDescription.Height = esContext.height;

	return S_OK;
}

CONST D3DSURFACE_DESC * DXUTGetBackBufferSurfaceDesc() {
	Debug("DXUTGetBackBufferSurfaceDesc");
	return &surfaceDescription;
}
