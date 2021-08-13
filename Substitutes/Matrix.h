#include <GLES2/gl2.h>

typedef struct _D3DMATRIX {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
        float m[4][4];
		GLfloat glFloats[16];
    };
} D3DMATRIX;

typedef struct D3DXMATRIX : public D3DMATRIX
{
public:
    D3DXMATRIX() {};
} D3DXMATRIX, *LPD3DXMATRIX;

D3DXMATRIX* D3DXMatrixPerspectiveFovLH(
  D3DXMATRIX *pOut,
  FLOAT      fovy,
  FLOAT      Aspect,
  FLOAT      zn,
  FLOAT      zf
);

D3DXMATRIX* D3DXMatrixIdentity(
  D3DXMATRIX *pOut
);

D3DXMATRIX* D3DXMatrixRotationX(
  D3DXMATRIX *pOut,
  FLOAT      Angle
);

D3DXMATRIX* D3DXMatrixRotationY(
  D3DXMATRIX *pOut,
  FLOAT      Angle
);

D3DXMATRIX* D3DXMatrixRotationZ(
  D3DXMATRIX *pOut,
  FLOAT      Angle
);

D3DXMATRIX* D3DXMatrixTranslation(
  D3DXMATRIX *pOut,
  FLOAT      x,
  FLOAT      y,
  FLOAT      z
);

D3DXMATRIX* D3DXMatrixLookAtLH(
  D3DXMATRIX  *pOut,
  const D3DXVECTOR3 *pEye,
  const D3DXVECTOR3 *pAt,
  const D3DXVECTOR3 *pUp
);

D3DXMATRIX* D3DXMatrixMultiply(
  D3DXMATRIX *pOut,
  const D3DXMATRIX *pM1,
  const D3DXMATRIX *pM2
);
