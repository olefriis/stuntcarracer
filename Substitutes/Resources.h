/*
 * Resource loading.
 * See https://docs.microsoft.com/en-us/windows/win32/menurc/using-resources
 */

HRSRC FindResource( HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType );
HGLOBAL LoadResource( HMODULE hModule, HRSRC hResInfo );
LPVOID LockResource( HGLOBAL hResData );
