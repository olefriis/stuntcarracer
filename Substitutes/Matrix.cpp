#include "../dxstdafx.h"

double cotan(double i) { return(1 / tan(i)); }
double dot(const D3DXVECTOR3 *v1, const D3DXVECTOR3 *v2) { return(v1->x * v2->x + v1->y * v2->y + v1->z * v2->z); }
void normalize(D3DXVECTOR3 *v) {
	double length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= length;
	v->y /= length;
	v->z /= length;
}
void cross(D3DXVECTOR3 *result, const D3DXVECTOR3 *v1, const D3DXVECTOR3 *v2) {
	result->x = v1->y * v2->z - v1->z * v2->y;
	result->y = v1->z * v2->x - v1->x * v2->z;
	result->z = v1->x * v2->y - v1->y * v2->x;
}

D3DXMATRIX* D3DXMatrixPerspectiveFovLH(
  D3DXMATRIX *pOut,
  FLOAT      fovy,
  FLOAT      Aspect,
  FLOAT      zn,
  FLOAT      zf
) {
	Debug("D3DXMatrixPerspectiveFovLH");
	DebugPrintf("fovy: %f, Aspect: %f, zn: %f, zf: %f\n", fovy, Aspect, zn, zf);

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
	double yScale = cotan(fovy/2);
	double xScale = yScale / Aspect;

	// Row 1
	pOut->_11 = xScale;
	pOut->_12 = 0;
	pOut->_13 = 0;
	pOut->_14 = 0;

	// Row 2
	pOut->_21 = 0;
	pOut->_22 = yScale;
	pOut->_23 = 0;
	pOut->_24 = 0;

	// Row 3
	pOut->_31 = 0;
	pOut->_32 = 0;
	pOut->_33 = zf / (zf - zn);
	pOut->_34 = 1;

	// Row 4
	pOut->_41 = 0;
	pOut->_42 = 0;
	pOut->_43 = -zn*zf / (zf - zn);
	pOut->_44 = 0;

	return pOut;
}

D3DXMATRIX* D3DXMatrixIdentity(
  D3DXMATRIX *pOut
) {
	Debug("D3DXMatrixIdentity");

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixidentity

	// Row 1
	pOut->_11 = 1;
	pOut->_12 = 0;
	pOut->_13 = 0;
	pOut->_14 = 0;

	// Row 2
	pOut->_21 = 0;
	pOut->_22 = 1;
	pOut->_23 = 0;
	pOut->_24 = 0;

	// Row 3
	pOut->_31 = 0;
	pOut->_32 = 0;
	pOut->_33 = 1;
	pOut->_34 = 0;

	// Row 4
	pOut->_41 = 0;
	pOut->_42 = 0;
	pOut->_43 = 0;
	pOut->_44 = 1;

	return pOut;
}

D3DXMATRIX* D3DXMatrixRotationX(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
	Debug("D3DXMatrixRotationX");

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixrotationx

	double c = cos(-Angle);
	double s = sin(-Angle);

	// Row 1
	pOut->_11 = 1;
	pOut->_12 = 0;
	pOut->_13 = 0;
	pOut->_14 = 0;

	// Row 2
	pOut->_21 = 0;
	pOut->_22 = c;
	pOut->_23 = s;
	pOut->_24 = 0;

	// Row 3
	pOut->_31 = 0;
	pOut->_32 = -s;
	pOut->_33 = c;
	pOut->_34 = 0;

	// Row 4
	pOut->_41 = 0;
	pOut->_42 = 0;
	pOut->_43 = 0;
	pOut->_44 = 1;

	return pOut;
}

D3DXMATRIX* D3DXMatrixRotationY(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
	Debug("D3DXMatrixRotationY");

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixrotationy

	double c = -cos(-Angle);
	double s = sin(-Angle);

	// Row 1
	pOut->_11 = c;
	pOut->_12 = 0;
	pOut->_13 = -s;
	pOut->_14 = 0;

	// Row 2
	pOut->_21 = 0;
	pOut->_22 = 1;
	pOut->_23 = 0;
	pOut->_24 = 0;

	// Row 3
	pOut->_31 = s;
	pOut->_32 = 0;
	pOut->_33 = c;
	pOut->_34 = 0;

	// Row 4
	pOut->_41 = 0;
	pOut->_42 = 0;
	pOut->_43 = 0;
	pOut->_44 = 1;

	return pOut;
}

D3DXMATRIX* D3DXMatrixRotationZ(
  D3DXMATRIX *pOut,
  FLOAT      Angle
) {
	Debug("D3DXMatrixRotationZ");

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixrotationz

	double c = cos(-Angle);
	double s = sin(-Angle);

	// Row 1
	pOut->_11 = c;
	pOut->_12 = s;
	pOut->_13 = 0;
	pOut->_14 = 0;

	// Row 2
	pOut->_21 = -s;
	pOut->_22 = c;
	pOut->_23 = 0;
	pOut->_24 = 0;

	// Row 3
	pOut->_31 = 0;
	pOut->_32 = 0;
	pOut->_33 = 1;
	pOut->_34 = 0;

	// Row 4
	pOut->_41 = 0;
	pOut->_42 = 0;
	pOut->_43 = 0;
	pOut->_44 = 1;

	return pOut;
}

