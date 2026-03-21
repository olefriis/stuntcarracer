#include "../dxstdafx.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GL/glew.h>

static D3DSURFACE_DESC surfaceDescription;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext glContext;

EM_JS(UINT, get_canvas_width, (), { return canvas.width; })
EM_JS(UINT, get_canvas_height, (), { return canvas.height; })
EM_JS(UINT, initial_canvas_width, (), { return window.gameCanvasWidth || (window.production ? window.innerWidth : 320); })
EM_JS(UINT, initial_canvas_height, (), { return window.gameCanvasHeight || (window.production ? window.innerHeight : 240); })
EM_JS(int, is_production, (), { return window.production ? 1 : 0; })

// Thanks to https://codingtidbit.com/2019/08/24/bring-your-c-opengl-code-to-the-web/ for tips!
HRESULT DXUTCreateWindow( const WCHAR* strWindowTitle ) {
	Debug("DXUTCreateWindow");

	surfaceDescription.Width = initial_canvas_width();
	surfaceDescription.Height = initial_canvas_height();

	emscripten_set_canvas_element_size("#canvas", surfaceDescription.Width, surfaceDescription.Height);
	EmscriptenWebGLContextAttributes attr;
	emscripten_webgl_init_context_attributes(&attr);
	attr.alpha = attr.stencil = attr.antialias = attr.preserveDrawingBuffer = attr.failIfMajorPerformanceCaveat = 0;
	attr.depth = 24;
	attr.enableExtensionsByDefault = 1;
	attr.premultipliedAlpha = 0;
	attr.majorVersion = 1;
	attr.minorVersion = 0;
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
	emscripten_webgl_make_context_current(ctx);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		ErrorPrintf("GLEW init failed: %s!\n", glewGetErrorString(err));
		return E_FAIL;
	}

	return S_OK;
}

void updateCanvasSize() {
	// In production mode, sync the canvas to the browser viewport every frame.
	// This handles window resizes on desktop. On iOS, a page reload on rotation
	// (see custom_shell.html) ensures we always start with correct dimensions.
	if (is_production()) {
		UINT vpW = initial_canvas_width();
		UINT vpH = initial_canvas_height();
		int curW, curH;
		emscripten_get_canvas_element_size("#canvas", &curW, &curH);
		if ((int)vpW != curW || (int)vpH != curH) {
			emscripten_set_canvas_element_size("#canvas", vpW, vpH);
		}
	}

	UINT newWidth = get_canvas_width();
	UINT newHeight = get_canvas_height();

	if (newWidth != surfaceDescription.Width || newHeight != surfaceDescription.Height) {
		ErrorPrintf("Resizing canvas to %d x %d\n", newWidth, newHeight);
		surfaceDescription.Width = newWidth;
		surfaceDescription.Height = newHeight;
		glViewport(0, 0, surfaceDescription.Width, surfaceDescription.Height);

		// Set the projection transform (view and world are updated per frame)
		// This snippet has been taken from OnResetDevice in StuntCarRacer.cpp
		FLOAT fAspect = newWidth / (FLOAT)newHeight;
		FLOAT furthestZ = 131072.0f;
		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, fAspect, 0.5f, furthestZ);
		DXUTGetD3DDevice()->SetTransform(D3DTS_PROJECTION, &matProj);
	}
}

CONST D3DSURFACE_DESC * DXUTGetBackBufferSurfaceDesc() {
	Debug("DXUTGetBackBufferSurfaceDesc");
	return &surfaceDescription;
}
