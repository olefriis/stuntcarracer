#include "../dxstdafx.h"

IDirect3DVertexBuffer9::IDirect3DVertexBuffer9(UINT Length) {
	data = malloc(Length);
	length = Length;
}

HRESULT IDirect3DVertexBuffer9::Lock( UINT  OffsetToLock, UINT  SizeToLock, void  **ppbData, DWORD Flags ) {
	Debug("IDirect3DVertexBuffer9::Lock");

	*ppbData = (void *) ((UINT) data + OffsetToLock);

	return S_OK;
}

HRESULT IDirect3DVertexBuffer9::Unlock() {
	Debug("IDirect3DVertexBuffer9::Unlock");

	return S_OK;
}

// https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
void IDirect3DVertexBuffer9::Release() {
	Debug("IDirect3DVertexBuffer9::Release");
}
