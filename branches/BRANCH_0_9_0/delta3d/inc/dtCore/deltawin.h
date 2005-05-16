/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#include "dtCore/deltarendersurface.h"
#include "dtCore/keyboard.h"
#include "dtCore/mouse.h"
#include "dtCore/refptr.h"

namespace Producer
{
   class KeyboardMouse;
}

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

      DeltaWin(   std::string name = "window", int x = 100, int y = 100, int width = 640, 
                  int height = 480, bool cursor = true, bool fullScreen = false );

      DeltaWin(   std::string name, DeltaRenderSurface* rs, Producer::InputArea* ia = 0 );

      virtual ~DeltaWin();

      ///Calculate the screen pixel coords ([0,w],[0,h]) given the window coords (x,y) ([-1,1],[-1,1])
      bool CalcPixelCoords( float x, float y, float &pixel_x, float &pixel_y );

      ///Calculate the window coords ([-1,1],[-1,1]), given the screen pixel coords (x,y) ([0,w],[0,h])
      bool CalcWindowCoords( float pixel_x, float pixel_y, float &x, float &y );

      ///Draw the cursor or not
      void ShowCursor( bool show = true );
      
      ///Is the cursor being drawn or not?
      bool GetShowCursor() { return mShowCursor; }

      ///Set the full screen mode.  If enabled, this will resize the window to fill the display and remove the window border.
      void SetFullScreenMode( bool enable = true );
  
      ///Is the window currently in fullscreen mode?
      bool GetFullScreenMode() { return mRenderSurface->isFullScreen(); }
      
      ///The the title on the DeltaWin border
      void SetWindowTitle( const std::string& title );

      const std::string& GetWindowTitle() const;
      
      ///Set the size and position of the DeltaWin
      void SetPosition( int x, int y, int width, int height );

      ///Get the size and position of the DeltaWin
      void GetPosition( int *x, int *y, int *width, int *height );

      ///Get a handle to the underlying DeltaRenderSurface
      DeltaRenderSurface* GetRenderSurface() { return mRenderSurface; }
      
      ///Get a handle to the Keyboard associated with the DeltaWin
      Keyboard* GetKeyboard() { return mKeyboard.get(); }

      ///Get a handle to the Mouse associated with the DeltaWin
      Mouse* GetMouse() { return mMouse.get(); }

      //TODO: put these into a dtCore::Display class
      static ResolutionVec GetResolutions();              
      static Resolution GetCurrentResolution();
      static bool ChangeScreenResolution( int width, int height, int colorDepth, int refreshRate );
      static bool ChangeScreenResolution( Resolution res );
      static int IsValidResolution( ResolutionVec rv, int width = 0, int height = 0, int refreshRate = 0, int colorDepth = 0 );

   private:

      static int CalcRefreshRate( int width, int height, int dotclock );
      
      DeltaRenderSurface* mRenderSurface; //changed from straight-up RS
      Producer::KeyboardMouse* mKeyboardMouse;

      RefPtr<Keyboard> mKeyboard;
      RefPtr<Mouse> mMouse;

      bool mShowCursor;
 
   };
  
};

#endif // DELTA_DELTA_WIN
