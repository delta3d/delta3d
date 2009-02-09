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

#include <dtCore/base.h>
#include <dtCore/refptr.h>
#include <dtUtil/deprecationmgr.h>
#include <vector>                   // for member

#include <osg/observer_ptr>
#include <osg/Vec2>
#include <osg/GraphicsContext>

//////////////////////////////////////////////////////////////////////

///@cond
namespace osgViewer
{
    class GraphicsWindow;
}
///@endcond


namespace dtCore
{
   /**
   * \brief The rendering area that a dtCore::Camera will render to
   *
   * The DeltaWin class is used as a canvas for the dtCore::Cameras to render upon.
   * The DeltaWin can be positioned and sized using SetPosition(), set to be fullscreen using
   * SetFullScreenMode(), and the cursor can be hidden using ShowCursor().
   * A DeltaWin can be embedded inside an existing window by derived from, or use, 
   * an osgViewer::EmbeddedGraphicsWindow, then passing the EmbeddedGraphicsWindow 
   * to the DeltaWin constructor, note you need to pass it to the dtCore::Camera too.
   * See dtABC::Widget for more information.
   *
   * Typical usage:
   * \code
   * dtCore::RefPtr<dtCore::DeltaWin> win = new dtCore::DeltaWin("myWin");
   * win->SetPosition(100, 100, 640, 480);
   * win->SetWindowTitle("My example window");
   * \endcode
   *
   * \see dtCore::Camera::SetWindow()
   */
   class DT_CORE_EXPORT DeltaWin : public Base
   {
      DECLARE_MANAGEMENT_LAYER(DeltaWin)

   public:

      /** 
      * Traits - Set of window specific properties that control appearance and behavior.
      *
      * @param name the name of the class as well as the window title
      * @param x the location of the window in pixels
      * @param y the location of the window in pixels
      * @param width the width of the window in pixels
      * @param height the height of the window in pixels
      * @param cursor true if you wish to use the default cursor, false if not
      * @param fullScreen true if this window should be displayed fullscreen
      * @param inheritedWindowData the inheritedWindowData use to embedded the window in GUI window  
      */
      struct DeltaWinTraits
      {
         static const int DEFAULT_WIDTH  = 640;
         static const int DEFAULT_HEIGHT = 480;

         DeltaWinTraits()
            : name("defaultWindow")
            , x(0)
            , y(0)
            , width(DEFAULT_WIDTH)
            , height(DEFAULT_HEIGHT)
            , showCursor(true)
            , fullScreen(false)
            , inheritedWindowData(NULL)
            , contextToShare(NULL)
            , hostName("")
            , displayNum(0)
            , screenNum(0)
         {
         }

         std::string name;
         int x;
         int y;
         int width;
         int height;
         bool showCursor;
         bool fullScreen;
         osg::Referenced* inheritedWindowData;
         osg::GraphicsContext* contextToShare;
         std::string hostName;
         int displayNum;
         int screenNum;

      };

      /** 
       * Constructor that uses custom traits
       *     
       * @param windowTraits : the properties used to create the window.
       */
      DeltaWin(const DeltaWinTraits& windowTraits = DeltaWinTraits());

      /// DEPRECATED 01/14/09 in favor of dtCore::DeltaWin(const DeltaWinTraits& windowTraits)
      DEPRECATE_FUNC DeltaWin(const std::string& name, 
               int x = 0, int y = 0, 
               int width = 640, int height = 480, 
               bool cursor = true, bool fullScreen = false,
               osg::Referenced* inheritedWindowData = NULL);

      /** 
      * Constructor
      *
      * @param name : the name of the class as well as the window title
      * @param gw : the GraphicsWindow use by this instance  
      */
      DeltaWin(const std::string& name, osgViewer::GraphicsWindow& gw);
      
   protected:

      virtual ~DeltaWin();

   public:

      /*!
       * Calculate the screen pixel coordinates given a normalized window coordinate. Screen
       * pixel coordinate (0,0) is located in the lower left of the display, which equates to
       * the normalized window coordinates of (-1,-1).
       *
       * @param winX : normalized window x coordinate [-1, 1]
       * @param winY : normalized window y coordinate [-1, 1]
       * @param &pixelX : The screen X pixel equivalent (out) [0..window width]
       * @param &pixelY : The screen Y pixel equivalent (out) [0..window height]
       *
       * @return bool  : Returns true if the (x,y) is a valid window coordinate
       */
      bool CalcPixelCoords(float winX, float winY, float& pixelX, float& pixelY) const;
      bool CalcPixelCoords(const osg::Vec2& window_xy, osg::Vec2& pixel_xy) const;

