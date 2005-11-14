// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include <cassert>

#include <Producer/Types>
#include <Producer/KeyboardMouse>
#include <dtCore/deltawin.h>
#include <dtUtil/log.h>
#include <Carbon/Carbon.h>

using namespace dtCore;
using namespace std;

void DeltaWin::KillGLWindow()
{
}

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

DeltaWin::ResolutionVec DeltaWin::GetResolutions()
{
  DeltaWin::ResolutionVec rv;
  return rv;
}

// get a double value from a dictionary
static double getDictDouble (CFDictionaryRef refDict, CFStringRef key)
{
   double double_value;
   CFNumberRef number_value = (CFNumberRef) CFDictionaryGetValue(refDict, key);
   if (!number_value) // if can't get a number for the dictionary
      return -1;  // fail
   if (!CFNumberGetValue(number_value, kCFNumberDoubleType, &double_value)) // or if cant convert it
      return -1; // fail
   return double_value; // otherwise return the long value
}

bool DeltaWin::ChangeScreenResolution( int width, int height, int colorDepth, int refreshRate ) 
{
   return false;
}

DeltaWin::Resolution DeltaWin::GetCurrentResolution()
{
   int w,h;

   static Producer::Display *displays = NULL;
   static CGDisplayCount numDisplays = 0;
   
   DeltaWin::Resolution r = { 0,0,0,0 };

   if( CGGetActiveDisplayList( 0, NULL, &numDisplays ) != CGDisplayNoErr )
   {
      LOG_ERROR("CGGetActiveDisplayList() falied");
      return r;
   }
   
   if( numDisplays == 0 )
   {
      LOG_ERROR("openDisplay: No Displays!");
      return r;
   }
   
   displays = new Producer::Display[numDisplays];
   if( CGGetActiveDisplayList( numDisplays, displays, &numDisplays ) != CGDisplayNoErr )
   {
      LOG_ERROR("CGGetActiveDisplayList() falied\n");
      delete[] displays;
      return r;
   }
   
   Producer::Display* dpy = &displays[0];
   delete[] displays;

   w = CGDisplayPixelsWide(*dpy);
   h = CGDisplayPixelsHigh(*dpy);
      
   
   // add next line and on following line replace hard coded depth and refresh rate
   int refresh =  (int)getDictDouble (CGDisplayCurrentMode (*dpy), kCGDisplayRefreshRate);
   int depth = (int)CGDisplayBitsPerPixel(*dpy);
   DeltaWin::Resolution result = { w,h, depth, refresh };
   return result;
}
