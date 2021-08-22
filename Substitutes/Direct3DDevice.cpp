#include "../dxstdafx.h"
#include "esUtil.h"
#include "SDL/SDL_opengl.h"
#include <GL/glut.h>

#define XYZRHW_DIFFUSE (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
#define XYZ_DIFFUSE_TEXTURE (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

void PrintMatrix(const char *name, const D3DMATRIX *m) {
	DebugPrintf("%s:\n", name);
	DebugPrintf("%f %f %f %f\n", m->_11, m->_12, m->_13, m->_14);
	DebugPrintf("%f %f %f %f\n", m->_21, m->_22, m->_23, m->_24);
	DebugPrintf("%f %f %f %f\n", m->_31, m->_32, m->_33, m->_34);
	DebugPrintf("%f %f %f %f\n", m->_41, m->_42, m->_43, m->_44);
	DebugPrintf("\n");
}

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;
   
   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
   	return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );
   
   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled ) 
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog = (char *)malloc (sizeof(char) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         esLogMessage ( "Error compiling shader:\n%s\n", infoLog );            
         
         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;
}

static GLuint programObjectForXyzDiffuseTexture;
static GLint projectionMatrixLocationForXyzDiffuseTexture;
static GLint viewMatrixLocationForXyzDiffuseTexture;
static GLint worldMatrixLocationForXyzDiffuseTexture;

static GLuint programObjectForXyzrhwDiffuse;
static GLint windowWidthLocationForXyzrhwDiffuse;
static GLint windowHeightLocationForXyzrhwDiffuse;

void setUpShadersForXyzDiffuseTexture() {
	const char *vertexShaderStringForXyzDiffuseTexture =
		"uniform mat4 projectionMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"uniform mat4 worldMatrix;\n"
		"attribute vec3 vPosition;\n"
		"void main() {\n"
		"   vec4 homogenousPosition = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);\n"
		"   vec4 transformedPosition = homogenousPosition * worldMatrix * viewMatrix * projectionMatrix;\n"
		"   gl_Position = transformedPosition / transformedPosition.w;\n"
		"   gl_Position.y *= -1.0;\n" // Accomodate for DirectX top-left origin
		"}\n";
	const char *fragmentShaderStringForXyzDiffuseTexture =  
		"precision mediump float;\n"
		"void main() {\n"
		"  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
		"}\n";
	GLuint vertexShaderForXyzDiffuseTexture = LoadShader ( GL_VERTEX_SHADER, vertexShaderStringForXyzDiffuseTexture );
	GLuint fragmentShaderForXyzDiffuseTexture = LoadShader ( GL_FRAGMENT_SHADER, fragmentShaderStringForXyzDiffuseTexture );

	programObjectForXyzDiffuseTexture = glCreateProgram();
	if ( programObjectForXyzDiffuseTexture == 0 ) {
		Debug("ERROR: programObjectForXyzDiffuseTexture is 0");
		return;
	}

	glAttachShader ( programObjectForXyzDiffuseTexture, fragmentShaderForXyzDiffuseTexture );
	glAttachShader ( programObjectForXyzDiffuseTexture, vertexShaderForXyzDiffuseTexture );

	// Link the program
	glLinkProgram ( programObjectForXyzDiffuseTexture );

	// Bind vPosition to attribute 0   
	glBindAttribLocation ( programObjectForXyzDiffuseTexture, 0, "vPosition" );

	// Get the uniform locations
	projectionMatrixLocationForXyzDiffuseTexture = glGetUniformLocation(programObjectForXyzDiffuseTexture, "projectionMatrix");
	viewMatrixLocationForXyzDiffuseTexture = glGetUniformLocation(programObjectForXyzDiffuseTexture, "viewMatrix");
	worldMatrixLocationForXyzDiffuseTexture = glGetUniformLocation(programObjectForXyzDiffuseTexture, "worldMatrix");
	DebugPrintf("projectionMatrixLocation: %d\n", projectionMatrixLocationForXyzDiffuseTexture);
	DebugPrintf("viewMatrixLocation: %d\n", viewMatrixLocationForXyzDiffuseTexture);
	DebugPrintf("worldMatrixLocation: %d\n", worldMatrixLocationForXyzDiffuseTexture);

	// Check the link status
	GLint linked;
	glGetProgramiv ( programObjectForXyzDiffuseTexture, GL_LINK_STATUS, &linked );
	if ( !linked ) 
	{
		Debug("ERROR: Shader for XYZ_DIFFUSE_TEXTURE not linked");
		return;
	}
}

