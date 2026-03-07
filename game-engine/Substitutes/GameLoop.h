//--------------------------------------------------------------------------------------
// Callback registration 
//--------------------------------------------------------------------------------------
typedef bool    (CALLBACK *LPDXUTCALLBACKISDEVICEACCEPTABLE)( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
typedef bool    (CALLBACK *LPDXUTCALLBACKMODIFYDEVICESETTINGS)( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
typedef bool    (CALLBACK *LPDXUTCALLBACKMODIFYDEVICESETTINGS)( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
typedef HRESULT (CALLBACK *LPDXUTCALLBACKDEVICECREATED)( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
typedef HRESULT (CALLBACK *LPDXUTCALLBACKDEVICERESET)( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
typedef void    (CALLBACK *LPDXUTCALLBACKDEVICEDESTROYED)( void* pUserContext );
typedef void    (CALLBACK *LPDXUTCALLBACKDEVICELOST)( void* pUserContext );

// Device callbacks
void DXUTSetCallbackDeviceCreated( LPDXUTCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext = NULL );
void DXUTSetCallbackDeviceReset( LPDXUTCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext = NULL );
void DXUTSetCallbackDeviceLost( LPDXUTCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext = NULL );
void DXUTSetCallbackDeviceDestroyed( LPDXUTCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext = NULL );
void DXUTSetCallbackDeviceChanging( LPDXUTCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings, void* pUserContext = NULL );

typedef void    (CALLBACK *LPDXUTCALLBACKFRAMEMOVE)( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
typedef void    (CALLBACK *LPDXUTCALLBACKFRAMERENDER)( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );

// Frame callbacks
void DXUTSetCallbackFrameMove( LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext = NULL );
void DXUTSetCallbackFrameRender( LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext = NULL );

typedef void    (CALLBACK *LPDXUTCALLBACKKEYBOARD)( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
typedef void    (CALLBACK *LPDXUTCALLBACKMOUSE)( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext );
typedef LRESULT (CALLBACK *LPDXUTCALLBACKMSGPROC)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );

// Message callbacks
void DXUTSetCallbackKeyboard( LPDXUTCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext = NULL );
void DXUTSetCallbackMouse( LPDXUTCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove = false, void* pUserContext = NULL );
void DXUTSetCallbackMsgProc( LPDXUTCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext = NULL );
