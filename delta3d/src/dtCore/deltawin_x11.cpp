// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(WIN32) && !defined(__APPLE__)

#include <cassert>

#include <Producer/KeyboardMouse>
#include <dtCore/deltawin.h>
#include <dtCore/notify.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/xf86vmode.h>

using namespace dtCore;
using namespace std;

void DeltaWin::KillGLWindow()
{
}

// Producer::RenderSurface must realized for this to work
void DeltaWin::SetWindowTitle( const std::string& title )
{
   mRenderSurface->setWindowName( title );

   if( mRenderSurface->isRealized() )
   {
      Display* dpy = mRenderSurface->getDisplay();
      Window win = mRenderSurface->getWindow();

      XStoreName( dpy, win, title.c_str() );
      XSetIconName( dpy, win, title.c_str() );
      XFlush( dpy );
   }
}

void DeltaWin::ShowCursor( bool show )
{
   mShowCursor = show;

   //Then move the cursor to be on our window'
   int x,y,w,h;
   GetPosition(&x, &y, &w, &h); //winuser.h
   mRenderSurface->positionPointer((x+w)/2, (y+h)/2);

   //Tell Producer
   mRenderSurface->useCursor(mShowCursor);
}

void DeltaWin::SetFullScreenMode( bool enable )
{
   mRenderSurface->fullScreen(enable);
   mRenderSurface->useBorder(!enable);
}

DeltaWin::ResolutionVec DeltaWin::GetResolutions()
{
   Display* dpy = XOpenDisplay(NULL);
   int screenNum = DefaultScreen(dpy);


   Resolution currentRes = GetCurrentResolution();

   int numResolutions;
   XF86VidModeModeInfo** resolutions;
   XF86VidModeGetAllModeLines(dpy,
      screenNum,
      &numResolutions,
      &resolutions );

   DeltaWin::ResolutionVec rv;

   for(int i=0; i < numResolutions; i++)
   {
      int refreshRate = CalcRefreshRate(resolutions[i]->htotal, resolutions[i]->vtotal, resolutions[i]->dotclock );

      DeltaWin::Resolution r = { resolutions[i]->hdisplay,
         resolutions[i]->vdisplay,
         currentRes.bitDepth,
         refreshRate };

      rv.push_back( r );
   }

   return rv;
}

bool DeltaWin::ChangeScreenResolution( int width, int height, int colorDepth, int refreshRate ) 
{
   bool changeSuccessful = false;

   std::vector<bool> fullScreenVec; //container to store fullScreen state of each RenderSurface

   for( int i = 0; i < DeltaWin::GetInstanceCount(); i++ )
   {
      DeltaWin* dw = DeltaWin::GetInstance(i);

      //store fullScreen state, then set to false
      fullScreenVec.push_back(dw->GetFullScreenMode());
      dw->SetFullScreenMode(false);

      //get "real" screen width and height
      unsigned int screenHeight;
      unsigned int screenWidth;
      dw->GetRenderSurface()->getScreenSize( screenWidth, screenHeight );

      //notify all render surfaces that resolution has changed,
      //we must pass screenHeight-height to properly place new window
      dw->GetRenderSurface()->setCustomFullScreenRectangle( 0, screenHeight-height, width, height );
   }

   Display* dpy = XOpenDisplay(NULL);
   int screenNum = DefaultScreen(dpy);

   int dotClock;
   XF86VidModeModeLine modeline;
   XF86VidModeGetModeLine( dpy, screenNum, &dotClock, &modeline);

   int tempRefresh = CalcRefreshRate( modeline.htotal, modeline.vtotal, dotClock );

   //test if new value is same as current, if so don't do anything
   if( modeline.hdisplay == width && modeline.vdisplay == height && tempRefresh == refreshRate )
   {
      changeSuccessful = true;
   }

   int numResolutions;
   XF86VidModeModeInfo** resolutions;
   XF86VidModeGetAllModeLines(dpy,
      screenNum,
      &numResolutions,
      &resolutions);

   for(int i = 0; i < numResolutions && !changeSuccessful; i++)
   {
      XF86VidModeModeInfo* tempRes = resolutions[i];

      tempRefresh = CalcRefreshRate( tempRes->htotal, tempRes->vtotal, tempRes->dotclock );

      if( tempRes->hdisplay == width && tempRes->vdisplay == height && tempRefresh == refreshRate )
      {
         XF86VidModeSwitchToMode( dpy, screenNum, tempRes );
         XF86VidModeSetViewPort( dpy,screenNum, 0, 0 );
         XSync(dpy,false);

         changeSuccessful = true;
      }
   }

   if(!changeSuccessful)
      Notify(WARN,"Resolution could not be changed to %dx%d @ %d, %d", width, height, colorDepth, refreshRate );

   //change back to original fullScreen state
   for( int i = 0; i < DeltaWin::GetInstanceCount(); i++ )
   {
      if(fullScreenVec[i])
      {
         DeltaWin::GetInstance(i)->SetFullScreenMode(fullScreenVec[i]);
      }
      else
      {
         //reset window position
         int x,y,w,h;
         DeltaWin::GetInstance(i)->GetPosition(&x,&y,&w,&h);
         DeltaWin::GetInstance(i)->SetPosition(x,y,w,h);
      }
   }

   return changeSuccessful;
}

DeltaWin::Resolution DeltaWin::GetCurrentResolution()
{
   Display* dpy = XOpenDisplay(NULL);
   int screenNum = DefaultScreen(dpy);

   int dotclock;
   XF86VidModeModeLine modeline;
   XF86VidModeGetModeLine( dpy, screenNum, &dotclock, &modeline);

   int thorz = static_cast<int>(modeline.hdisplay);
   int tvert = static_cast<int>(modeline.vdisplay);
   int tfreq = CalcRefreshRate( modeline.htotal, modeline.vtotal, dotclock );
   int tdepth = XDefaultDepth( dpy, screenNum );

   DeltaWin::Resolution r = { thorz, tvert, tdepth, tfreq };
   return r;
}

#endif // defined(__GNUG__) && !defined(WIN32) && !defined(__APPLE__)