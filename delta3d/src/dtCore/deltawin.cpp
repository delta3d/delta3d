// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include <cassert>

#include "Producer/KeyboardMouse"
#include "dtCore/deltawin.h"
#include "dtCore/notify.h"

using namespace dtCore;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(DeltaWin)

class InputCallback : public Producer::KeyboardMouseCallback
{
   public:

      InputCallback(Keyboard* keyboard, Mouse* mouse)
         : mKeyboard(keyboard), mMouse(mouse)
      {}

      void mouseScroll(ScrollingMotion sm)
      {
         mMouse->mouseScroll(sm);
      }

       void mouseMotion(float x, float y)
      {
         mMouse->mouseMotion( x, y );
      }

      void passiveMouseMotion(float x, float y)
      {
         mMouse->passiveMouseMotion( x, y );
      }

      void buttonPress(float x, float y, unsigned int button)
      {
         mMouse->buttonPress(x, y, button);
      }

      void doubleButtonPress(float x, float y, unsigned int button)
      {
         mMouse->doubleButtonPress(x, y, button);
      }

      void buttonRelease(float x, float y, unsigned int button)
      {
         mMouse->buttonRelease(x, y, button);
      }

      void keyPress(Producer::KeyCharacter kc)
      {
         mKeyboard->keyPress(kc);
      }

      void keyRelease(Producer::KeyCharacter kc)
      {
         mKeyboard->keyRelease(kc);
      }

      void specialKeyPress(Producer::KeyCharacter kc)
      {
         mKeyboard->specialKeyPress(kc);
      }

      void specialKeyRelease(Producer::KeyCharacter kc)
      {
         mKeyboard->specialKeyRelease(kc);
      }


   private:

