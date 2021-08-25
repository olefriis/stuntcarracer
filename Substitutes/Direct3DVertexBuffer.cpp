#include "../dxstdafx.h"

/**
 * Basically a wrapper around an OpenGL vertex buffer object.
 * 
 * In DirectX, you are allowed to pass a color as a 32-bit integer which encodes red, green, blue, and maybe
 * alpha as separate bytes. But in WebGL, which is based on OpenGL ES, you can only pass floats, not ints,
 * and it doesn't seem like there are proper bitwise operators anyway to convert back to the colors. So...
 * we convert the colors from the integers into floats before actually passing them to the vertex buffer.
 * The code tries to do this effectively, only re-converting the values necessary on each iteration.
 * 
 * Only "XyzDiffuseTexture" and "XyzrhwDiffuse" objects are supported.
 * 
 * The code makes a lot of assumptions that will be pretty dangerous for general use. For example, we assume
 * that once the vertex buffer has been used for a certain type of object, it will always be used for that
 * type. This is the case in Stunt Car Racer, luckily.
 */

IDirect3DVertexBuffer9::IDirect3DVertexBuffer9(UINT Length) {
	originalData = malloc(Length);
	originalLength = Length;
	vbo = 0;
}

HRESULT IDirect3DVertexBuffer9::Lock( UINT OffsetToLock, UINT SizeToLock, void  **ppbData, DWORD Flags ) {
	Debug("IDirect3DVertexBuffer9::Lock");

	if (OffsetToLock != 0) {
		Error("OffsetToLock != 0");
	}

	*ppbData = (void *) ((UINT) originalData + OffsetToLock);
	dirtySize = SizeToLock == 0 ? originalLength : SizeToLock;

	return S_OK;
}

HRESULT IDirect3DVertexBuffer9::Unlock() {
	Debug("IDirect3DVertexBuffer9::Unlock");

	return S_OK;
}

void IDirect3DVertexBuffer9::PrepareForForXyzDiffuseTexture(UINT stride) {
	if (vbo == 0) {
		UINT convertedDataLength = sizeof(XyzDiffuseTextureConverted) * originalLength / stride;
		convertedData = malloc(convertedDataLength);
		glGenBuffers(1, &vbo); // Generate a single OpenGL buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, convertedDataLength, convertedData, GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (dirtySize > 0) {
		for (UINT i = 0; i < (dirtySize / stride); i++) {
			XyzDiffuseTextureOriginal *original = (XyzDiffuseTextureOriginal*)((char*)originalData + i * stride);
			XyzDiffuseTextureConverted *converted = (XyzDiffuseTextureConverted *)((char*)convertedData + i * sizeof(XyzDiffuseTextureConverted));
			converted->x = original->x;
			converted->y = original->y;
			converted->z = original->z;
			converted->red = (float) ((original->color >> 16) & 0xFF) / 255.0f;
			converted->green = (float) ((original->color >> 8) & 0xFF) / 255.0f;;
			converted->blue = (float) (original->color & 0xFF) / 255.0f;;
			converted->alpha = (float) 1.0f;
			converted->tu = original->tu;
			converted->tv = original->tv;
		}
		UINT dirtyConvertedDataLength = sizeof(XyzDiffuseTextureConverted) * dirtySize / stride;
		glBufferSubData(GL_ARRAY_BUFFER, 0, dirtyConvertedDataLength, convertedData);
		dirtySize = 0;
	}
}

void IDirect3DVertexBuffer9::PrepareForXyzrhwDiffuse(UINT stride) {
	if (vbo == 0) {
		UINT convertedDataLength = sizeof(XyzrhwDiffuseConverted) * originalLength / stride;
		convertedData = malloc(convertedDataLength);
		glGenBuffers(1, &vbo); // Generate a single OpenGL buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, convertedDataLength, convertedData, GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (dirtySize > 0) {
		for (UINT i = 0; i < (dirtySize / stride); i++) {
			XyzrhwDiffuseOriginal *original = (XyzrhwDiffuseOriginal*)((char*)originalData + i * stride);
			XyzrhwDiffuseConverted *converted = (XyzrhwDiffuseConverted *)((char*)convertedData + i * sizeof(XyzrhwDiffuseConverted));
			converted->x = original->x;
			converted->y = original->y;
			converted->z = original->z;
			converted->w = original->w;
			converted->red = (float) ((original->color >> 16) & 0xFF) / 255.0f;
			converted->green = (float) ((original->color >> 8) & 0xFF) / 255.0f;;
			converted->blue = (float) (original->color & 0xFF) / 255.0f;;
			converted->alpha = (float) 1.0f;
		}
		UINT dirtyConvertedDataLength = sizeof(XyzrhwDiffuseConverted) * dirtySize / stride;
		glBufferSubData(GL_ARRAY_BUFFER, 0, dirtyConvertedDataLength, convertedData);
		dirtySize = 0;
	}
}

// https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
void IDirect3DVertexBuffer9::Release() {
	Debug("IDirect3DVertexBuffer9::Release");
}
