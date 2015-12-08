// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(WIN32) && !defined(__APPLE__)

#include <cassert>

#include <dtCore/deltawin.h>
#include <dtUtil/log.h>

#include <osgViewer/GraphicsWindow>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/xf86vmode.h>

using namespace dtCore;
using namespace std;

class DOXInitThreads
{
public:
   DOXInitThreads()
   {
      if (XInitThreads() == 0)
      {
         LOG_ERROR("Initializing X11 threads failed!  Will continue anyway.");
      }
      else
      {
         LOG_INFO("Initializing X11 threads succeeded!");
      }
   }
};

static DOXInitThreads threadiniter;

void DeltaWin::KillGLWindow()
{
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
      osg::GraphicsContext::getWindowingSystemInterface()->getScreenResolution(0, screenWidth, screenHeight);

      //notify all render surfaces that resolution has changed,
      //we must pass screenHeight-height to properly place new window
      dw->GetOsgViewerGraphicsWindow()->setWindowRectangle( 0, screenHeight-height, width, height );
      dw->GetOsgViewerGraphicsWindow()->setWindowDecoration(false);
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
   {
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FILE__, 
                                    "Resolution could not be changed to %dx%d @ %d, %d",
                                    width, height, colorDepth, refreshRate );
   }

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
         DeltaWin::GetInstance(i)->GetPosition(x,y,w,h);
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

//////////////////////////////////////////////////////////////////////////
void DeltaWin::HideWindowsConsole(bool hide)
{
}

#endif // defined(__GNUG__) && !defined(WIN32) && !defined(__APPLE__)
