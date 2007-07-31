// deltawin.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/deltawin.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtUtil/deprecationmgr.h>
#include <dtUtil/log.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/inputcallback.h>
#include <dtUtil/exception.h>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(DeltaWin)

///////////////////////////////////////////////////
// --- InputCallback's implementation --- /////////
///////////////////////////////////////////////////
InputCallback::InputCallback(Keyboard* keyboard, Mouse* mouse) : mKeyboard(keyboard), mMouse(mouse)
{
}

///////////////////////////////////////////////////
void InputCallback::mouseScroll(Producer::KeyboardMouseCallback::ScrollingMotion sm)
{
   mMouse->MouseScroll(sm);
}

///////////////////////////////////////////////////
void InputCallback::mouseMotion(float x, float y)
{
   mMouse->MouseMotion( x, y );
}

///////////////////////////////////////////////////
void InputCallback::passiveMouseMotion(float x, float y)
{
   mMouse->PassiveMouseMotion( x, y );
}

///////////////////////////////////////////////////
void InputCallback::buttonPress(float x, float y, unsigned int button)
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

///////////////////////////////////////////////////
void InputCallback::doubleButtonPress(float x, float y, unsigned int button)
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

///////////////////////////////////////////////////
void InputCallback::buttonRelease(float x, float y, unsigned int button)
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

///////////////////////////////////////////////////
void InputCallback::keyPress(Producer::KeyCharacter kc)
{
   mKeyboard->KeyDown(kc);
}

///////////////////////////////////////////////////
void InputCallback::keyRelease(Producer::KeyCharacter kc)
{
   mKeyboard->KeyUp(kc);
}

///////////////////////////////////////////////////
void InputCallback::specialKeyPress(Producer::KeyCharacter kc)
{
   mKeyboard->KeyDown(kc);
}

///////////////////////////////////////////////////
void InputCallback::specialKeyRelease(Producer::KeyCharacter kc)
{
   mKeyboard->KeyUp(kc);
}

///////////////////////////////////////////////////
void InputCallback::SetKeyboard(Keyboard* kb)
{
   mKeyboard = kb;
}

///////////////////////////////////////////////////
void InputCallback::SetMouse(Mouse* m)
{
   mMouse = m;
}
///////////////////////////////////////////////////
// --- end of InputCallback's implementation --- //
///////////////////////////////////////////////////

/////////////////////////////////////////////////
// --- DeltaWin's implementation --- ////////////
/////////////////////////////////////////////////
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

/////////////////////////////////////////////////
DeltaWin::DeltaWin(  const std::string& name, 
                     Producer::RenderSurface* rs, 
                     Producer::InputArea* ia ) :
   Base(name), 
   mRenderSurface(rs),
   mKeyboardMouse(NULL),
   mMouse(NULL),
   mShowCursor(true),
   mInputCallback(NULL)
{
   if( mRenderSurface == NULL )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied Producer::RenderSurface is NULL", __FILE__, __LINE__);
   }

   RegisterInstance(this);

   mKeyboard = new Keyboard;

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

/////////////////////////////////////////////////
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
   if( !mKeyboard.valid() || !mMouse.valid() )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Keyboard or dtCore::Mouse is invalid", __FILE__, __LINE__);
   }

   RegisterInstance(this);

   mKeyboardMouse->setCallback( new InputCallback( mKeyboard.get(), mMouse.get() ) );

   SetWindowTitle(name);
   ShowCursor();
}

/////////////////////////////////////////////////
DeltaWin::~DeltaWin()
{
   mKeyboardMouse->cancel();
   mKeyboardMouse = 0;

   KillGLWindow();
   mRenderSurface->cancel();
   mRenderSurface = 0;

   DeregisterInstance(this);
}

/////////////////////////////////////////////////
InputCallback* DeltaWin::GetInputCallback() 
{ 
   return mInputCallback.get(); 
}

/////////////////////////////////////////////////
const InputCallback* DeltaWin::GetInputCallback() const 
{ 
   return mInputCallback.get(); 
}

///////////////////////////////////////////////////
bool DeltaWin::GetFullScreenMode() const
{ 
   return mRenderSurface->isFullScreen();
}

///////////////////////////////////////////////////
void DeltaWin::SetPosition( int x, int y, int width, int height )
{
   mRenderSurface->setWindowRectangle(x, y, width, height);
}