      osg::ref_ptr<Keyboard> mKeyboard;
      osg::ref_ptr<Mouse> mMouse;
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


DeltaWin::DeltaWin(string name, int x, int y, int width, int height, bool cursor, bool fullScreen) :
Base(name),
mShowCursor(true)
{
   RegisterInstance(this);

   mRenderSurface = new DeltaRenderSurface; 
   
   mKeyboard = new Keyboard;
   mMouse = new Mouse;

   mKeyboardMouse = new Producer::KeyboardMouse(mRenderSurface);
   mKeyboardMouse->setCallback( new InputCallback(mKeyboard.get(), mMouse.get()) );
   mKeyboardMouse->startThread();

   if(!fullScreen)
   {
      SetPosition(x, y, width, height);
   }
   else
   {
   }

   SetName( name );
   SetWindowTitle(name.c_str());
   ShowCursor( cursor );

}

DeltaWin::DeltaWin(string name, DeltaRenderSurface* rs, Producer::InputArea* ia) :
Base(name),
mShowCursor(true),
mRenderSurface(rs)
{
   RegisterInstance(this);
   
   mKeyboard = new Keyboard;
   mMouse = new Mouse;

   if(ia) // use the passed InputArea if not NULL
      mKeyboardMouse = new Producer::KeyboardMouse(ia);
   else // otherwise use the passed DeltaRenderSurface
      mKeyboardMouse = new Producer::KeyboardMouse(mRenderSurface);

   mKeyboardMouse->setCallback( new InputCallback(mKeyboard.get(), mMouse.get()) );
   mKeyboardMouse->startThread();

   ShowCursor();
}


DeltaWin::~DeltaWin()
{
   mKeyboardMouse->cancel();

   mKeyboardMouse = NULL;

   mRenderSurface = NULL;

   DeregisterInstance(this);
}

/** Set the position and size of the DeltaWin in screen coordinates
* @param x The left edge of the window in screen coordinates
* @param y The bottom edge of the window in screen coordinates
* @param width The width of the window
* @param height The height of the window
*/
void DeltaWin::SetPosition(const int x, const int y, const int width, const int height)
{
   mRenderSurface->setWindowRectangle(x, y, width, height);

   #if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
   mRenderSurface->useBorder(true);
   #endif
}

void DeltaWin::GetPosition( int *x, int *y,int *width, int *height )
{
   unsigned int w, h;
   
   mRenderSurface->getWindowRectangle( *x, *y, w, h );
   *width = w;
   *height = h;
}

// Producer::RenderSurface must realized for this to work
void DeltaWin::SetWindowTitle(const char *title)
{
   mRenderSurface->setWindowName(title);

   //Producer doesn't dynamically re-title the window so we do it ourself here
    
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
   HWND win = mRenderSurface->getWindow();
   SetWindowText(win, title); //from winuser.h
#else
   if( mRenderSurface->isRealized() )
   {
      Display* dpy = mRenderSurface->getDisplay();
      Window win = mRenderSurface->getWindow();

      XStoreName( dpy, win, title );
      XSetIconName( dpy, win, title );
      XFlush( dpy );
   }
#endif
}

const std::string DeltaWin::GetWindowTitle() const
{
   return mRenderSurface->getWindowName();
}

void DeltaWin::ShowCursor(const bool show )
{
   mShowCursor = show;

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
   //Win32: gotta do a little cursor game to make this work
   //First, save the current position of the cursor
   POINT coords;
   GetCursorPos(&coords);
#endif 
   
   //Then move the cursor to be on our window'
   int x,y,w,h;
   GetPosition(&x, &y, &w, &h); //winuser.h
   mRenderSurface->positionPointer((x+w)/2, (y+h)/2);

   //Tell Producer
   mRenderSurface->useCursor(mShowCursor);

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
   //Then move the cursor back to where it started from
   SetCursorPos(coords.x, coords.y);
#endif
}

void DeltaWin::SetFullScreenMode( bool enable )
{
   mRenderSurface->fullScreen(enable);

   #if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
   mRenderSurface->useBorder(enable);
   #endif
}

/*!
 * Calculate the screen coordinates given a window coordinate. Screen
 * coordinate (0,0) is located in the lower left of the display.
 *
 * @param x : window x coordinate [-1, 1]
 * @param y : window y coordinate [-1, 1]
 * @param &pixel_x : The screen X pixel equivelent [out]
 * @param &pixel_y : The screen Y pixel equivelent [out]
 *
 * @return bool  : Returns true if the (x,y) is a valid window coordinate
 */
bool DeltaWin::CalcPixelCoords(const float x, const float y, float &pixel_x, float &pixel_y)
{
   if ( x < -1.0f || x > 1.0f ) return false;
   if ( y < -1.0f || y > 1.0f ) return false;
   
   int wx, wy;
   unsigned int w, h;
   GetRenderSurface()->getWindowRectangle( wx, wy, w, h );

   pixel_x = ( w/2 ) * (x + 1.0f);
   pixel_y = ( h/2 ) * (1.0f - y);
   
   return true;

}

bool DeltaWin::CalcWindowCoords(const float pixel_x, const float pixel_y, float &x, float &y)
{
   int wx, wy;
   unsigned int w, h;
   GetRenderSurface()->getWindowRectangle( wx, wy, w, h );

   if (pixel_x < 0 || pixel_x > w ) return false;
   if (pixel_y < 0 || pixel_y > h ) return false;

   if( w != 0 && y != 0)
   {
      x = ( 2 * pixel_x / w ) - 1;
      y = ( 2 * pixel_y / h ) - 1;
      
      return true;
   }
   else
   {
      Notify(DEBUG_INFO,"Window size of 0");
      return false;
   }
}


ResolutionVec DeltaWin::GetResolutions( void )
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

   HDC hDC = GetDC(GetDesktopWindow());

   Resolution currentRes = GetCurrentResolution();
   int currentDepth = currentRes.bitDepth;
     
   DEVMODE dm;
   ResolutionVec rv;

   int i = 0;
   for (i = 0; EnumDisplaySettings(NULL, i, &dm); i++) {
 
     Resolution r = { dm.dmPelsWidth,
                      dm.dmPelsHeight,
                      dm.dmBitsPerPel,
                      dm.dmDisplayFrequency };

     rv.push_back( r );
   }
   int numResolutions = i;
    
   ReleaseDC(GetDesktopWindow(), hDC);
 
   return rv;

#else

   Display* dpy = XOpenDisplay(NULL);
   int screenNum = DefaultScreen(dpy);

   
   Resolution currentRes = GetCurrentResolution();
   
   int numResolutions;
   XF86VidModeModeInfo** resolutions;
   XF86VidModeGetAllModeLines(dpy,
                              screenNum,
                              &numResolutions,
                              &resolutions );

