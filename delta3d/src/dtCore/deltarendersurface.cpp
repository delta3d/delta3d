/* 
* deltarendersurface.cpp: Implementation of the DeltaRenderSurface class
*/

#include "dtCore/deltarendersurface.h"

using namespace dtCore;

void
DeltaRenderSurface::GetScreenWidthHeight( unsigned int* width, unsigned int* height )
{
   width = &_screenWidth;
   height = &_screenHeight;
}

void
DeltaRenderSurface::SetScreenWidthHeight( unsigned int width, unsigned int height )
{
   _screenWidth = width;
   _screenHeight = height;
}
