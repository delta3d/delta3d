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
//#include <dtCore/inputcallback.h>
#include <dtUtil/exception.h>
#include <dtCore/scene.h>  //due to include of camera.h
#include <dtCore/keyboardmousehandler.h> //due to include of scene.h
#include <dtCore/keyboard.h> //due to include of scene.h
#include <osgViewer/GraphicsWindow>

#include <cassert>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(DeltaWin)

/////////////////////////////////////////////////
// --- DeltaWin's implementation --- ////////////
/////////////////////////////////////////////////
DeltaWin::DeltaWin(  const std::string& name, 
                     int x, int y, 
                     int width, int height, 
                     bool cursor, bool fullScreen,
                     osg::Referenced * inheritedWindowData) :
   Base(name),
   mIsFullScreen(false),
   mShowCursor(true) // force set fullscreen
{
   RegisterInstance(this);
   
   CreateGraphicsWindow(name, x, y, width, height, 0, cursor, inheritedWindowData);
   
   if(!fullScreen)
   {
      SetPosition( x, y, width, height );
   }
   else
   {
      SetFullScreenMode(fullScreen);
   }
}


DeltaWin::DeltaWin(const std::string& name, Camera * camera) :
   Base(name),
   mIsFullScreen(false),
   mShowCursor(true)
{
   RegisterInstance(this);

   if( camera == NULL )
   {
      DeregisterInstance(this);
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied dtCore::Camera is NULL", __FILE__, __LINE__);
   }

   osg::GraphicsContext * gc = camera->GetOsgCamera()->getGraphicsContext();
   try
   {
      SetOsgViewerGraphicsWindow(dynamic_cast<osgViewer::GraphicsWindow*>(gc));
   }
   catch(dtUtil::Exception & ex)
   {
      DeregisterInstance(this);
      throw ex;
   }
   mCamera = camera;
   
   SetWindowTitle(name);
   ShowCursor();
}

osgViewer::GraphicsWindow * DeltaWin::CreateGraphicsWindow(const std::string& name, 
                                                           int x, int y, 
                                                           int width, int height, 
                                                           unsigned int screenNum,
                                                           bool cursor, osg::Referenced * inheritedWindowData)
{
   osg::DisplaySettings* ds = osg::DisplaySettings::instance();

   osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

   traits->readDISPLAY();
   if (traits->displayNum<0)
      traits->displayNum = 0;

   traits->windowName = name;
   traits->screenNum = screenNum;
   traits->x = x;
   traits->y = y;
   traits->width = width;
   traits->height = height;
   traits->alpha = ds->getMinimumNumAlphaBits();
   traits->stencil = ds->getMinimumNumStencilBits();
   traits->windowDecoration = true;
   traits->doubleBuffer = true;
   traits->sharedContext = 0;
   traits->sampleBuffers = ds->getMultiSamples();
   traits->samples = ds->getNumMultiSamples();
   traits->inheritedWindowData = inheritedWindowData;
   
   if (ds->getStereo())
   {
      switch (ds->getStereoMode())
      {
         case (osg::DisplaySettings::QUAD_BUFFER):
            traits->quadBufferStereo = true;
            break;
         case (osg::DisplaySettings::VERTICAL_INTERLACE):
         case (osg::DisplaySettings::CHECKERBOARD):
         case (osg::DisplaySettings::HORIZONTAL_INTERLACE):
            traits->stencil = 8;
            break;
         default:
            break;
      }
   }

   osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
   
   if (gc.valid() == false)
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_CONTEXT,
         "The graphics context could not be created.",
         __FILE__, __LINE__ );
   }
   
   osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
   if (gw)
   {
      gw->getEventQueue()->getCurrentEventState()->setWindowRectangle(x, y, width, height );
   }
   
   mOsgViewerGraphicsWindow = gw;
   gw->realize();
   gw->makeCurrent();
   
   return (mOsgViewerGraphicsWindow.get());
}


