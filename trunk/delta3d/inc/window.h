#ifndef DELTA_WINDOW
#define DELTA_WINDOW

// window.h: interface for the Window class.
//
//////////////////////////////////////////////////////////////////////

#include <Producer/KeyboardMouse>
#include <Producer/RenderSurface>

#include "keyboard.h"
#include "mouse.h"
#include <osg/ref_ptr>

#include "base.h"



namespace dtCore
{
   
   
   class DT_EXPORT Window : public Base
   {
      DECLARE_MANAGEMENT_LAYER(Window)

   public:
      Window(std::string name="window", int x=100, int y=100, int width=640, int height=480);
      Window(std::string name, Producer::RenderSurface* rs);
      Window(std::string name, Producer::InputArea* ia);
      virtual ~Window();

      ///Calculate the screen pixel coords given the window (x,y)
	   bool CalcPixelCoords( float x, float y, float &pixel_x, float &pixel_y);


      ///Draw the cursor or not
      void ShowCursor( bool show = true);

      ///Is the cursor being drawn or not?
      bool GetShowCursor(void) {return mShowCursor;}

      /** Set the full screen mode.  If enabled, this will resize the window to fill
       *  the display and remove the window border.
       */
      void SetFullScreenMode( bool enable=true) {mRenderSurface->fullScreen(enable);}

      ///Is the window currently in fullscreen mode?
      bool GetFullScreenMode(void) {return mRenderSurface->isFullScreen();}
      
      ///The the title on the Window border
	   void SetWindowTitle( const char *title );
      const std::string GetWindowTitle(void) const;
      
      ///Set the size and position of the Window
	   void SetPosition( int x, int y, int width, int height );

      ///Get the size and position of the Window
      void GetPosition( int *x, int *y, int *width, int *height );

      ///Get a handle to the underlying Producer RenderSurface
      Producer::RenderSurface *GetRenderSurface(void) {return mRenderSurface;}
      
      ///Get a handle to the Keyboard associated with the Window
      Keyboard *GetKeyboard() {return mKeyboard.get();}

      ///Get a handle to the Mouse associated with the Window
      Mouse *GetMouse() {return mMouse.get();}

      #ifdef _WIN32
      ///Changes system screen resolution
      bool  ChangeScreenResolution (int width, int height, int bitsPerPixel);
      #endif // _WIN32

   private:

      Producer::RenderSurface *mRenderSurface;
      Producer::KeyboardMouse *mKeyboardMouse;
      osg::ref_ptr<Keyboard> mKeyboard;
      osg::ref_ptr<Mouse> mMouse;
      bool mShowCursor;
   };

};



#endif // DELTA_WINDOW
