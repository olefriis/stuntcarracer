#include "../dxstdafx.h"
#include "esUtil.h"
#include "SDL/SDL_opengl.h"
#include <GL/glut.h>

void PrintMatrix(const char *name, const D3DMATRIX *m) {
	printf("%s:\n", name);
	printf("%f %f %f %f\n", m->_11, m->_12, m->_13, m->_14);
	printf("%f %f %f %f\n", m->_21, m->_22, m->_23, m->_24);
	printf("%f %f %f %f\n", m->_31, m->_32, m->_33, m->_34);
	printf("%f %f %f %f\n", m->_41, m->_42, m->_43, m->_44);
	printf("\n");
}

HRESULT IDirect3DDevice9::SetTransform( D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix) {
	puts("IDirect3DDevice9::SetTransform");

	if (State == D3DTS_VIEW) {
		PrintMatrix("New view matrix", pMatrix);
		memcpy(&this->viewMatrix, pMatrix, sizeof(D3DMATRIX));
	} else if (State == D3DTS_PROJECTION) {
		memcpy(&this->projectionMatrix, pMatrix, sizeof(D3DMATRIX));
	} else if (State == D3DTS_WORLD) {
		memcpy(&this->worldMatrix, pMatrix, sizeof(D3DMATRIX));
	} else {
		printf("Unknown transform state type: %d", State);
	}

	return S_OK;
}

HRESULT IDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
	puts("IDirect3DDevice9::SetRenderState");
	return S_OK;
}

HRESULT IDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) {
	puts("IDirect3DDevice9::SetTextureStageState");
	return S_OK;
}

HRESULT IDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture) {
	puts("IDirect3DDevice9::SetTexture");
	return S_OK;
}

HRESULT IDirect3DDevice9::BeginScene() {
	puts("IDirect3DDevice9::BeginScene");
	return S_OK;
}

HRESULT IDirect3DDevice9::EndScene() {
	puts("IDirect3DDevice9::EndScene");
	return S_OK;
}

HRESULT IDirect3DDevice9::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	printf("IDirect3DDevice9::Clear(count=%ld, Color=%ld)\n", Count, Color);
	if (Flags == D3DCLEAR_ZBUFFER) {
		glClear ( GL_DEPTH_BUFFER_BIT );
		return S_OK;
	} else if (Flags == D3DCLEAR_TARGET) {
		glEnable(GL_SCISSOR_TEST);
		GLclampf red = ((Color >> 16) & 0xFF) / 255.0f;
		GLclampf green = ((Color >> 8) & 0xFF) / 255.0f;
		GLclampf blue = ((Color) & 0xFF) / 255.0f;
		glClearColor(red, green, blue, 0xFF);
		for (int i = 0; i < Count; i++) {
			D3DRECT rect = pRects[i];
			glScissor(rect.x1, rect.y1, rect.x2 - rect.x1, rect.y2 - rect.y1);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		glDisable(GL_SCISSOR_TEST);
		return S_OK;
	}

	printf("Unknown flag: %ld", Flags);
	return E_FAIL;
}

HRESULT IDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle) {
	puts("IDirect3DDevice9::CreateVertexBuffer");

	*ppVertexBuffer = new IDirect3DVertexBuffer9(Length);

	return S_OK;
}

HRESULT IDirect3DDevice9::SetFVF(DWORD FVF) {
	puts("IDirect3DDevice9::SetFVF");

	// We're being called with these FVFs:
	// - `D3DFVF_XYZRHW | D3DFVF_DIFFUSE` for `TRANSFORMEDVERTEX` (FLOAT x, y, z, rhw; DWORD color;)
	// - `D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1` for `UTVERTEX` (D3DXVECTOR3 pos; DWORD color;	FLOAT tu,tv;)

	if (FVF != (D3DFVF_XYZRHW | D3DFVF_DIFFUSE) && FVF != (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)) {
		printf("Unsupported FVF: %ld", FVF);
	}

	currentFvf = FVF;

	return S_OK;
}

HRESULT IDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride) {
	puts("IDirect3DDevice9::SetStreamSource");

	if (StreamNumber != 0) {
		printf("Stream number %d is not supported\n", StreamNumber);
	}
	if (OffsetInBytes != 0) {
		printf("OffsetInBytes %d is not supported\n", OffsetInBytes);
	}

	currentStreamSource = pStreamData;
	currentStride = Stride;

	return S_OK;
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

void DrawDummyTriangle() {
	   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f, 
                           -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f };

   // No clientside arrays, so do this in a webgl-friendly manner
   GLuint vertexPosObject;
   glGenBuffers(1, &vertexPosObject);
   glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
   glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);
   
   // Set the viewport
	const D3DSURFACE_DESC *surfaceDescription = DXUTGetBackBufferSurfaceDesc();
	glViewport(0, 0, surfaceDescription->Width, surfaceDescription->Height);
   
   // Load the vertex data
   glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
   glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
   glEnableVertexAttribArray(0);

   glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

