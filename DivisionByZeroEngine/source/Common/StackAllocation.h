#pragma once

#include "Platform/PlatformDefines.h"

#if IS_WINDOWS_PLATFORM

#include <malloc.h>

#ifndef ALLOCATE_STACK_MEMORY
#define ALLOATE_STACK_MEMORY(sizeInBytes) alloca(sizeInBytes)
#endif // ALLOCATE_STACK_MEMORY

#endif // IS_WINDOWS_PLATFORM
