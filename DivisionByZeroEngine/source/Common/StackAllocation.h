#pragma once

#include "Platform/PlatformDefines.h"

#if IS_WINDOWS_PLATFORM

#include <malloc.h>

#ifndef DBZ_ALLOCATE_STACK_MEMORY
#define DBZ_ALLOCATE_STACK_MEMORY(sizeInBytes) alloca(sizeInBytes)
#endif // DBZ_ALLOCATE_STACK_MEMORY

#endif // IS_WINDOWS_PLATFORM
