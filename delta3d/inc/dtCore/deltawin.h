/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_DELTA_WIN
#define DELTA_DELTA_WIN

// deltawin.h: interface for the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/base.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/refptr.h>
#include <Producer/KeyboardMouse>   // for InputCallback's base class

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Producer
{
   class InputArea;
   class KeyboardMouse;
   class RenderSurface;
}
/// @endcond

namespace dtCore
{
   /**
   * DeltaWin: The base window class for delta3D
   */
   class DT_CORE_EXPORT DeltaWin : public Base
   {
      DECLARE_MANAGEMENT_LAYER(DeltaWin)

   public:

      /** 
       * Constructor
       *
       * @param name the name of the class as well as the window title
       * @param x the location of the window in pixels
       * @param y the location of the window in pixels
       * @param width the width of the window in pixels
       * @param height the height of the window in pixels
       * @param cursor true if you wish to use the default cursor, false if not
       * @param fullScreen true if this window should be displayed fullscreen
       */
      DeltaWin( const std::string& name = "window", int x = 100, int y = 100, int width = 640, 
                int height = 480, bool cursor = true, bool fullScreen = false );

      /** 
      * Constructor
      *
      * @param the name of the class as well as the window title
      * @param rs if you have created your own Producer::RenderSurface and wish to use it
      * with this instance of DeltaWin, pass it in here.
      * @param ia if you have your own Producer::InputArea, pass it here. If 0 is passed, then
      * it will use the RenderSurface as the input area.
      */
      DeltaWin( const std::string& name, Producer::RenderSurface* rs, Producer::InputArea* ia = 0 );

      /** 
      * Constructor
      *
      * @param the name of the class as well as the window title
      * @param keyboard a custom keyboard
      * @param mouse a custom mouse
      */
      DeltaWin( const std::string& name, Keyboard* keyboard, Mouse* mouse );

      virtual ~DeltaWin();

      ///Calculate the screen pixel coords ([0,w],[0,h]) given the window coords (x,y) ([-1,1],[-1,1])
      bool CalcPixelCoords( float x, float y, float &pixel_x, float &pixel_y );

      ///Calculate the window coords ([-1,1],[-1,1]), given the screen pixel coords (x,y) ([0,w],[0,h])
      bool CalcWindowCoords( float pixel_x, float pixel_y, float &x, float &y );

      ///Draw the cursor or not
      void ShowCursor( bool show = true );
      
      ///Is the cursor being drawn or not?
      bool GetShowCursor() const { return mShowCursor; }

      ///Set the full screen mode.  If enabled, this will resize the window to fill the display and remove the window border.
      void SetFullScreenMode( bool enable = true );
  
      ///Is the window currently in fullscreen mode?
      bool GetFullScreenMode() const;
      
      void KillGLWindow();
      
      void Update();

      ///The the title on the DeltaWin border
      void SetWindowTitle( const std::string& title );

      const std::string& GetWindowTitle() const;
      
      ///Set the size and position of the DeltaWin
      void SetPosition( int x, int y, int width, int height );

      ///Get the size and position of the DeltaWin
      void GetPosition( int *x, int *y, int *width, int *height );
      void GetPosition( int& x, int& y, int& width, int& height );

      ///Get a handle to the underlying RenderSurface
      Producer::RenderSurface* GetRenderSurface() { return mRenderSurface; }
      const Producer::RenderSurface* GetRenderSurface() const { return mRenderSurface; }
      
      ///Get a handle to the Keyboard associated with the DeltaWin
      Keyboard* GetKeyboard() { return mKeyboard.get(); }
      const Keyboard* GetKeyboard() const { return mKeyboard.get(); }

      void SetKeyboard( Keyboard* keyboard );
      void SetMouse( Mouse* mouse );

      ///Get a handle to the Mouse associated with the DeltaWin
      Mouse* GetMouse() { return mMouse.get(); }
      const Mouse* GetMouse() const { return mMouse.get(); }

      struct Resolution
      {
         int width;  ///<In pixels
         int height; ///<In pixels
         int bitDepth; ///<number of color bits
         int refresh; ///<refresh rate in Hz
      };

      typedef std::vector<Resolution> ResolutionVec;

      //TODO: put these into a dtCore::Display class
      static ResolutionVec GetResolutions();              
      static Resolution GetCurrentResolution();
      static bool ChangeScreenResolution( int width, int height, int colorDepth, int refreshRate );
      static bool ChangeScreenResolution( Resolution res );
      static int IsValidResolution( const ResolutionVec &rv, int width = 0, int height = 0, int refreshRate = 0, int colorDepth = 0 );

   private:

      static int CalcRefreshRate( int width, int height, int dotclock );
      
      Producer::RenderSurface* mRenderSurface; //changed from straight-up RS
      Producer::KeyboardMouse* mKeyboardMouse;

      RefPtr<Keyboard> mKeyboard;
      RefPtr<Mouse> mMouse;

      bool mShowCursor;
 
      class InputCallback : public Producer::KeyboardMouseCallback
      {
      public:
         InputCallback(Keyboard* keyboard, Mouse* mouse);

         // virtual methods
         void mouseScroll(Producer::KeyboardMouseCallback::ScrollingMotion sm);
         void mouseMotion(float x, float y);
         void passiveMouseMotion(float x, float y);
         void buttonPress(float x, float y, unsigned int button);
         void doubleButtonPress(float x, float y, unsigned int button);
         void buttonRelease(float x, float y, unsigned int button);
         void keyPress(Producer::KeyCharacter kc);
         void keyRelease(Producer::KeyCharacter kc);
         void specialKeyPress(Producer::KeyCharacter kc);
         void specialKeyRelease(Producer::KeyCharacter kc);

         void SetKeyboard(Keyboard* kb);
         void SetMouse(Mouse* m);

      private:
         RefPtr<Keyboard> mKeyboard;
         RefPtr<Mouse> mMouse;
      };

      RefPtr<InputCallback> mInputCallback;
   };
};

#endif // DELTA_DELTA_WIN
