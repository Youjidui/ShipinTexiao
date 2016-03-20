#pragma once

#ifdef RESOURCE_LIBRARY_EXPORT
#define RESOURCE_CLASS_EXPORT __declspec(dllexport)
#else
#define RESOURCE_CLASS_EXPORT __declspec(dllimport)
#endif


RESOURCE_CLASS_EXPORT 
bool ZipShaderResourcesToFile(LPCTSTR pszTargetFilename, LPCTSTR pszSrcResourceDir);