///////////////////////////////////////////////////
void DeltaWin::SetPosition( const DeltaWin::PositionSize& positionSize )
{
   SetPosition(positionSize.mX, positionSize.mY, positionSize.mWidth, positionSize.mHeight);
}

///////////////////////////////////////////////////
void DeltaWin::GetPosition( int *x, int *y,int *width, int *height )
{
   DEPRECATE(  "void GetPosition( int *x, int *y,int *width, int *height )",
               "void GetPosition( int& x, int& y, int& width, int& height )")
   GetPosition( *x, *y, *width, *height );
}

///////////////////////////////////////////////////
void DeltaWin::GetPosition( int& x, int& y, int& width, int& height )
{
   unsigned int w, h;

   mRenderSurface->getWindowRectangle( x, y, w, h );
   width = w;
   height = h;
}

///////////////////////////////////////////////////
DeltaWin::PositionSize DeltaWin::GetPosition()
{
   PositionSize positionSize;
   GetPosition(positionSize.mX, positionSize.mY, positionSize.mWidth, positionSize.mHeight);
   return positionSize;
}

///////////////////////////////////////////////////
void DeltaWin::SetRenderSurface( Producer::RenderSurface* renderSurface )
{
   if( renderSurface == 0 )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied Producer::RenderSurface is invalid", __FILE__, __LINE__);
   }
   mRenderSurface = renderSurface;
}


///////////////////////////////////////////////////
void DeltaWin::SetKeyboard( Keyboard* keyboard )
{
   if( keyboard == 0 )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Keyboard is invalid", __FILE__, __LINE__);
   }
   mKeyboard = keyboard;
   mInputCallback->SetKeyboard( mKeyboard.get() );
}
//////////////////////////////////////////////////
void DeltaWin::SetKeyRepeat(bool on)
{
   mKeyboardMouse->setAutoRepeatMode(on);
}

/////////////////////////////////////////////////
bool DeltaWin::GetKeyRepeat() const
{
   return mKeyboardMouse->getAutoRepeatMode();   
}

/////////////////////////////////////////////////
void DeltaWin::SetMouse( Mouse* mouse )
{
   if( mouse == 0 )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Mouse is invalid", __FILE__, __LINE__);
   }
   mMouse = mouse;
   mInputCallback->SetMouse( mMouse.get() );
}

/////////////////////////////////////////////////
const std::string& DeltaWin::GetWindowTitle() const
{
   return mRenderSurface->getWindowName();
}

/////////////////////////////////////////////////
void DeltaWin::Update()
{
   if( mKeyboardMouse.valid() && !mKeyboardMouse->isRunning() )
   {
      mKeyboardMouse->update( *mKeyboardMouse->getCallback() );
   }
}

/////////////////////////////////////////////////
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

/////////////////////////////////////////////////
bool DeltaWin::CalcPixelCoords( osg::Vec2 window_xy, osg::Vec2& pixel_xy )
{
   return CalcPixelCoords( window_xy.x(), window_xy.y(), pixel_xy.x(), pixel_xy.y() );
}

/////////////////////////////////////////////////
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

/////////////////////////////////////////////////
bool DeltaWin::CalcWindowCoords( osg::Vec2 pixel_xy, osg::Vec2& window_xy )
{
   return CalcWindowCoords( pixel_xy.x(), pixel_xy.y(), window_xy.x(), window_xy.y() );
}

/////////////////////////////////////////////////
bool DeltaWin::ChangeScreenResolution( Resolution res ) 
{
   return ChangeScreenResolution( res.width, res.height, res.bitDepth, res.refresh );
}

/////////////////////////////////////////////////
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

/////////////////////////////////////////////////
bool DeltaWin::IsValidResolution(const DeltaWin::Resolution& candidate)
{
   ResolutionVec vec = DeltaWin::GetResolutions();

   DeltaWin::ResolutionVec::iterator iter = vec.begin();
   DeltaWin::ResolutionVec::iterator enditer = vec.end();
   while(iter != enditer)
   {
      if( (candidate.width==iter->width) &&
          (candidate.height==iter->height) &&
          (candidate.bitDepth==iter->bitDepth) &&
          (candidate.refresh==iter->refresh) )
      {
         return true;
      }

      ++iter;
   }

   return false;
}

/////////////////////////////////////////////////
int DeltaWin::CalcRefreshRate( int horzTotal, int vertTotal, int dotclock )
{
   return static_cast<int>( 0.5f + ( ( 1000.0f * dotclock ) / ( horzTotal * vertTotal ) ) );
}