D3DXMATRIX* D3DXMatrixTranslation(
  D3DXMATRIX *pOut,
  FLOAT      x,
  FLOAT      y,
  FLOAT      z
) {
	Debug("D3DXMatrixTranslation");

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixtranslation

	// Row 1
	pOut->_11 = 1;
	pOut->_12 = 0;
	pOut->_13 = 0;
	pOut->_14 = 0;

	// Row 2
	pOut->_21 = 0;
	pOut->_22 = 1;
	pOut->_23 = 0;
	pOut->_24 = 0;

	// Row 3
	pOut->_31 = 0;
	pOut->_32 = 0;
	pOut->_33 = 1;
	pOut->_34 = 0;

	// Row 4
	pOut->_41 = x;
	pOut->_42 = y;
	pOut->_43 = z;
	pOut->_44 = 1;

	return pOut;
}

void PrintVector(const char *name, const D3DXVECTOR3 *v) {
	DebugPrintf("%s: {%f, %f, %f}\n", name, v->x, v->y, v->z);
}

D3DXMATRIX* D3DXMatrixLookAtLH(
  D3DXMATRIX  *pOut,
  const D3DXVECTOR3 *pEye,
  const D3DXVECTOR3 *pAt,
  const D3DXVECTOR3 *pUp
) {
	Debug("D3DXMatrixLookAtLH");
	PrintVector("pEye", pEye);
	PrintVector("pAt", pAt);
	PrintVector("pUp", pUp);

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh

	D3DXVECTOR3 zaxis, xaxis, yaxis;

	zaxis.x = pEye->x - pAt->x;
	zaxis.y = pEye->y - pAt->y;
	zaxis.z = pEye->z - pAt->z;
	normalize(&zaxis);

	cross(&xaxis, pUp, &zaxis);
	normalize(&xaxis);

	cross(&yaxis, &zaxis, &xaxis);

	// Row 1
	pOut->_11 = xaxis.x;
	pOut->_12 = yaxis.x;
	pOut->_13 = zaxis.x;
	pOut->_14 = 0;

	// Row 2
	pOut->_21 = xaxis.y;
	pOut->_22 = yaxis.y;
	pOut->_23 = zaxis.y;
	pOut->_24 = 0;

	// Row 3
	pOut->_31 = xaxis.z;
	pOut->_32 = yaxis.z;
	pOut->_33 = zaxis.z;
	pOut->_34 = 0;

	// Row 4
	pOut->_41 = -dot(&xaxis, pEye);
	pOut->_42 = -dot(&yaxis, pEye);
	pOut->_43 = -dot(&zaxis, pEye);
	pOut->_44 = 1;

	return pOut;
}

D3DXMATRIX* D3DXMatrixMultiply(
  D3DXMATRIX *pOut,
  const D3DXMATRIX *pM1,
  const D3DXMATRIX *pM2
) {
	Debug("D3DXMatrixMultiply");

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixmultiply

	D3DXMATRIX tmp;
	tmp._11 = pM1->_11 * pM2->_11 + pM1->_12 * pM2->_21 + pM1->_13 * pM2->_31 + pM1->_14 * pM2->_41;
	tmp._12 = pM1->_11 * pM2->_12 + pM1->_12 * pM2->_22 + pM1->_13 * pM2->_32 + pM1->_14 * pM2->_42;
	tmp._13 = pM1->_11 * pM2->_13 + pM1->_12 * pM2->_23 + pM1->_13 * pM2->_33 + pM1->_14 * pM2->_43;
	tmp._14 = pM1->_11 * pM2->_14 + pM1->_12 * pM2->_24 + pM1->_13 * pM2->_34 + pM1->_14 * pM2->_44;

	tmp._21 = pM1->_21 * pM2->_11 + pM1->_22 * pM2->_21 + pM1->_23 * pM2->_31 + pM1->_24 * pM2->_41;
	tmp._22 = pM1->_21 * pM2->_12 + pM1->_22 * pM2->_22 + pM1->_23 * pM2->_32 + pM1->_24 * pM2->_42;
	tmp._23 = pM1->_21 * pM2->_13 + pM1->_22 * pM2->_23 + pM1->_23 * pM2->_33 + pM1->_24 * pM2->_43;
	tmp._24 = pM1->_21 * pM2->_14 + pM1->_22 * pM2->_24 + pM1->_23 * pM2->_34 + pM1->_24 * pM2->_44;

	tmp._31 = pM1->_31 * pM2->_11 + pM1->_32 * pM2->_21 + pM1->_33 * pM2->_31 + pM1->_34 * pM2->_41;
	tmp._32 = pM1->_31 * pM2->_12 + pM1->_32 * pM2->_22 + pM1->_33 * pM2->_32 + pM1->_34 * pM2->_42;
	tmp._33 = pM1->_31 * pM2->_13 + pM1->_32 * pM2->_23 + pM1->_33 * pM2->_33 + pM1->_34 * pM2->_43;
	tmp._34 = pM1->_31 * pM2->_14 + pM1->_32 * pM2->_24 + pM1->_33 * pM2->_34 + pM1->_34 * pM2->_44;

	tmp._41 = pM1->_41 * pM2->_11 + pM1->_42 * pM2->_21 + pM1->_43 * pM2->_31 + pM1->_44 * pM2->_41;
	tmp._42 = pM1->_41 * pM2->_12 + pM1->_42 * pM2->_22 + pM1->_43 * pM2->_32 + pM1->_44 * pM2->_42;
	tmp._43 = pM1->_41 * pM2->_13 + pM1->_42 * pM2->_23 + pM1->_43 * pM2->_33 + pM1->_44 * pM2->_43;
	tmp._44 = pM1->_41 * pM2->_14 + pM1->_42 * pM2->_24 + pM1->_43 * pM2->_34 + pM1->_44 * pM2->_44;

	memcpy(pOut, &tmp, sizeof(D3DXMATRIX));

	return pOut;
}
