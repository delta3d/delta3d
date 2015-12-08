// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtUtil/mswinmacros.h>
#if defined(DELTA_WIN32)
#include <dtUtil/mswin.h>
#include <dtCore/deltawin.h>
#include <dtUtil/log.h>
#include <algorithm>

using namespace dtCore;
using namespace dtUtil;

// Inspired by KillGLWindow in Producer's RenderSurface_Win32, unfortunately,
// we can't call it directly...
void DeltaWin::KillGLWindow()
{
   //Producer::GLContext glContext = mRenderSurface->getGLContext();
   //if(glContext)
   //{
   //   mRenderSurface->makeCurrent();
   //}

   //if( mRenderSurface->isFullScreen() )
   //{
   //   ChangeDisplaySettings(0,0);
   //   ShowCursor(true);
   //}

   //if( glContext && !wglDeleteContext(glContext) )
   //{
   //   LOG_ERROR( "Release Rendering Context Failed." );
   //}

   //Producer::Window window = mRenderSurface->getWindow();
   //HDC hdc = GetDC( window );
   //if( hdc && !ReleaseDC( window, hdc ) )
   //{
   //   LOG_ERROR( "Release Device Context Failed." );
   //}
}

//////////////////////////////////////////////////////////////////////////
DeltaWin::ResolutionVec DeltaWin::GetResolutions()
{
   HDC hDC = GetDC(GetDesktopWindow());

   DEVMODE dm;
   ResolutionVec rv;

   for (int i = 0; EnumDisplaySettings(0, i, &dm); i++)
   {
      Resolution r = { dm.dmPelsWidth,
                       dm.dmPelsHeight,
                       dm.dmBitsPerPel,
                       dm.dmDisplayFrequency };

      if (std::find(rv.begin(), rv.end(), r) == rv.end())
      {
         rv.push_back(r);
      }
   }

   ReleaseDC(GetDesktopWindow(), hDC);

   return rv;
}

//////////////////////////////////////////////////////////////////////////
bool DeltaWin::ChangeScreenResolution( int width, int height, int colorDepth, int refreshRate )
{
   //Note: If a window is fullscreen, we have to trick it a little in order
   // for it to resize correctly after the resolution changes.  To do this,
   // we store the fullscreen mode, then set it to be not fullscreened, change
   // the resolution, then restore the fullscreen mode.

   bool changeSuccessful = false;

   std::vector<bool> fullScreenVec; //container to store fullScreen state of each RenderSurface

   for( int i = 0; i < DeltaWin::GetInstanceCount(); i++ )
   {
      DeltaWin* dw = DeltaWin::GetInstance(i);

      //store fullScreen state, then set to false
      fullScreenVec.push_back(dw->GetFullScreenMode());
      dw->SetFullScreenMode(false);
   }

   DEVMODE dmScreenSettings;
   ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));

   dmScreenSettings.dmSize             = sizeof (DEVMODE);
   dmScreenSettings.dmPelsWidth        = width;
   dmScreenSettings.dmPelsHeight       = height;
   dmScreenSettings.dmBitsPerPel       = colorDepth;
   dmScreenSettings.dmDisplayFrequency = refreshRate;
   dmScreenSettings.dmFields           = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

   if ( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,
         "Resolution could not be changed to %dx%d @ %d, %d",
         width, height, colorDepth, refreshRate );

   }
   else
   {
      changeSuccessful = true;
   }

   //change back to original fullScreen state
   for( int i = 0; i < DeltaWin::GetInstanceCount(); i++ )
   {
      if(fullScreenVec[i] == true)
      {
         DeltaWin::GetInstance(i)->SetFullScreenMode(fullScreenVec[i]);
      }
   }

   return changeSuccessful;
}

DeltaWin::Resolution DeltaWin::GetCurrentResolution()
{
   HDC hdc = GetDC( GetDesktopWindow() );

   Resolution r  = { GetDeviceCaps(hdc, HORZRES),
      GetDeviceCaps(hdc, VERTRES),
      GetDeviceCaps(hdc, BITSPIXEL),
      GetDeviceCaps(hdc, VREFRESH) };
   return r;
}


//////////////////////////////////////////////////////////////////////////
void DeltaWin::HideWindowsConsole(bool hide)
{
   HWND hwnd = GetConsoleWindow();
   int cmd = hide ? 0 : 5;
   ShowWindow(hwnd, cmd);
}

#endif //WIN32
