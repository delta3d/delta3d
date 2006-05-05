// deltawin.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/deltawin.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtUtil/deprecationmgr.h>
#include <dtUtil/log.h>

#include <cassert>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(DeltaWin)

DeltaWin::InputCallback::InputCallback(Keyboard* keyboard, Mouse* mouse) : mKeyboard(keyboard), mMouse(mouse)
{
}

void DeltaWin::InputCallback::mouseScroll(Producer::KeyboardMouseCallback::ScrollingMotion sm)
{
   mMouse->MouseScroll(sm);
}

void DeltaWin::InputCallback::mouseMotion(float x, float y)
{
   mMouse->MouseMotion( x, y );
}

void DeltaWin::InputCallback::passiveMouseMotion(float x, float y)
{
   mMouse->PassiveMouseMotion( x, y );
}

void DeltaWin::InputCallback::buttonPress(float x, float y, unsigned int button)
{
   // an unknown button number defaults to LeftButton.
   Mouse::MouseButton mb(Mouse::LeftButton);

   // prepare the value from Producer to be what Delta3D is expecting.
   if( button > 0 )
   {
      button -= 1;
   }

   switch( button )
   {
   case Mouse::RightButton:
      {
         mb = Mouse::RightButton;
      } break;

   case Mouse::MiddleButton:
      {
         mb = Mouse::MiddleButton;
      } break;
   }
   mMouse->ButtonDown(x, y, mb);
}

void DeltaWin::InputCallback::doubleButtonPress(float x, float y, unsigned int button)
{
   // an unknown button number defaults to LeftButton.
   Mouse::MouseButton mb(Mouse::LeftButton);

   // prepare the value from Producer to be what Delta3D is expecting.
   if( button > 0 )
   {
      button -= 1;
   }

   switch( button )
   {
   case Mouse::RightButton:
      {
         mb = Mouse::RightButton;
      } break;

   case Mouse::MiddleButton:
      {
         mb = Mouse::MiddleButton;
      } break;
   }

   mMouse->DoubleButtonDown(x, y, mb);
}

void DeltaWin::InputCallback::buttonRelease(float x, float y, unsigned int button)
{
   // an unknown button number defaults to LeftButton.
   Mouse::MouseButton mb(Mouse::LeftButton);

   // prepare the value from Producer to be what Delta3D is expecting.
   if( button > 0 )
   {
      button -= 1;
   }

   switch( button )
   {
   case Mouse::RightButton:
      {
         mb = Mouse::RightButton;
      } break;

   case Mouse::MiddleButton:
      {
         mb = Mouse::MiddleButton;
      } break;
   }

   mMouse->ButtonUp(x, y, mb);
}

void DeltaWin::InputCallback::keyPress(Producer::KeyCharacter kc)
{
   mKeyboard->KeyDown(kc);
}

void DeltaWin::InputCallback::keyRelease(Producer::KeyCharacter kc)
{
   mKeyboard->KeyUp(kc);
}

void DeltaWin::InputCallback::specialKeyPress(Producer::KeyCharacter kc)
{
   mKeyboard->KeyDown(kc);
}

void DeltaWin::InputCallback::specialKeyRelease(Producer::KeyCharacter kc)
{
   mKeyboard->KeyUp(kc);
}

void DeltaWin::InputCallback::SetKeyboard(Keyboard* kb)
{
   mKeyboard = kb;
}

