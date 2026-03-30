#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <cmath>
#include "../../sdk/include/CEffectPluginIntf.h"
#include "../../sdk/include/AVSConsts.h"

WCHAR* export_str(const WCHAR* ptr)
{
	if (ptr == NULL)
		return NULL;

	size_t len = wcslen(ptr);
	WCHAR* result = new WCHAR[len + 1];

	if (result == NULL)
		return NULL;

	wcscpy_s(result, len + 1, ptr);

	return result;
}
void release_export_ptr(const WCHAR* ptr)
{
	delete[] ptr;
}