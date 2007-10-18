// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
#include <dtCore/deltawin.h>
#include <dtUtil/log.h>

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

DeltaWin::ResolutionVec DeltaWin::GetResolutions()
{
   HDC hDC = GetDC(GetDesktopWindow());

   Resolution currentRes = GetCurrentResolution();
   int currentDepth = currentRes.bitDepth;

   DEVMODE dm;
   ResolutionVec rv;

   int i = 0;

   for( i = 0; EnumDisplaySettings( 0, i, &dm ); i++ )
   {

      Resolution r = { dm.dmPelsWidth,
         dm.dmPelsHeight,
         dm.dmBitsPerPel,
         dm.dmDisplayFrequency };

      rv.push_back( r );
   }

   int numResolutions = i;

   ReleaseDC(GetDesktopWindow(), hDC);

   return rv;
}

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

#endif //WIN32
