#pragma once

#ifdef NDEBUG
#define BUILD_TYPE "Release build"
#else
#define BUILD_TYPE "Debug build"
#endif

#define DATA_LAYOUT_LINEAR
//#define DATA_LAYOUT_EMBEDDED_QUADTREE
//#define DATA_LAYOUT_HIERARCHICAL_PI_ORDER

