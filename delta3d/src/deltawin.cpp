// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include "Producer/KeyboardMouse"
#include "deltawin.h"
#include "notify.h"

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


DeltaWin::DeltaWin(string name, int x, int y, int width, int height, bool cursor, bool fullScreen) ://, bool callback) :
Base(name),
mShowCursor(true)
{
   RegisterInstance(this);

   mRenderSurface = new Producer::RenderSurface;
   
   mKeyboard = new Keyboard;

   mMouse = new Mouse;

   //if(callback)
   //{
   mKeyboardMouse = new Producer::KeyboardMouse(mRenderSurface);
   mKeyboardMouse->setCallback( new InputCallback(mKeyboard.get(), mMouse.get()) );
   mKeyboardMouse->startThread();
   //}

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

DeltaWin::DeltaWin(string name, Producer::RenderSurface* rs) ://, bool callback) :
Base(name),
mShowCursor(true),
mRenderSurface(rs)
{
   RegisterInstance(this);
   
   mKeyboard = new Keyboard;

   mMouse = new Mouse;


   //if(callback)
   //{
      mKeyboardMouse = new Producer::KeyboardMouse(mRenderSurface);

      mKeyboardMouse->setCallback( new InputCallback(mKeyboard.get(), mMouse.get()) );
      mKeyboardMouse->startThread();
   //}

   ShowCursor();
}


DeltaWin::DeltaWin(string name, Producer::InputArea* ia) ://, bool callback) :
Base(name),
mShowCursor(true),
mRenderSurface(ia->getRenderSurface(0))
{
   RegisterInstance(this);
   
   mKeyboard = new Keyboard;

   mMouse = new Mouse;


   //if(callback)
   //{
      mKeyboardMouse = new Producer::KeyboardMouse(ia);

      mKeyboardMouse->setCallback( new InputCallback(mKeyboard.get(), mMouse.get()) );
      mKeyboardMouse->startThread();
   //}
 

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
   mRenderSurface->positionPointer(x+1, y+1);

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
}

/*!
 * Calculate the screen coordinates given a window coordinate. Screem
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

   pixel_x = ( w/2 ) * (x + 1);
   pixel_y = ( h/2 ) * (y + 1);
   
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
      return false;
   }
   return true;
   
#else

   Display* dpy = XOpenDisplay(NULL);
   int screenNum = DefaultScreen(dpy);

   int dotClock;
   XF86VidModeModeLine modeline;
   XF86VidModeGetModeLine( dpy, screenNum, &dotClock, &modeline);

   int tempRefresh = CalcRefreshRate( modeline.htotal, modeline.vtotal, dotClock );

   //test if new value is same as current, if so don't do anything
   if( modeline.hdisplay == width && modeline.vdisplay == height && tempRefresh == refreshRate )
       return true;
  

   int numResolutions;
   XF86VidModeModeInfo** resolutions;
   XF86VidModeGetAllModeLines(dpy,
                              screenNum,
                              &numResolutions,
                              &resolutions);

   for(int i = 0; i < numResolutions; i++)
   {
      XF86VidModeModeInfo* tempRes = resolutions[i];
      
      tempRefresh = CalcRefreshRate( tempRes->htotal, tempRes->vtotal, tempRes->dotclock );

      //Notify(WARN,"Checking resolution: %dx%d @ %d, %d", tempRes->hdisplay, tempRes->vdisplay, colorDepth, tempRefresh );
      //Notify(WARN,"against:             %dx%d @ %d, %d\n", width, height, colorDepth, refreshRate );
      
      if( tempRes->hdisplay == width && tempRes->vdisplay == height && tempRefresh == refreshRate )
      {
         XF86VidModeSwitchToMode( dpy, screenNum, tempRes );
         XF86VidModeSetViewPort( dpy,screenNum, 0, 0 );
         XSync(dpy,false);

         return true;
      }
   }

   Notify(WARN,"Resolution could not be changed to %dx%d @ %d, %d", width, height, colorDepth, refreshRate );
   return false;
   
#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
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



//Approximates refresh rate (X11 only)
#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
int DeltaWin::CalcRefreshRate( int horzTotal, int vertTotal, int dotclock )
{
   return static_cast<int>( 0.5f + ( ( 1000.0f * dotclock ) / ( horzTotal * vertTotal ) ) );
}
#endif
