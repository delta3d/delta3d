// deltawin.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include <Producer/KeyboardMouse>

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

      RefPtr<Keyboard> mKeyboard;
      RefPtr<Mouse> mMouse;
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


DeltaWin::DeltaWin( string name, int x, int y, int width, int height, bool cursor, bool fullScreen ) :
   Base(name),
   mRenderSurface(0),
   mKeyboardMouse(0),
   mKeyboard(0),
   mMouse(0),
   mShowCursor(true)
{
   RegisterInstance(this);

   mRenderSurface = new Producer::RenderSurface; 
   
   mKeyboard = new Keyboard;
   mMouse = new Mouse;

   mKeyboardMouse = new Producer::KeyboardMouse( mRenderSurface );
   mKeyboardMouse->setCallback( new InputCallback( mKeyboard.get(), mMouse.get() ) );
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

DeltaWin::DeltaWin( string name, Producer::RenderSurface* rs, Producer::InputArea* ia ) :
Base(name),
mRenderSurface(rs),
mKeyboardMouse(0),
mKeyboard(0),
mMouse(0),
mShowCursor(true)
{
   RegisterInstance(this);
   
   mKeyboard = new Keyboard;
   mMouse = new Mouse;

   if(ia) // use the passed InputArea if not NULL
      mKeyboardMouse = new Producer::KeyboardMouse( ia );
   else // otherwise use the passed DeltaRenderSurface
      mKeyboardMouse = new Producer::KeyboardMouse( mRenderSurface );

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
void DeltaWin::SetPosition( int x, int y, int width, int height )
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

const std::string& DeltaWin::GetWindowTitle() const
{
   return mRenderSurface->getWindowName();
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
bool DeltaWin::CalcPixelCoords( float x, float y, float &pixel_x, float &pixel_y )
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

bool DeltaWin::CalcWindowCoords( float pixel_x, float pixel_y, float &x, float &y )
{
   int wx, wy;
   unsigned int w, h;
   GetRenderSurface()->getWindowRectangle( wx, wy, w, h );

   if (pixel_x < 0 || pixel_x > w ) return false;
   if (pixel_y < 0 || pixel_y > h ) return false;

   if( w != 0 && h != 0 )
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

bool DeltaWin::ChangeScreenResolution( Resolution res ) 
{
   return ChangeScreenResolution( res.width, res.height, res.bitDepth, res.refresh );
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

int DeltaWin::CalcRefreshRate( int horzTotal, int vertTotal, int dotclock )
{
   return static_cast<int>( 0.5f + ( ( 1000.0f * dotclock ) / ( horzTotal * vertTotal ) ) );
}