   ResolutionVec rv;

   for(int i=0; i < numResolutions; i++)
   {
      int refreshRate = CalcRefreshRate(resolutions[i]->htotal, resolutions[i]->vtotal, resolutions[i]->dotclock );
      
      Resolution r = { resolutions[i]->hdisplay,
                       resolutions[i]->vdisplay,
                       currentRes.bitDepth,
                       refreshRate };

      rv.push_back( r );
   }

   return rv;
   
#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
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

      //notify all render surfaces that resolution has changed
      dw->GetRenderSurface()->SetScreenWidthHeight(   static_cast<unsigned int>(width),
                                                      static_cast<unsigned int>(height) );
   }

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

   DEVMODE dmScreenSettings;                                                           
   ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));
   
   dmScreenSettings.dmSize             = sizeof (DEVMODE);             
   dmScreenSettings.dmPelsWidth        = width;                                        
   dmScreenSettings.dmPelsHeight       = height;                              
   dmScreenSettings.dmBitsPerPel       = colorDepth;    
   dmScreenSettings.dmDisplayFrequency = refreshRate;
   dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

   if ( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
   {
      Notify(WARN,"Resolution could not be changed to %dx%d @ %d, %d", width, height, colorDepth, refreshRate );
   }
   else
   {
      changeSuccessful = true;
   }

#else

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
   
#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

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

bool DeltaWin::ChangeScreenResolution( Resolution res ) 
{
   return ChangeScreenResolution( res.width, res.height, res.bitDepth, res.refresh );
}


Resolution DeltaWin::GetCurrentResolution( void )
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)   

   HDC hdc = GetDC( GetDesktopWindow() );
   
   Resolution r  = { GetDeviceCaps(hdc, HORZRES),
                     GetDeviceCaps(hdc, VERTRES),
                     GetDeviceCaps(hdc, BITSPIXEL),
                     GetDeviceCaps(hdc, VREFRESH) };
   return r;

#else

   Display* dpy = XOpenDisplay(NULL);
   int screenNum = DefaultScreen(dpy);

   int dotclock;
   XF86VidModeModeLine modeline;
   XF86VidModeGetModeLine( dpy, screenNum, &dotclock, &modeline);

   int thorz = static_cast<int>(modeline.hdisplay);
   int tvert = static_cast<int>(modeline.vdisplay);
   int tfreq = CalcRefreshRate( modeline.htotal, modeline.vtotal, dotclock );
   int tdepth = XDefaultDepth( dpy, screenNum );

   Resolution r = { thorz, tvert, tdepth, tfreq };
   return r;
   
#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)   
}



int DeltaWin::IsValidResolution( ResolutionVec rv, int width, int height, int refreshRate, int colorDepth )
{
   for( unsigned int i = 0; i < rv.size(); i++ )
   {
      if( width && height )
      {
         if( refreshRate && colorDepth )
         {
            if( rv[i].width == width && rv[i].height == height && rv[i].refresh == refreshRate && rv[i].bitDepth == colorDepth )
               return i;
         }
         else if( refreshRate )
         {
            if( rv[i].width == width && rv[i].height == height && rv[i].refresh == refreshRate )
               return i;
         }
         else if( colorDepth )
         {
            if( rv[i].width == width && rv[i].height == height && rv[i].bitDepth == colorDepth )
               return i;
         }
         else
         {
            if( rv[i].width == width && rv[i].height == height )
               return i;
         }
      }
      else if( refreshRate && colorDepth )
      {
         if( rv[i].refresh == refreshRate && rv[i].bitDepth == colorDepth )
            return i;
      }
      else if( refreshRate )
      {
         if(  rv[i].refresh == refreshRate )
            return i;
      }
      else if( colorDepth )
      {
         if( rv[i].bitDepth == colorDepth )
            return i;
      }
   }
   
   return -1;
}

//Approximates refresh rate (X11 only)
#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
int DeltaWin::CalcRefreshRate( int horzTotal, int vertTotal, int dotclock )
{
   return static_cast<int>( 0.5f + ( ( 1000.0f * dotclock ) / ( horzTotal * vertTotal ) ) );
}
#endif
