// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#include <dtCore/deltawin.h>
#include <dtUtil/log.h>
#include <Producer/RenderSurface>

using namespace dtCore;
using namespace dtUtil;

// Inspired by KillGLWindow in Producer's RenderSurface_Win32, unfortunately,
// we can't call it directly...
void DeltaWin::KillGLWindow()
{
   Producer::GLContext glContext = mRenderSurface->getGLContext();
   if(glContext)
   {
      mRenderSurface->makeCurrent();
   }

   if( mRenderSurface->isFullScreen() )
   {
      ChangeDisplaySettings(0,0);    
      ShowCursor(true);
   }

   if( glContext && !wglDeleteContext(glContext) )
   {
      LOG_ERROR( "Release Rendering Context Failed." );
   }

   Producer::Window window = mRenderSurface->getWindow();
   HDC hdc = GetDC( window );
   if( hdc && !ReleaseDC( window, hdc ) )
   {
      LOG_ERROR( "Release Device Context Failed." );
   }
}

// Producer::RenderSurface must realized for this to work
void DeltaWin::SetWindowTitle( const std::string& title )
{
   mRenderSurface->setWindowName( title );

   HWND win = mRenderSurface->getWindow();
   SetWindowText( win, title.c_str() ); 
}

void DeltaWin::ShowCursor( bool show )
{
   mShowCursor = show;

   POINT coords;
   GetCursorPos(&coords);

   //Then move the cursor to be on our window'
   int x,y,w,h;
   GetPosition(x, y, w, h); 
   mRenderSurface->positionPointer((x+w)/2, (y+h)/2);

   //Tell Producer
   mRenderSurface->useCursor(mShowCursor);

   //Then move the cursor back to where it started from
   SetCursorPos(coords.x, coords.y);
}

void DeltaWin::SetFullScreenMode( bool enable )
{
   mRenderSurface->fullScreen(enable);
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
   bool changeSuccessful = false;

   std::vector<bool> fullScreenVec; //container to store fullScreen state of each RenderSurface
   std::vector<unsigned int> screenHeightVec; 

   for( int i = 0; i < DeltaWin::GetInstanceCount(); i++ )
   {
      DeltaWin* dw = DeltaWin::GetInstance(i);

      //store fullScreen state, then set to false
      fullScreenVec.push_back(dw->GetFullScreenMode());
      dw->SetFullScreenMode(false);

      //get "real" screen width and height

      unsigned int sWidth, sHeight;    
      dw->GetRenderSurface()->getScreenSize( sWidth, sHeight );
      
      screenHeightVec.push_back(sHeight);

      //notify all render surfaces that resolution has changed,
      //we must pass screenHeight-height to properly place new window
      if(unsigned(height) < screenHeightVec[0])
      {
         dw->GetRenderSurface()->setCustomFullScreenRectangle( 0, screenHeightVec[0] - height, width, height );
      }
      
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
      if(unsigned(height) > screenHeightVec[i])
      {
         DeltaWin::GetInstance(i)->GetRenderSurface()->setCustomFullScreenRectangle( 0, 0, width, height );
      }

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
   HDC hdc = GetDC( GetDesktopWindow() );

   Resolution r  = { GetDeviceCaps(hdc, HORZRES),
      GetDeviceCaps(hdc, VERTRES),
      GetDeviceCaps(hdc, BITSPIXEL),
      GetDeviceCaps(hdc, VREFRESH) };
   return r;
}

#endif //WIN32