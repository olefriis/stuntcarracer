#include "../dxstdafx.h"
#include "SDL/SDL_surface.h"
#include "SDL_image.h"

HRESULT D3DXCreateTextureFromResource(
	LPDIRECT3DDEVICE9  pDevice,
	HMODULE            hSrcModule,
	LPCTSTR            pSrcResource,
	LPDIRECT3DTEXTURE9 *ppTexture
) {
	Debug("D3DXCreateTextureFromResource");

	int resourceNameLength = wcslen(pSrcResource);
    int pathLength = 7 + resourceNameLength + 4 + 1;
    char *fileName = (char*) malloc(pathLength);
	sprintf(fileName, "Bitmap/%ls.bmp", pSrcResource);

	SDL_Surface *image = IMG_Load(fileName);
	if (!image) {
    	ErrorPrintf("IMG_Load: %s\n", IMG_GetError());
		return E_FAIL;
	}
	if (image->format->BitsPerPixel != 24) {
		ErrorPrintf("Unsupported number of bits per pixel: %i\n", image->format->BitsPerPixel);
		return E_FAIL;
	}
	if (image->format->BytesPerPixel != 3) {
		ErrorPrintf("Unsupported number of bytes per pixel: %i\n", image->format->BytesPerPixel);
	}

	// Convert BGR to RGB. Theoretically we should use the method described here: https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlpixelformat.html
	// ...but this will work for our textures:
	for (int i = 0; i < image->w * image->h; i++) {
		Uint8 *pixel = (Uint8*) image->pixels + i * image->format->BytesPerPixel;
		Uint8 temp = pixel[0];
		pixel[0] = pixel[2];
		pixel[2] = temp;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// None of our textures have dimensions that are powers of 2, so don't use mipmapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(image);

	*ppTexture = new IDirect3DTexture9(texture);

	return S_OK;
}
