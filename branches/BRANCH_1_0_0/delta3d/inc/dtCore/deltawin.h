#ifndef DELTA_DELTA_WIN
#define DELTA_DELTA_WIN

// deltawin.h: interface for the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include <Producer/KeyboardMouse>
#include <Producer/RenderSurface>

#include "dtCore/keyboard.h"
#include "dtCore/mouse.h"
#include <osg/ref_ptr>

#include "dtCore/base.h"

#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/xf86vmode.h>
#endif

namespace dtCore
{
   struct Resolution
   {
      int width;
      int height;
      int bitDepth;
      int refresh;
   };

   typedef std::vector<Resolution> ResolutionVec;
   
   class DT_EXPORT DeltaWin : public Base
   {
      DECLARE_MANAGEMENT_LAYER(DeltaWin)

   public:
      DeltaWin(std::string name="window", int x=100, int y=100, int width=640, int height=480, bool cursor=true, bool fullScreen=false);//, bool callback=true);
      DeltaWin(std::string name, Producer::RenderSurface* rs);//, bool callback=true);
      DeltaWin(std::string name, Producer::InputArea* ia);//, bool callback=true);
      virtual ~DeltaWin();

      ///Calculate the screen pixel coords ([0,w],[0,h]) given the window coords (x,y) ([-1,1],[-1,1])
      bool CalcPixelCoords( float x, float y, float &pixel_x, float &pixel_y);

      ///Calculate the window coords ([-1,1],[-1,1]), given the screen pixel coords (x,y) ([0,w],[0,h])
      bool CalcWindowCoords( float pixel_x, float pixel_y, float &x, float &y);
      
      ///Draw the cursor or not
      void ShowCursor( bool show = true);
      
      ///Is the cursor being drawn or not?
      bool GetShowCursor(void) {return mShowCursor;}

      /** Set the full screen mode.  If enabled, this will resize the window to fill
       *  the display and remove the window border.
       */
      void SetFullScreenMode( bool enable=true);// {mRenderSurface->fullScreen(enable);}
  
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

      //Note: make these static
      static ResolutionVec GetResolutions( void );
 
      //void  SetChangeScreenResolutionFlag( int width, int height, int pixelDepth );
                              
      static Resolution GetCurrentResolution( void );
      static bool  ChangeScreenResolution( int width, int height, int colorDepth, int refreshRate );
      static bool  ChangeScreenResolution( Resolution res );

      static int   IsValidResolution( ResolutionVec rv, int width = 0, int height = 0, int refreshRate = 0, int colorDepth = 0 );

   private:

      #if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
      static int CalcRefreshRate( int width, int height, int dotclock );
      #endif
      
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