typedef struct POINT3D {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

HRESULT IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
	puts("IDirect3DDevice9::DrawPrimitive");

	switch (PrimitiveType) {
		case D3DPT_LINELIST:
		break;
		case D3DPT_LINESTRIP:
		break;
		case D3DPT_TRIANGLELIST:
		{
			//PrintMatrix("View matrix", &this->viewMatrix);
			//PrintMatrix("World matrix", &this->worldMatrix);
			PrintMatrix("Projection matrix", &this->projectionMatrix);

			// This is used for drawing the track
			const char *vShaderStr =
				"uniform mat4 projectionMatrix;\n"
				"uniform mat4 viewMatrix;\n"
				"uniform mat4 worldMatrix;\n"
				"attribute vec3 vPosition;\n"
				"void main() {\n"
				"   vec4 homogenousPosition = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);\n"
				"   vec4 transformedPosition = homogenousPosition * worldMatrix * viewMatrix * projectionMatrix;\n"
				"   gl_Position = transformedPosition / transformedPosition.w;\n"
				"}\n";
			GLuint vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );

			const char *fShaderStr =  
				"precision mediump float;\n"
				"void main() {\n"
				"  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
				"}\n";
			GLuint fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

			GLuint programObject = glCreateProgram();
			if ( programObject == 0 ) {
				puts("programObject is 0");
				return E_FAIL;
			}

			glAttachShader ( programObject, vertexShader );
			glAttachShader ( programObject, fragmentShader );

			// Link the program
			glLinkProgram ( programObject );

			// Bind vPosition to attribute 0   
			glBindAttribLocation ( programObject, 0, "vPosition" );

			// Get the uniform locations
			GLint projectionMatrixLocation = glGetUniformLocation(programObject, "projectionMatrix");
			GLint viewMatrixLocation = glGetUniformLocation(programObject, "viewMatrix");
			GLint worldMatrixLocation = glGetUniformLocation(programObject, "worldMatrix");
			printf("projectionMatrixLocation: %d\n", projectionMatrixLocation);
			printf("viewMatrixLocation: %d\n", viewMatrixLocation);
			printf("worldMatrixLocation: %d\n", worldMatrixLocation);

			// Check the link status
			GLint linked;
			glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );
			if ( !linked ) 
			{
				puts("Shader not linked");
				return E_FAIL;
			}

			glUseProgram ( programObject );

			// Very temporary very hack: Shrink the track and move it a bit to the left so that we can see it
			for (int i=0; i<3*PrimitiveCount; i++) {
				POINT3D *p = (POINT3D *)((int) currentStreamSource->data + i*currentStride);
				int divideBy = 2500;
				p->x /= divideBy;
				p->y /= divideBy;
				p->z /= divideBy;
				p->x -= 15.0;
				p->z -= 30.0;
				if (i < 10) {
					printf("%d: (%f, %f, %f)\n", i, p->x, p->y, p->z);
					printf("%f\n", *((GLfloat*) ((int)currentStreamSource->data + i*currentStride)));
					printf("%f\n", *((GLfloat*) ((int)currentStreamSource->data + i*currentStride + 4)));
					printf("%f\n", *((GLfloat*) ((int)currentStreamSource->data + i*currentStride + 8)));
				}
			}

			D3DXMATRIX identity;
			D3DXMatrixIdentity(&identity);
			// Load the matrices
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, (GLfloat *)&projectionMatrix.glFloats[0]);
			//glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, (GLfloat *)&viewMatrix.glFloats[0]);
			//glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, (GLfloat *)&worldMatrix.glFloats[0]);
			//glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, (GLfloat *)&identity.glFloats[0]);
			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, (GLfloat *)&identity.glFloats[0]);
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, (GLfloat *)&identity.glFloats[0]);

			//DrawDummyTriangle();

			unsigned int VBO;
			glGenBuffers(1, &VBO); // Generate a single OpenGL buffer object
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			printf("length: %ld\n", currentStreamSource->length);
			printf("currentStride: %d", currentStride);
			glBufferData(GL_ARRAY_BUFFER, currentStreamSource->length, currentStreamSource->data, GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, VBO); // Seems superfluous? 
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, currentStride, 0);
			glEnableVertexAttribArray(0);

			printf("Drawing %d triangles\n", PrimitiveCount);
			glDrawArrays ( GL_TRIANGLES, 0, PrimitiveCount * 3 );
		}
		break;
		case D3DPT_TRIANGLESTRIP:
		break;
		case D3DPT_TRIANGLEFAN:
		break;
	}

	return S_OK;
}

static IDirect3DDevice9 *globalDirect3DDevice;

void SetUpD3DDevice() {
	globalDirect3DDevice = new IDirect3DDevice9();
}

IDirect3DDevice9* DXUTGetD3DDevice() {
  puts("DXUTGetD3DDevice");
  return globalDirect3DDevice;
}
