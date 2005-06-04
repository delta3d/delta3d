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
   mRenderSurface->setWindowName( title );
}

void DeltaWin::ShowCursor( bool show )
{
   mShowCursor = show;

   int x,y;
   unsigned w,h;
   mRenderSurface->getWindowRectangle(x,y,w,h);
   
   //Then move the cursor to be on our window'
   mRenderSurface->positionPointer((x+w)/2, (y+h)/2);
   
   //Tell Producer
   mRenderSurface->useCursor(mShowCursor);

}

void DeltaWin::SetFullScreenMode( bool enable )
{
   mRenderSurface->fullScreen(enable);
   mRenderSurface->useBorder(enable);
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
   unsigned w,h;
   //mRenderSurface->getScreenSize(w,h);
   //CFDictionaryRef ref = CGDisplayCurrentMode(mRenderSurface->getDisplay());
   //ref
   Resolution r = { 1280,854,24,60 };
   return r;
}