void DeltaWin::InputCallback::SetMouse(Mouse* m)
{
   mMouse = m;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


DeltaWin::DeltaWin(  const std::string& name, 
                     int x, int y, 
                     int width, int height, 
                     bool cursor, bool fullScreen ) :
   Base(name),
   mRenderSurface( new Producer::RenderSurface ),
   mKeyboardMouse( new Producer::KeyboardMouse( mRenderSurface.get() ) ),
   mKeyboard( new Keyboard ),
   mMouse( new Mouse(mKeyboardMouse.get(),"mouse") ),
   mShowCursor(true),
   mInputCallback(new InputCallback( mKeyboard.get(), mMouse.get() ))
{
   RegisterInstance(this);

   mKeyboardMouse->setCallback( mInputCallback.get() );

   if(!fullScreen)
   {
      SetPosition( x, y, width, height );
   }

   SetWindowTitle(name);
   ShowCursor(cursor);
}

DeltaWin::DeltaWin(  const std::string& name, 
                     Producer::RenderSurface* rs, 
                     Producer::InputArea* ia ) :
   Base(name), 
   mRenderSurface(rs),
   mKeyboardMouse(0),
   mKeyboard( new Keyboard ),
   mMouse(0),
   mShowCursor(true),
   mInputCallback(0)
{
   RegisterInstance(this);

   // \TODO: Throw exception if NULL RenderSurface is passed
   assert( mRenderSurface != 0 );
   
   if(ia) // use the passed InputArea if not NULL
   {
      mKeyboardMouse = new Producer::KeyboardMouse(ia);
   }
   else // otherwise use the passed RenderSurface
   {
      mKeyboardMouse = new Producer::KeyboardMouse(mRenderSurface.get());
   }

   mMouse = new Mouse( mKeyboardMouse.get(), "mouse" );

   mInputCallback = new InputCallback( mKeyboard.get(), mMouse.get() );
   mKeyboardMouse->setCallback( new InputCallback( mKeyboard.get(), mMouse.get() ) );

   SetWindowTitle(name);
   ShowCursor();
}

DeltaWin::DeltaWin(  const std::string& name, 
                     dtCore::Keyboard* keyboard, 
                     dtCore::Mouse* mouse ) :
   Base(name),
   mRenderSurface( new Producer::RenderSurface ),
   mKeyboardMouse( new Producer::KeyboardMouse(mRenderSurface.get()) ),
   mKeyboard(keyboard),
   mMouse(mouse),
   mShowCursor(true)
{
   // \TODO: Throw exception if NULL Keyboard or Mouse is passed
   assert( mKeyboard.valid() );
   assert( mMouse.valid() );

   RegisterInstance(this);

   mKeyboardMouse->setCallback( new InputCallback( mKeyboard.get(), mMouse.get() ) );

   SetWindowTitle(name);
   ShowCursor();
}

DeltaWin::~DeltaWin()
{
   mKeyboardMouse->cancel();
   mKeyboardMouse = 0;

   KillGLWindow();
   mRenderSurface = 0;

   DeregisterInstance(this);
}

///Is the window currently in fullscreen mode?
bool DeltaWin::GetFullScreenMode() const
{ 
   return mRenderSurface->isFullScreen();
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
   DEPRECATE(  "void GetPosition( int *x, int *y,int *width, int *height )",
               "void GetPosition( int& x, int& y, int& width, int& height )")
   GetPosition( *x, *y, *width, *height );
}

void DeltaWin::GetPosition( int& x, int& y, int& width, int& height )
{
   unsigned int w, h;

   mRenderSurface->getWindowRectangle( x, y, w, h );
   width = w;
   height = h;
}

void DeltaWin::SetKeyboard( Keyboard* keyboard )
{
   assert( keyboard != 0 );
   mKeyboard = keyboard;
   mInputCallback->SetKeyboard( mKeyboard.get() );
}

void DeltaWin::SetMouse( Mouse* mouse )
{
   assert( mouse != 0 );
   mMouse = mouse;
   mInputCallback->SetMouse( mMouse.get() );
}

const std::string& DeltaWin::GetWindowTitle() const
{
   return mRenderSurface->getWindowName();
}

void DeltaWin::Update()
{
   if( mKeyboardMouse.valid() && !mKeyboardMouse->isRunning() )
   {
      mKeyboardMouse->update( *mKeyboardMouse->getCallback() );
   }
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
      LOG_DEBUG("Window size of 0");
      return false;
   }
}

bool DeltaWin::ChangeScreenResolution( Resolution res ) 
{
   return ChangeScreenResolution( res.width, res.height, res.bitDepth, res.refresh );
}

int DeltaWin::IsValidResolution( const ResolutionVec &rv, int width, int height, int refreshRate, int colorDepth )
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
