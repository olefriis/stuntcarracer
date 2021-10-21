typedef struct D3DVECTOR {
  FLOAT x;
  FLOAT y;
  FLOAT z;
} D3DVECTOR, *LPD3DXVECTOR3;

typedef struct D3DXVECTOR3 : public D3DVECTOR
{
public:
    D3DXVECTOR3() {};
    D3DXVECTOR3( FLOAT x, FLOAT y, FLOAT z ) {
		this->x = x;
		this->y = y;
		this->z = z;
	};
} D3DXVECTOR3;
