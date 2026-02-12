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

// Quaternion from rotation matrix
// Based on: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
// Note: The matrix layout is column-first for OpenGL, so we access elements accordingly.
// In the column-first layout: _11,_21,_31,_41 is column 0, _12,_22,_32,_42 is column 1, etc.
// For the rotation extraction, we need the 3x3 rotation part in row-major terms:
//   row0: m[0][0], m[1][0], m[2][0]  which is _11, _12, _13
//   row1: m[0][1], m[1][1], m[2][1]  which is _21, _22, _23
//   row2: m[0][2], m[1][2], m[2][2]  which is _31, _32, _33
D3DXQUATERNION* D3DXQuaternionRotationMatrix(
  D3DXQUATERNION *pOut,
  const D3DXMATRIX *pM
) {
	// Use the standard algorithm with the column-first stored matrix
	float m00 = pM->_11, m01 = pM->_12, m02 = pM->_13;
	float m10 = pM->_21, m11 = pM->_22, m12 = pM->_23;
	float m20 = pM->_31, m21 = pM->_32, m22 = pM->_33;

	float trace = m00 + m11 + m22;

	if (trace > 0) {
		float s = 0.5f / sqrtf(trace + 1.0f);
		pOut->w = 0.25f / s;
		pOut->x = (m21 - m12) * s;
		pOut->y = (m02 - m20) * s;
		pOut->z = (m10 - m01) * s;
	} else if (m00 > m11 && m00 > m22) {
		float s = 2.0f * sqrtf(1.0f + m00 - m11 - m22);
		pOut->w = (m21 - m12) / s;
		pOut->x = 0.25f * s;
		pOut->y = (m01 + m10) / s;
		pOut->z = (m02 + m20) / s;
	} else if (m11 > m22) {
		float s = 2.0f * sqrtf(1.0f + m11 - m00 - m22);
		pOut->w = (m02 - m20) / s;
		pOut->x = (m01 + m10) / s;
		pOut->y = 0.25f * s;
		pOut->z = (m12 + m21) / s;
	} else {
		float s = 2.0f * sqrtf(1.0f + m22 - m00 - m11);
		pOut->w = (m10 - m01) / s;
		pOut->x = (m02 + m20) / s;
		pOut->y = (m12 + m21) / s;
		pOut->z = 0.25f * s;
	}

	return pOut;
}

// Spherical linear interpolation between two quaternions
D3DXQUATERNION* D3DXQuaternionSlerp(
  D3DXQUATERNION *pOut,
  const D3DXQUATERNION *pQ1,
  const D3DXQUATERNION *pQ2,
  FLOAT t
) {
	float dot = pQ1->x * pQ2->x + pQ1->y * pQ2->y + pQ1->z * pQ2->z + pQ1->w * pQ2->w;

	D3DXQUATERNION q2 = *pQ2;
	// If dot product is negative, negate one quaternion to take shortest path
	if (dot < 0.0f) {
		dot = -dot;
		q2.x = -q2.x;
		q2.y = -q2.y;
		q2.z = -q2.z;
		q2.w = -q2.w;
	}

	if (dot > 0.9995f) {
		// Quaternions are very close — use linear interpolation
		pOut->x = pQ1->x + t * (q2.x - pQ1->x);
		pOut->y = pQ1->y + t * (q2.y - pQ1->y);
		pOut->z = pQ1->z + t * (q2.z - pQ1->z);
		pOut->w = pQ1->w + t * (q2.w - pQ1->w);
		// Normalize
		float len = sqrtf(pOut->x*pOut->x + pOut->y*pOut->y + pOut->z*pOut->z + pOut->w*pOut->w);
		pOut->x /= len; pOut->y /= len; pOut->z /= len; pOut->w /= len;
	} else {
		float theta = acosf(dot);
		float sinTheta = sinf(theta);
		float w1 = sinf((1.0f - t) * theta) / sinTheta;
		float w2 = sinf(t * theta) / sinTheta;
		pOut->x = w1 * pQ1->x + w2 * q2.x;
		pOut->y = w1 * pQ1->y + w2 * q2.y;
		pOut->z = w1 * pQ1->z + w2 * q2.z;
		pOut->w = w1 * pQ1->w + w2 * q2.w;
	}

	return pOut;
}

// Build a rotation matrix from a quaternion
D3DXMATRIX* D3DXMatrixRotationQuaternion(
  D3DXMATRIX *pOut,
  const D3DXQUATERNION *pQ
) {
	float xx = pQ->x * pQ->x, yy = pQ->y * pQ->y, zz = pQ->z * pQ->z;
	float xy = pQ->x * pQ->y, xz = pQ->x * pQ->z, yz = pQ->y * pQ->z;
	float wx = pQ->w * pQ->x, wy = pQ->w * pQ->y, wz = pQ->w * pQ->z;

	pOut->_11 = 1.0f - 2.0f * (yy + zz);
	pOut->_12 = 2.0f * (xy + wz);
	pOut->_13 = 2.0f * (xz - wy);
	pOut->_14 = 0.0f;

	pOut->_21 = 2.0f * (xy - wz);
	pOut->_22 = 1.0f - 2.0f * (xx + zz);
	pOut->_23 = 2.0f * (yz + wx);
	pOut->_24 = 0.0f;

	pOut->_31 = 2.0f * (xz + wy);
	pOut->_32 = 2.0f * (yz - wx);
	pOut->_33 = 1.0f - 2.0f * (xx + yy);
	pOut->_34 = 0.0f;

	pOut->_41 = 0.0f;
	pOut->_42 = 0.0f;
	pOut->_43 = 0.0f;
	pOut->_44 = 1.0f;

	return pOut;
}
