#ifndef DELTA_DELTA_WIN
#define DELTA_DELTA_WIN

// deltawin.h: interface for the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include <Producer/KeyboardMouse>
#include <Producer/RenderSurface>

#include "keyboard.h"
#include "mouse.h"
#include <osg/ref_ptr>

#include "base.h"

#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include <X11/Xlib.h>
#endif

namespace dtCore
{
   
   
   class DT_EXPORT DeltaWin : public Base
   {
      DECLARE_MANAGEMENT_LAYER(DeltaWin)

   public:
      DeltaWin(std::string name="window", int x=100, int y=100, int width=640, int height=480);
      DeltaWin(std::string name, Producer::RenderSurface* rs);
      DeltaWin(std::string name, Producer::InputArea* ia);
      virtual ~DeltaWin();

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
      
      ///The the title on the DeltaWin border
	   void SetWindowTitle( const char *title );
      const std::string GetWindowTitle(void) const;
      
      ///Set the size and position of the DeltaWin
	   void SetPosition( int x, int y, int width, int height );

      ///Get the size and position of the DeltaWin
      void GetPosition( int *x, int *y, int *width, int *height );

      ///Get a handle to the underlying Producer RenderSurface
      Producer::RenderSurface *GetRenderSurface(void) {return mRenderSurface;}
      
      ///Get a handle to the Keyboard associated with the DeltaWin
      Keyboard *GetKeyboard() {return mKeyboard.get();}

      ///Get a handle to the Mouse associated with the DeltaWin
      Mouse *GetMouse() {return mMouse.get();}

      ///Changes system screen resolution
      bool  ChangeScreenResolution (int width, int height, int bitsPerPixel);

   private:

      Producer::RenderSurface *mRenderSurface;
      Producer::KeyboardMouse *mKeyboardMouse;
      osg::ref_ptr<Keyboard> mKeyboard;
      osg::ref_ptr<Mouse> mMouse;
      bool mShowCursor;
   };

   #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
   typedef DeltaWin Window;
   #endif
   
};



#endif // DELTA_DELTA_WIN
