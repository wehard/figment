#pragma once

#if defined(__APPLE__)
#define FIGMENT_MACOS
#elif defined(__EMSCRIPTEN__)
#define FIGMENT_WEB
#endif
