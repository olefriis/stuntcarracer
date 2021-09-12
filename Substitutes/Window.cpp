#include "../dxstdafx.h"

#include "SDL.h"
#include "SDL_opengl.h"
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
EM_JS(UINT, initial_canvas_width, (), { return window.production ? window.innerWidth : 320; })
EM_JS(UINT, initial_canvas_height, (), { return window.production ? window.innerHeight : 240; })

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
	UINT newWidth = get_canvas_width();
	UINT newHeight = get_canvas_height();

	if (newWidth != surfaceDescription.Width || newHeight != surfaceDescription.Height) {
		ErrorPrintf("Resizing canvas to %d x %d\n", newWidth, newHeight);
		surfaceDescription.Width = newWidth;
		surfaceDescription.Height = newHeight;
		glViewport(0, 0, surfaceDescription.Width, surfaceDescription.Height);
	}
}

CONST D3DSURFACE_DESC * DXUTGetBackBufferSurfaceDesc() {
	Debug("DXUTGetBackBufferSurfaceDesc");
	return &surfaceDescription;
}