/////////////////////////////////////////////////
DeltaWin::DeltaWin(const std::string& name, osgViewer::GraphicsWindow * gw) :
   Base(name),
   mIsFullScreen(false),
   mShowCursor(true)
{
   RegisterInstance(this);
 
   try
   {
      SetOsgViewerGraphicsWindow(gw);
   }
   catch(dtUtil::Exception & ex)
   {
      DeregisterInstance(this);
      throw ex;
   }
   
//   SetWindowTitle(name);
   SetWindowTitle("toto");
   ShowCursor();
}

/////////////////////////////////////////////////
DeltaWin::~DeltaWin()
{
   KillGLWindow();
//   mRenderSurface->cancel();
   //mRenderSurface = 0;

   DeregisterInstance(this);
}
/////////////////////////////////////////////////
void DeltaWin::SetWindowTitle( const std::string& title )
{
   mOsgViewerGraphicsWindow->setWindowName(title);
}
/////////////////////////////////////////////////
const std::string DeltaWin::GetWindowTitle() const
{
   return mOsgViewerGraphicsWindow->getWindowName();
}

void DeltaWin::ShowCursor( bool show )
{
   mOsgViewerGraphicsWindow->useCursor(show);
}

void DeltaWin::SetFullScreenMode( bool enable )
{
   if (mIsFullScreen == enable) return;
   
   osg::GraphicsContext::WindowingSystemInterface    *wsi = osg::GraphicsContext::getWindowingSystemInterface();

   if (wsi == NULL)
   {
      LOG_WARNING("Error, no WindowSystemInterface available, cannot toggle window fullscreen.");
      return;
   }

   unsigned int screenWidth;
   unsigned int screenHeight;

   wsi->getScreenResolution(*(mOsgViewerGraphicsWindow->getTraits()), screenWidth, screenHeight);

   if (mIsFullScreen)
   {
      osg::Vec2 resolution;

      int rx = 640;
      int ry = 480;
      
      mOsgViewerGraphicsWindow->setWindowDecoration(true);
      mOsgViewerGraphicsWindow->setWindowRectangle((screenWidth - rx) / 2, (screenHeight - ry) / 2, rx, ry);
   }
   else
   {
      mOsgViewerGraphicsWindow->setWindowDecoration(false);
      mOsgViewerGraphicsWindow->setWindowRectangle(0, 0, screenWidth, screenHeight);
   }
   
   mIsFullScreen = enable;

   mOsgViewerGraphicsWindow->grabFocusIfPointerInWindow();
}

///////////////////////////////////////////////////
void DeltaWin::SetPosition( int x, int y, int width, int height )
{
   mOsgViewerGraphicsWindow->setWindowRectangle(x, y, width, height);
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
   mOsgViewerGraphicsWindow->getWindowRectangle( x, y, width, height );
}

///////////////////////////////////////////////////
DeltaWin::PositionSize DeltaWin::GetPosition()
{
   PositionSize positionSize;
   GetPosition(positionSize.mX, positionSize.mY, positionSize.mWidth, positionSize.mHeight);
   return positionSize;
}

///////////////////////////////////////////////////
void DeltaWin::SetOsgViewerGraphicsWindow(osgViewer::GraphicsWindow * graphicsWindow)
{
   if( graphicsWindow == 0 )
   {
      throw dtUtil::Exception(dtCore::ExceptionEnum::INVALID_PARAMETER,
         "Supplied osgViewer::GraphicsWindow is invalid", __FILE__, __LINE__);
   }
   
   mOsgViewerGraphicsWindow = graphicsWindow;
//   mOsgViewerGraphicsWindow->realize();
   
   if (mCamera.valid())
   {
      mCamera->UpdateFromWindow();
   }
}

/////////////////////////////////////////////////
bool DeltaWin::CalcPixelCoords( float x, float y, float &pixel_x, float &pixel_y )
{
   if ( x < -1.0f || x > 1.0f ) return false;
   if ( y < -1.0f || y > 1.0f ) return false;
   
   int wx, wy;
   int w, h;
   mOsgViewerGraphicsWindow->getWindowRectangle( wx, wy, w, h );

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
   int w, h;
   mOsgViewerGraphicsWindow->getWindowRectangle( wx, wy, w, h );

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