      /** Calculate the normalized window coords given the screen pixel coords.  Pixel
       *  coordinate (0,0) is the lower left of the display which equates to the
       *  normalized window coordinate of (-1.0, -1.f).
       * @param pixel_x : The screen X pixel [0..window width]
       * @param pixel y : The screen Y pixel [0..window height]
       * @param x : The normalized window X coordinate [-1.0..1.0]
       * @param y : The normalized window Y coordinate [-1.0..1.0]
       * @return bool : True if the screen pixel coords are valid
       */
      bool CalcWindowCoords(float pixel_x, float pixel_y, float& x, float& y) const;
      bool CalcWindowCoords(const osg::Vec2& pixel_xy, osg::Vec2& window_xy) const;

      ///Draw the cursor or not
      void ShowCursor(bool show = true);
      
      ///Is the cursor being drawn or not?
      bool GetShowCursor() const { return mShowCursor; }

      ///Set the full screen mode.  If enabled, this will resize the window to fill the display and remove the window border.
      void SetFullScreenMode(bool enable = true);
  
      ///Is the window currently in fullscreen mode?
      bool GetFullScreenMode() const { return mIsFullScreen; }
      
      void KillGLWindow();
      
      ///The the title on the DeltaWin border
      void SetWindowTitle(const std::string& title);

      const std::string GetWindowTitle() const;
      
      ///Set the size and position of the DeltaWin
      struct PositionSize
      {
         int mX;
         int mY;
         int mWidth;
         int mHeight;
      };

      /** 
       * Set the position and size of the DeltaWin in screen coordinates
       * @param x The left edge of the window in screen coordinates
       * @param y The bottom edge of the window in screen coordinates
       * @param width The width of the window
       * @param height The height of the window
       */
      void SetPosition(int x, int y, int width, int height);
      void SetPosition(const PositionSize& positionSize);

      ///Get the size and position of the DeltaWin
      void GetPosition(int& x, int& y, int& width, int& height);
      PositionSize GetPosition();

      ///Get a handle to the underlying GraphicsWindow
      osgViewer::GraphicsWindow* GetOsgViewerGraphicsWindow() { return mOsgViewerGraphicsWindow.get(); }
      const osgViewer::GraphicsWindow* GetOsgViewerGraphicsWindow() const { return mOsgViewerGraphicsWindow.get(); }      


      /// The data structure modeling monitor resolution
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
      static bool ChangeScreenResolution(int width, int height, int colorDepth, int refreshRate);
      static bool ChangeScreenResolution(Resolution res);

      static int IsValidResolution(const ResolutionVec &rv, int width = 0, int height = 0, int refreshRate = 0, int colorDepth = 0);

      /// Tests to see if the system supports the desired resolution.
      /// @param candidate the Resolution to be tested.
      /// @return 'true' when the Resolution is supported.
      bool IsValidResolution(const Resolution& candidate);
      
   private:

      int mLastWindowedWidth;
      int mLastWindowedHeight;

      void CreateDeltaWindow(const DeltaWinTraits& windowTraits);

      ///Convenient method to create a GraphicsWindow
      osg::ref_ptr<osgViewer::GraphicsWindow> CreateGraphicsWindow(osg::GraphicsContext::Traits& traits) const;

      static int CalcRefreshRate(int width, int height, int dotclock);

      osg::ref_ptr<osg::GraphicsContext::Traits> CreateTraits(const std::string& name = "defaulWindow", 
                                                              int x = 500, int y = 500, 
                                                              int width = 640, int height = 480, 
                                                              unsigned int screenNum = 0,
                                                              bool cursor = true, 
                                                              osg::Referenced * inheritedWindowData = NULL) const;

      ///Little utility used to convert the supplied parameters into a OSG Trait.
      osg::ref_ptr<osg::GraphicsContext::Traits> CreateOSGTraits(const DeltaWinTraits& deltaTraits) const;
      
      dtCore::RefPtr<osgViewer::GraphicsWindow> mOsgViewerGraphicsWindow;

      bool mIsFullScreen;
      bool mShowCursor;

      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      DeltaWin& operator=(const DeltaWin&);
      DeltaWin(const DeltaWin&);
   };
}


#endif // DELTA_DELTA_WIN