void setUpShadersForXyzrhwDiffuse() {
	const char *vertexShaderStringForXyzrhwDiffuse =
		"attribute vec4 vPosition;\n"
		"uniform float windowWidth;\n"
		"uniform float windowHeight;\n"
		"varying lowp vec4 outputColor;\n"
		"void main() {\n"
		"   gl_Position = vec4 (2.0 * vPosition.x / windowWidth - 1.0, -(2.0 * vPosition.y / windowHeight - 1.0), 0.5, 1.0);\n"
		"   outputColor = vec4(0.5, 1.0, 1.0, 1.0);\n"
		"}\n";
	const char *fragmentShaderStringForXyzrhwDiffuse =  
		"precision mediump float;\n"
		"varying lowp vec4 outputColor;\n"
		"void main() {\n"
		"  gl_FragColor = outputColor;\n"
		"}\n";
	GLuint vertexShaderForXyzrhwDiffuse = LoadShader ( GL_VERTEX_SHADER, vertexShaderStringForXyzrhwDiffuse );
	GLuint fragmentShaderForXyzrhwDiffuse = LoadShader ( GL_FRAGMENT_SHADER, fragmentShaderStringForXyzrhwDiffuse );

	programObjectForXyzrhwDiffuse = glCreateProgram();
	if ( programObjectForXyzrhwDiffuse == 0 ) {
		Debug("ERROR: programObjectForXyzrhwDiffuse is 0");
		return;
	}

	glAttachShader ( programObjectForXyzrhwDiffuse, fragmentShaderForXyzrhwDiffuse );
	glAttachShader ( programObjectForXyzrhwDiffuse, vertexShaderForXyzrhwDiffuse );

	// Link the program
	glLinkProgram ( programObjectForXyzrhwDiffuse );

	// Bind vPosition to attribute 0 and vColor to attribute 1   
	glBindAttribLocation ( programObjectForXyzrhwDiffuse, 0, "vPosition" );
	//glBindAttribLocation ( programObjectForXyzrhwDiffuse, 1, "vColor" );

	// Get the uniform locations
	windowWidthLocationForXyzrhwDiffuse = glGetUniformLocation(programObjectForXyzrhwDiffuse, "windowWidth");
	windowHeightLocationForXyzrhwDiffuse = glGetUniformLocation(programObjectForXyzrhwDiffuse, "windowHeight");
	DebugPrintf("windowWidthLocationForXyzrhwDiffuse: %d\n", windowWidthLocationForXyzrhwDiffuse);
	DebugPrintf("windowHeightLocationForXyzrhwDiffuse: %d\n", windowHeightLocationForXyzrhwDiffuse);

	// Check the link status
	GLint linked;
	glGetProgramiv ( programObjectForXyzrhwDiffuse, GL_LINK_STATUS, &linked );
	if ( !linked ) 
	{
		Debug("ERROR: Shader for XYZRHW_DIFFUSE not linked");
		return;
	}
}

IDirect3DDevice9::IDirect3DDevice9() {
	D3DXMatrixIdentity(&viewMatrix);
	D3DXMatrixIdentity(&worldMatrix);
	D3DXMatrixIdentity(&projectionMatrix);

	setUpShadersForXyzDiffuseTexture();
	setUpShadersForXyzrhwDiffuse();
}

HRESULT IDirect3DDevice9::SetTransform( D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix) {
	DebugPrintf("IDirect3DDevice9::SetTransform(%d, matrix)\n", State);

	if (State == D3DTS_VIEW) {
		memcpy(&this->viewMatrix, pMatrix, sizeof(D3DMATRIX));
	} else if (State == D3DTS_PROJECTION) {
		memcpy(&this->projectionMatrix, pMatrix, sizeof(D3DMATRIX));
	} else if (State == D3DTS_WORLD) {
		memcpy(&this->worldMatrix, pMatrix, sizeof(D3DMATRIX));
	} else {
		ErrorPrintf("Unknown transform state type: %d", State);
	}

	return S_OK;
}

