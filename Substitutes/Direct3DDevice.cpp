#include "../dxstdafx.h"
#include "esUtil.h"

HRESULT IDirect3DDevice9::SetTransform( D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix) {
	puts("IDirect3DDevice9::SetTransform");
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
	return S_OK;
}

HRESULT IDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride) {
	puts("IDirect3DDevice9::SetStreamSource");
	return S_OK;
}

HRESULT IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
	puts("IDirect3DDevice9::DrawPrimitive");
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
