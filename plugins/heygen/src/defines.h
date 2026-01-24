#ifndef DEFINES_H
#define DEFINES_H

#define RELEASE_PTR(ptr) if (nullptr != ptr) delete ptr, ptr = NULL;
#define LOGW(s) OutputDebugStringW(s)
#define LOG(s) OutputDebugString(s)

#define PLUGIN_ID 	L"heygen.plugin"

#endif // DEFINES_H