HRESULT IDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
	Debug("IDirect3DDevice9::SetRenderState");

	if (State == D3DRS_CULLMODE) {
		if (Value == D3DCULL_NONE) {
			glDisable(GL_CULL_FACE);
		} else if (Value == D3DCULL_CW) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		} else if (Value == D3DCULL_CCW) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		} else {
			ErrorPrintf("Unknown cull mode: %lu", Value);
		}
	} else if (State == D3DRS_ZENABLE) {
		if (Value == TRUE) {
			glEnable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_DEPTH_TEST);
		}
	} else {
		ErrorPrintf("Unknown render state type: %d\n", State);
	}

	return S_OK;
}

HRESULT IDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) {
	Debug("IDirect3DDevice9::SetTextureStageState");
	return S_OK;
}

HRESULT IDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture) {
	Debug("IDirect3DDevice9::SetTexture");
	return S_OK;
}

HRESULT IDirect3DDevice9::BeginScene() {
	Debug("IDirect3DDevice9::BeginScene");
	return S_OK;
}

HRESULT IDirect3DDevice9::EndScene() {
	Debug("IDirect3DDevice9::EndScene");
	return S_OK;
}

HRESULT IDirect3DDevice9::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	DebugPrintf("IDirect3DDevice9::Clear(count=%ld, Color=%ld)\n", Count, Color);
	if (Flags == D3DCLEAR_ZBUFFER) {
		glClear ( GL_DEPTH_BUFFER_BIT );
		return S_OK;
	} else if (Flags == D3DCLEAR_TARGET) {
		UINT windowHeight = DXUTGetBackBufferSurfaceDesc()->Height;

		glEnable(GL_SCISSOR_TEST);
		GLclampf red = ((Color >> 16) & 0xFF) / 255.0f;
		GLclampf green = ((Color >> 8) & 0xFF) / 255.0f;
		GLclampf blue = ((Color) & 0xFF) / 255.0f;
		glClearColor(red, green, blue, 0xFF);
		for (int i = 0; i < Count; i++) {
			D3DRECT rect = pRects[i];
			UINT y1 = windowHeight - rect.y2; // Accomodate for DirectX top-left origin
			glScissor(rect.x1, y1, rect.x2 - rect.x1, rect.y2 - rect.y1);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		glDisable(GL_SCISSOR_TEST);
		return S_OK;
	}

	ErrorPrintf("Unknown flag: %ld", Flags);
	return E_FAIL;
}

HRESULT IDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle) {
	Debug("IDirect3DDevice9::CreateVertexBuffer");

	*ppVertexBuffer = new IDirect3DVertexBuffer9(Length);

	return S_OK;
}

HRESULT IDirect3DDevice9::SetFVF(DWORD FVF) {
	Debug("IDirect3DDevice9::SetFVF");

	// We're being called with these FVFs:
	// - `D3DFVF_XYZRHW | D3DFVF_DIFFUSE` for `TRANSFORMEDVERTEX` (FLOAT x, y, z, rhw; DWORD color;)
	// - `D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1` for `UTVERTEX` (D3DXVECTOR3 pos; DWORD color;	FLOAT tu,tv;)

	if (FVF != XYZRHW_DIFFUSE && FVF != XYZ_DIFFUSE_TEXTURE) {
		ErrorPrintf("Unsupported FVF: %ld", FVF);
	}

	currentFvf = FVF;

	return S_OK;
}

HRESULT IDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride) {
	Debug("IDirect3DDevice9::SetStreamSource");

	if (StreamNumber != 0) {
		ErrorPrintf("Stream number %d is not supported\n", StreamNumber);
	}
	if (OffsetInBytes != 0) {
		ErrorPrintf("OffsetInBytes %d is not supported\n", OffsetInBytes);
	}

	currentStreamSource = pStreamData;
	currentStride = Stride;

	return S_OK;
}

