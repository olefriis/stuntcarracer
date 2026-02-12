#include <GLES2/gl2.h>

typedef struct _D3DMATRIX {
    union {
        struct {
			// Direct3D packs the floats "row-first":
            //float        _11, _12, _13, _14;
            //float        _21, _22, _23, _24;
            //float        _31, _32, _33, _34;
            //float        _41, _42, _43, _44;

			// However, pack the floats "column-first", since that's the way OpenGL expects them
            float        _11, _21, _31, _41;
            float        _12, _22, _32, _42;
            float        _13, _23, _33, _43;
            float        _14, _24, _34, _44;
        };
        float m[4][4];
		GLfloat glFloats[16];
    };
} D3DMATRIX;

typedef struct D3DXMATRIX : public D3DMATRIX
{
public:
    D3DXMATRIX() {};
    D3DXMATRIX(const D3DMATRIX &other) { memcpy(this, &other, sizeof(D3DMATRIX)); }

    D3DXMATRIX& operator=(const D3DMATRIX &other) { memcpy(this, &other, sizeof(D3DMATRIX)); return *this; }

    D3DXMATRIX operator+(const D3DXMATRIX &other) const {
        D3DXMATRIX result;
        for (int i = 0; i < 16; i++)
            result.glFloats[i] = glFloats[i] + other.glFloats[i];
        return result;
    }

    D3DXMATRIX operator-(const D3DXMATRIX &other) const {
        D3DXMATRIX result;
        for (int i = 0; i < 16; i++)
            result.glFloats[i] = glFloats[i] - other.glFloats[i];
        return result;
    }

    friend D3DXMATRIX operator*(float scalar, const D3DXMATRIX &m) {
        D3DXMATRIX result;
        for (int i = 0; i < 16; i++)
            result.glFloats[i] = scalar * m.glFloats[i];
        return result;
    }
} D3DXMATRIX, *LPD3DXMATRIX;

typedef struct D3DXQUATERNION {
    FLOAT x, y, z, w;
    D3DXQUATERNION() : x(0), y(0), z(0), w(1) {}
    D3DXQUATERNION(FLOAT x, FLOAT y, FLOAT z, FLOAT w) : x(x), y(y), z(z), w(w) {}
} D3DXQUATERNION;

D3DXQUATERNION* D3DXQuaternionRotationMatrix(
  D3DXQUATERNION *pOut,
  const D3DXMATRIX *pM
);

D3DXQUATERNION* D3DXQuaternionSlerp(
  D3DXQUATERNION *pOut,
  const D3DXQUATERNION *pQ1,
  const D3DXQUATERNION *pQ2,
  FLOAT t
);

D3DXMATRIX* D3DXMatrixRotationQuaternion(
  D3DXMATRIX *pOut,
  const D3DXQUATERNION *pQ
);

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
