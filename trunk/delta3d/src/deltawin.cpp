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
         mMouse->mouseMotion(x, y);
      }

      void passiveMouseMotion(float x, float y)
      {
         mMouse->passiveMouseMotion(x, y);
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


DeltaWin::DeltaWin(string name, int x, int y, int width, int height) :
Base(name),
mShowCursor(true)
{
   RegisterInstance(this);

   mRenderSurface = new Producer::RenderSurface;
   
   mKeyboard = new Keyboard;

   mMouse = new Mouse;

   mKeyboardMouse = new Producer::KeyboardMouse(mRenderSurface);

   mKeyboardMouse->setCallback( new InputCallback(mKeyboard.get(), mMouse.get()) );

   mKeyboardMouse->startThread();

   SetPosition(x, y, width, height);
   SetName( name );
   SetWindowTitle(name.c_str());
   ShowCursor();
}



DeltaWin::DeltaWin(string name, Producer::RenderSurface* rs) :
Base(name),
mShowCursor(true),
mRenderSurface(rs)
{
   RegisterInstance(this);
   
   mKeyboard = new Keyboard;

   mMouse = new Mouse;

   mKeyboardMouse = new Producer::KeyboardMouse(mRenderSurface);

   mKeyboardMouse->setCallback( new InputCallback(mKeyboard.get(), mMouse.get()) );

   mKeyboardMouse->startThread();

   ShowCursor();
}



DeltaWin::DeltaWin(string name, Producer::InputArea* ia) :
Base(name),
mShowCursor(true),
mRenderSurface(ia->getRenderSurface(0))
{
   RegisterInstance(this);
   
   mKeyboard = new Keyboard;

   mMouse = new Mouse;

   mKeyboardMouse = new Producer::KeyboardMouse(ia);

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
   Notify(DEBUG_INFO, "destroying DeltaWin, ref count:%d", this->referenceCount() );

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

void DeltaWin::SetWindowTitle(const char *title)
{
   mRenderSurface->setWindowName(title);

   //Producer doesn't dynamically re-title the window so we do it ourself here
   //This only works for win32
#ifdef WIN32
   HWND win = mRenderSurface->getWindow();
   SetWindowText(win, title); //from winuser.h
#endif
}

const std::string DeltaWin::GetWindowTitle() const
{
   return mRenderSurface->getWindowName();
}

void DeltaWin::ShowCursor(const bool show )
{
   mShowCursor = show;

   //gotta do a little cursor game to make this work

   //First, save the current position of the cursor
#ifdef WIN32
   POINT coords;
   GetCursorPos(&coords);
#endif 
   
   //Then move the cursor to be on our window'
   int x,y,w,h;
   GetPosition(&x, &y, &w, &h); //winuser.h
   mRenderSurface->positionPointer(x+1, y+1);

   //Tell Producer
   mRenderSurface->useCursor(mShowCursor);

#ifdef WIN32
   //Then move the cursor back to where it started from
   SetCursorPos(coords.x, coords.y);
#endif
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
   if (x<-1.0f) return false;
   if (x>1.0f) return false;
   
   if (y<-1.0f) return false;
   if (y>1.0f) return false;
   
   float rx = (x+1.0f)*0.5f;
   float ry = (y+1.0f)*0.5f;
   
   int wx, wy;
   unsigned int w, h;
   GetRenderSurface()->getWindowRectangle( wx, wy, w, h );
   
   pixel_x = (float)wx + ((float)w)* rx;
   pixel_y = (float)wy + ((float)h)* ry;
   
   return true;

}

#ifdef _WIN32

bool DeltaWin::ChangeScreenResolution (int width, int height, int bitsPerPixel)   // Change The Screen Resolution
{
   DEVMODE dmScreenSettings;                                                            // Device Mode
   ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));                                    // Make Sure Memory Is Cleared
   dmScreenSettings.dmSize                              = sizeof (DEVMODE);             // Size Of The Devmode Structure
   dmScreenSettings.dmPelsWidth         = width;                                        // Select Screen Width
   dmScreenSettings.dmPelsHeight                = height;                               // Select Screen Height
   dmScreenSettings.dmBitsPerPel                = bitsPerPixel;                         // Select Bits Per Pixel
   dmScreenSettings.dmFields                    = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

   if ( ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
   {
      return FALSE;                                                                     // Display Change Failed, Return False
   }
   return TRUE;                                                                         // Display Change Was Successful, Return True
}

#endif // _WIN32
