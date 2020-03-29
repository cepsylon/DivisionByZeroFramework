#pragma once

#include "GlobalDefines.h"

#if IS_WINDOWS_PLATFORM

struct HINSTANCE__;

namespace Process
{
	using Handle = HINSTANCE__*;
	Handle GetHandle();
}

#endif // IS_WIN32_API
