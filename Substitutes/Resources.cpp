#include <fstream> 
#include "../dxstdafx.h"

/*
 * Very rude implementation of FindResource, LoadResource, and LockResource.
 *
 * FindResource will return NULL if the resource does not exist, which is according to specs. However,
 * if the resource exists, it will just return the file name. (This file name will never be freed, BTW.
 * Maybe that's an issue, maybe it isn't.)
 * 
 * LoadResource will return NULL if the resource cannot be loaded, which is according to specs. However,
 * if the resource can be loaded, it will just return a raw pointer to the resource data.
 * 
 * Finally, LockResource will just return what it is given in the hResData parameter, since it has already
 * been loaded.
 */

// Seems like we have to define this helper function ourselves...
long lstrcmpW(LPCWSTR s1, LPCWSTR s2) {
	while (true) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			return 0;
		}
		s1++;
		s2++;
	}
}

HRSRC FindResource( HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType ) {
	char *prefix;
	char *suffix;

	if (lstrcmpW(lpType, L"TRACK") == 0) {
		prefix = "Tracks/";
		suffix = ".bin";
	} else if (lstrcmpW(lpType, L"WAVE") == 0) {
		prefix = "Sounds/";
		suffix = ".wav";
	} else {
		ErrorPrintf("Unknown resource type: %ls (resource name: %ls)\n", lpType, lpName);
		return null;
	}

    int pathLength = strlen(prefix) + wcslen(lpName) + strlen(suffix) + 1;
    char *fileName = (char*) malloc(pathLength);
    sprintf(fileName, "%s%ls%s", prefix, lpName, suffix);

	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (file.tellg() < 0) {
		ErrorPrintf("File '%s' does not exist", fileName);
		free(fileName);
		return null;
	}

	return fileName;
}

HGLOBAL LoadResource( HMODULE hModule, HRSRC hResInfo ) {
	char *fileName = (char*) hResInfo;
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	if (size < 0) {
		free(fileName);
		ErrorPrintf("File %s does not exist\n", fileName);
		return null;
	}
	file.seekg(0, std::ios::beg);

	char *buffer = (char*)malloc(size);

	if (file.read(buffer, size)) {
		DebugPrintf("Loaded the resource %s (%ld bytes)\n", fileName, size);
		return buffer;
	}

	ErrorPrintf("Could not load %s\n", fileName);
	return null;
}

LPVOID LockResource( HGLOBAL hResData ) {
	return hResData;
}
