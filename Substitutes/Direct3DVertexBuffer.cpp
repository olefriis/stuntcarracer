#include "../dxstdafx.h"

IDirect3DVertexBuffer9::IDirect3DVertexBuffer9(UINT Length) {
	data = malloc(Length);
	length = Length;

	glGenBuffers(1, &vbo); // Generate a single OpenGL buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);
}

HRESULT IDirect3DVertexBuffer9::Lock( UINT OffsetToLock, UINT SizeToLock, void  **ppbData, DWORD Flags ) {
	Debug("IDirect3DVertexBuffer9::Lock");

	if (OffsetToLock != 0) {
		Error("OffsetToLock != 0");
	}

	*ppbData = (void *) ((UINT) data + OffsetToLock);
	lockedSize = SizeToLock;

	return S_OK;
}

HRESULT IDirect3DVertexBuffer9::Unlock() {
	Debug("IDirect3DVertexBuffer9::Unlock");

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	UINT size = lockedSize == 0 ? length : lockedSize;
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	return S_OK;
}

// https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
void IDirect3DVertexBuffer9::Release() {
	Debug("IDirect3DVertexBuffer9::Release");
}