HRESULT IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
	DebugPrintf("IDirect3DDevice9::DrawPrimitive(%d, %d, %d)\n", PrimitiveType, StartVertex, PrimitiveCount);

	switch (PrimitiveType) {
		case D3DPT_LINELIST:
		Error("Not implemented yet: D3DPT_LINELIST");
		break;
		case D3DPT_LINESTRIP:
		Error("Not implemented yet: D3DPT_LINESTRIP");
		break;
		case D3DPT_TRIANGLELIST:
		{
			//PrintMatrix("World matrix for TRIANGLELIST", &this->worldMatrix);
			//PrintMatrix("View matrix for TRIANGLELIST", &this->viewMatrix);
			//PrintMatrix("Projection matrix for TRIANGLELIST", &this->projectionMatrix);

			// This is used for drawing the track

			if (currentFvf == XYZ_DIFFUSE_TEXTURE) {
				DrawTriangleListForXyzDiffuseTexture(StartVertex, PrimitiveCount);
			} else {
				ErrorPrintf("ERROR: Unsupported FVF for D3DPT_TRIANGLELIST: %ld\n", currentFvf);
			}
		}
		break;
		case D3DPT_TRIANGLESTRIP:
		Error("Not implemented yet: D3DPT_TRIANGLESTRIP");
		break;
		case D3DPT_TRIANGLEFAN:
		{
			//PrintMatrix("World matrix for TRIANGLEFAN", &this->worldMatrix);
			//PrintMatrix("View matrix for TRIANGLEFAN", &this->viewMatrix);
			//PrintMatrix("Projection matrix for TRIANGLEFAN", &this->projectionMatrix);

			if (currentFvf == XYZRHW_DIFFUSE) {
				DrawTriangleListForXyzrhwDiffuse(StartVertex, PrimitiveCount);
			} else {
				ErrorPrintf("ERROR: Unsupported FVF for D3DPT_TRIANGLEFAN: %ld\n", currentFvf);
			}
		}
		break;
	}

	return S_OK;
}

void IDirect3DDevice9::DrawTriangleListForXyzDiffuseTexture(UINT StartVertex, UINT PrimitiveCount) {
	glUseProgram(programObjectForXyzDiffuseTexture);

	glUniformMatrix4fv(projectionMatrixLocationForXyzDiffuseTexture, 1, GL_FALSE, (GLfloat *)&projectionMatrix.glFloats[0]);
	glUniformMatrix4fv(viewMatrixLocationForXyzDiffuseTexture, 1, GL_FALSE, (GLfloat *)&viewMatrix.glFloats[0]);
	glUniformMatrix4fv(worldMatrixLocationForXyzDiffuseTexture, 1, GL_FALSE, (GLfloat *)&worldMatrix.glFloats[0]);

	unsigned int VBO;
	glGenBuffers(1, &VBO); // Generate a single OpenGL buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, currentStreamSource->length, currentStreamSource->data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Seems superfluous? 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, currentStride, 0);
	glEnableVertexAttribArray(0);

	DebugPrintf("Drawing %d triangles\n", PrimitiveCount);
	glDrawArrays ( GL_TRIANGLES, 0, PrimitiveCount * 3 );
}

// I've no idea at all why we should not take the transform matrices into account in this
// shader. Some DirectX call must be turning off the world-view-projection transform...?
void IDirect3DDevice9::DrawTriangleListForXyzrhwDiffuse(UINT StartVertex, UINT PrimitiveCount) {
	glUseProgram(programObjectForXyzrhwDiffuse);

	const D3DSURFACE_DESC *surfaceDescription = DXUTGetBackBufferSurfaceDesc();

	glUniform1f(windowWidthLocationForXyzrhwDiffuse, (float) surfaceDescription->Width);
	glUniform1f(windowHeightLocationForXyzrhwDiffuse, (float) surfaceDescription->Height);

	unsigned int VBO;
	glGenBuffers(1, &VBO); // Generate a single OpenGL buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, currentStreamSource->length, currentStreamSource->data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Seems superfluous? 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, currentStride, 0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, currentStride, 0);
	glEnableVertexAttribArray(0);

	DebugPrintf("Drawing %d triangle fans\n", PrimitiveCount);
	glDrawArrays ( GL_TRIANGLE_FAN, 0, 2 + PrimitiveCount );
}

static IDirect3DDevice9 *globalDirect3DDevice;

void SetUpD3DDevice() {
	globalDirect3DDevice = new IDirect3DDevice9();
}

IDirect3DDevice9* DXUTGetD3DDevice() {
  Debug("DXUTGetD3DDevice");
  return globalDirect3DDevice;
}
