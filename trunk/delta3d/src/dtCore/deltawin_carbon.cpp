// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include <cassert>

#include "Producer/KeyboardMouse"
#include "dtCore/deltawin.h"


using namespace dtCore;
using namespace std;

// Producer::RenderSurface must realized for this to work
void DeltaWin::SetWindowTitle( const std::string& title )
{
}

void DeltaWin::ShowCursor( bool show )
{
}

void DeltaWin::SetFullScreenMode( bool enable )
{
}

ResolutionVec DeltaWin::GetResolutions()
{
  ResolutionVec rv;
  return rv;
}

bool DeltaWin::ChangeScreenResolution( int width, int height, int colorDepth, int refreshRate ) 
{
   return false;
}

Resolution DeltaWin::GetCurrentResolution()
{
   Resolution r = { 0,0,0,0 };
   return r;
}
