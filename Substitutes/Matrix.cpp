#include "../dxstdafx.h"

D3DXMATRIX* D3DXMatrixPerspectiveFovLH(
  D3DXMATRIX *pOut,
  FLOAT      fovy,
  FLOAT      Aspect,
  FLOAT      zn,
  FLOAT      zf
) {
  puts("D3DXMatrixPerspectiveFovLH");
  return NULL;
}

D3DXMATRIX* D3DXMatrixIdentity(
  D3DXMATRIX *pOut
) {
  puts("D3DXMatrixIdentity");
  return NULL;
}

D3DXMATRIX* D3DXMatrixRotationX(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
  puts("D3DXMatrixRotationX");
  return NULL;
}

D3DXMATRIX* D3DXMatrixRotationY(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
  puts("D3DXMatrixRotationY");
  return NULL;
}

D3DXMATRIX* D3DXMatrixRotationZ(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
  puts("D3DXMatrixRotationZ");
  return NULL;
}

D3DXMATRIX* D3DXMatrixTranslation(
  D3DXMATRIX *pOut,
  FLOAT      x,
  FLOAT      y,
  FLOAT      z
) {
  puts("D3DXMatrixTranslation");
  return NULL;
}

D3DXMATRIX* D3DXMatrixLookAtLH(
  D3DXMATRIX  *pOut,
  const D3DXVECTOR3 *pEye,
  const D3DXVECTOR3 *pAt,
  const D3DXVECTOR3 *pUp
) {
  puts("D3DXMatrixLookAtLH");
  return NULL;
}

D3DXMATRIX* D3DXMatrixMultiply(
  D3DXMATRIX *pOut,
  const D3DXMATRIX *pM1,
  const D3DXMATRIX *pM2
) {
  puts("D3DXMatrixMultiply");
  return NULL;
}
