#pragma once

#if defined(_WIN32)

#define IS_WINDOWS_PLATFORM 1

#if defined(_WIN64)
#define IS_X86 0
#define IS_X64 1
#else
#define IS_X86 1
#define IS_X64 0
#endif // _WIN64

#else

#define IS_WINDOWS_PLATFORM 0
#define IS_X86 0
#define IS_X64 0

#endif // _WIN32

#if defined(_DEBUG)
#define IS_DEBUG_BUILD 1
#else
#define IS_DEBUG_BUILD 0
#endif // _DEBUG
