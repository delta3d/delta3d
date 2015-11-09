#include <prefix/dtcoreprefix.h>

#include <dtCore/deltawin.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/windowresizecallback.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <osgViewer/GraphicsWindow>

#include <cassert>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(DeltaWin)
namespace dtCore
{
   ///Default WindowResizeCallback.  Used to implement the default OSG window resizing
   class DefResizeCB : public WindowResizeCallback
   {
   public:
      DefResizeCB(){};
      ~DefResizeCB(){};

      virtual void operator () (const dtCore::DeltaWin& win, int x, int y, int width, int height)
      {
         dtCore::DeltaWin& non_const_win = const_cast<dtCore::DeltaWin&>(win);
         if (non_const_win.GetOsgViewerGraphicsWindow() != NULL)
         {
            non_const_win.GetOsgViewerGraphicsWindow()->resizedImplementation(x, y, width, height);
         }
         else
         {
            LOG_ERROR("In the default resize callback of deltawin, GetOsgViewerGraphicsWindow() is NULL.  This is a bug.");
         }
      }
   };
}

////////////////////////////////////////////////////////////////////////////////
DeltaWin::DeltaWin(const DeltaWinTraits& windowTraits)
   : Base(windowTraits.name)
   , mLastWindowedWidth(640)
   , mLastWindowedHeight(480)
   , mIsFullScreen(false)
   , mShowCursor(true)
   , mResizeCallbackContainer(NULL)

{
   RegisterInstance(this);
   CreateDeltaWindow(windowTraits);
}


////////////////////////////////////////////////////////////////////////////////
void DeltaWin::CreateDeltaWindow(const DeltaWinTraits& windowTraits)
{
   osg::ref_ptr<osg::GraphicsContext::Traits> osgTraits;
   osgTraits = CreateOSGTraits(windowTraits);

   mResizeCallbackContainer = new WindowResizeContainer(*this);

   //automatically add in our default callback
   WindowResizeCallback* defCB = new DefResizeCB();
   mResizeCallbackContainer->AddCallback(*defCB);

   CreateGraphicsWindow(*osgTraits, windowTraits.realizeUponCreate);

   if (windowTraits.fullScreen)
   {
      mIsFullScreen = true;
   }

   // Save the window size in case we have to restore it later
   mLastWindowedWidth  = osgTraits->width;
   mLastWindowedHeight = osgTraits->height;

   SetShowCursor(windowTraits.showCursor);
}

////////////////////////////////////////////////////////////////////////////////
void DeltaWin::CreateGraphicsWindow(osg::GraphicsContext::Traits& traits,
                                    bool realizeUponCreate)
{
   osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(&traits);

   if (gc.valid() == false)
   {
      throw dtCore::InvalidContextException(
         "The graphics context could not be created.",
         __FILE__, __LINE__);
   }

   gc->setResizedCallback(mResizeCallbackContainer.get());

   mOsgViewerGraphicsWindow = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
   if (mOsgViewerGraphicsWindow.valid())
   {
      mOsgViewerGraphicsWindow->getEventQueue()->getCurrentEventState()->setWindowRectangle(traits.x, traits.y,
                                                                      traits.width, traits.height);
      if (realizeUponCreate)
      {
         mOsgViewerGraphicsWindow->realize();
         mOsgViewerGraphicsWindow->makeCurrent();
         mOsgViewerGraphicsWindow->setClearMask(~0);
         mOsgViewerGraphicsWindow->clear();
         mOsgViewerGraphicsWindow->setClearMask(0);
      }

   }
}


////////////////////////////////////////////////////////////////////////////////
DeltaWin::DeltaWin(const std::string& name, osgViewer::GraphicsWindow& gw)
   : Base(name)
   , mIsFullScreen(false)
   , mShowCursor(true)
{
   RegisterInstance(this);

   mOsgViewerGraphicsWindow = &gw;

   SetWindowTitle(name);
   SetShowCursor(true);
}

////////////////////////////////////////////////////////////////////////////////
DeltaWin::~DeltaWin()
{
   KillGLWindow();
   DeregisterInstance(this);

   if (mOsgViewerGraphicsWindow->valid())
   {
      mOsgViewerGraphicsWindow->setResizedCallback(NULL);
      mResizeCallbackContainer = NULL;
   }
}

////////////////////////////////////////////////////////////////////////////////
void DeltaWin::SetWindowTitle(const std::string& title)
{
   mOsgViewerGraphicsWindow->setWindowName(title);
}

////////////////////////////////////////////////////////////////////////////////
const std::string DeltaWin::GetWindowTitle() const
{
   return mOsgViewerGraphicsWindow->getWindowName();
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::DeltaWin::SetShowCursor(bool shouldShow)
{
   mShowCursor = shouldShow;
   mOsgViewerGraphicsWindow->useCursor(shouldShow);
}

////////////////////////////////////////////////////////////////////////////////
void DeltaWin::SetFullScreenMode(bool enable)
{
   if (mIsFullScreen == enable)
   {
      return;
   }

   osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();

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
      int rx = mLastWindowedWidth;
      int ry = mLastWindowedHeight;

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

////////////////////////////////////////////////////////////////////////////////
void DeltaWin::SetPosition(int x, int y, int width, int height)
{
   mOsgViewerGraphicsWindow->setWindowRectangle(x, y, width, height);

   // Save the window size in case we have to restore it later
   mLastWindowedWidth  = width;
   mLastWindowedHeight = height;
}

////////////////////////////////////////////////////////////////////////////////
void DeltaWin::SetPosition(const DeltaWin::PositionSize& positionSize)
{
   SetPosition(positionSize.mX, positionSize.mY, positionSize.mWidth, positionSize.mHeight);
}

////////////////////////////////////////////////////////////////////////////////
void DeltaWin::GetPosition(int& x, int& y, int& width, int& height) const
{
   mOsgViewerGraphicsWindow->getWindowRectangle(x, y, width, height);
}

////////////////////////////////////////////////////////////////////////////////
DeltaWin::PositionSize DeltaWin::GetPosition() const
{
   PositionSize positionSize;
   GetPosition(positionSize.mX, positionSize.mY, positionSize.mWidth, positionSize.mHeight);
   return positionSize;
}

////////////////////////////////////////////////////////////////////////////////
bool DeltaWin::CalcPixelCoords(float winX, float winY, float &pixelX, float &pixelY) const
{
   if (winX < -1.0f || winX > 1.0f) return false;
   if (winY < -1.0f || winY > 1.0f) return false;

   int wx, wy;
   int w, h;
   mOsgViewerGraphicsWindow->getWindowRectangle(wx, wy, w, h);

   pixelX = (w/2) * (winX + 1.0f);
   pixelY = (h/2) * (winY + 1.0f);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool DeltaWin::CalcPixelCoords(const osg::Vec2 &window_xy, osg::Vec2 &pixel_xy) const
{
   return CalcPixelCoords(window_xy.x(), window_xy.y(), pixel_xy.x(), pixel_xy.y());
}

/////////////////////////////////////////////////
bool DeltaWin::CalcWindowCoords(float pixel_x, float pixel_y, float& x, float& y) const
{
   int wx, wy;
   int w, h;
   mOsgViewerGraphicsWindow->getWindowRectangle(wx, wy, w, h);

   if (pixel_x < 0 || pixel_x > w) return false;
   if (pixel_y < 0 || pixel_y > h) return false;

   if (w != 0 && h != 0)
   {
      x = (2 * pixel_x / w) - 1;
      y = (2 * pixel_y / h) - 1;

      return true;
   }
   else
   {
      LOG_DEBUG("Window size of 0");
      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DeltaWin::CalcWindowCoords(const osg::Vec2& pixel_xy, osg::Vec2& window_xy) const
{
   return CalcWindowCoords(pixel_xy.x(), pixel_xy.y(), window_xy.x(), window_xy.y());
}

////////////////////////////////////////////////////////////////////////////////
bool DeltaWin::ChangeScreenResolution(Resolution res)
{
   return ChangeScreenResolution(res.width, res.height, res.bitDepth, res.refresh);
}

/////////////////////////////////////////////////
int DeltaWin::IsValidResolution(const ResolutionVec &rv, int width, int height, int refreshRate, int colorDepth)
{
   for (unsigned int i = 0; i < rv.size(); ++i)
   {
      if (width && height)
      {
         if (refreshRate && colorDepth)
         {
            if (rv[i].width == width && rv[i].height == height && rv[i].refresh == refreshRate && rv[i].bitDepth == colorDepth)
            {
               return i;
            }
         }
         else if (refreshRate)
         {
            if (rv[i].width == width && rv[i].height == height && rv[i].refresh == refreshRate)
            {
               return i;
            }
         }
         else if (colorDepth)
         {
            if (rv[i].width == width && rv[i].height == height && rv[i].bitDepth == colorDepth)
            {
               return i;
            }
         }
         else
         {
            if (rv[i].width == width && rv[i].height == height)
            {
               return i;
            }
         }
      }
      else if (refreshRate && colorDepth)
      {
         if (rv[i].refresh == refreshRate && rv[i].bitDepth == colorDepth)
         {
            return i;
         }
      }
      else if (refreshRate)
      {
         if (rv[i].refresh == refreshRate)
         {
            return i;
         }
      }
      else if (colorDepth)
      {
         if (rv[i].bitDepth == colorDepth)
         {
            return i;
         }
      }
   }

   return -1;
}

////////////////////////////////////////////////////////////////////////////////
bool DeltaWin::IsValidResolution(const DeltaWin::Resolution& candidate)
{
   ResolutionVec vec = DeltaWin::GetResolutions();

   DeltaWin::ResolutionVec::iterator iter = vec.begin();
   DeltaWin::ResolutionVec::iterator enditer = vec.end();
   while (iter != enditer)
   {
      if ((candidate.width==iter->width)       &&
          (candidate.height==iter->height)     &&
          (candidate.bitDepth==iter->bitDepth) &&
          (candidate.refresh==iter->refresh))
      {
         return true;
      }

      ++iter;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
int DeltaWin::CalcRefreshRate(int horzTotal, int vertTotal, int dotclock)
{
   return static_cast<int>(0.5f + ((1000.0f * dotclock) / (horzTotal * vertTotal)));
}

////////////////////////////////////////////////////////////////////////////////
osg::ref_ptr<osg::GraphicsContext::Traits> DeltaWin::CreateOSGTraits(const DeltaWinTraits& deltaTraits) const
{
   osg::DisplaySettings* ds = osg::DisplaySettings::instance();

   osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

   //Favor the value of the environment variable "DISPLAY" to set the
   //diplayNum, screenNum, and host name, unless it fails, in which case use the
   //passed in value.  Format is "host:displayNum.screenNum".

   //Doing this first because sometimes readDISPLAY doesn't set displayNum
   //and screenNum to -1 when DISPLAY isn't defined (even though it claims
   //to in the OSG docs).
   traits->displayNum = deltaTraits.displayNum;
   traits->screenNum = deltaTraits.screenNum;

   traits->readDISPLAY();

   if (traits->displayNum < 0)
   {
      traits->displayNum = deltaTraits.displayNum;
   }

   if (traits->screenNum < 0)
   {
      traits->screenNum = deltaTraits.screenNum;
   }

   if (traits->hostName.empty())
   {
      traits->hostName = deltaTraits.hostName;
   }

   //if displayNum or screenNum are still undefined, use the default
   traits->setUndefinedScreenDetailsToDefaultScreen();

   traits->windowName = deltaTraits.name;
   traits->x = deltaTraits.x;
   traits->y = deltaTraits.y;
   traits->width = deltaTraits.width;
   traits->height = deltaTraits.height;
   traits->alpha = ds->getMinimumNumAlphaBits();
   traits->stencil = ds->getMinimumNumStencilBits();
   traits->windowDecoration = deltaTraits.windowDecoration;
   traits->doubleBuffer = true;
   traits->supportsResize = deltaTraits.supportResize;
   traits->sharedContext = deltaTraits.contextToShare;

   traits->sampleBuffers = ds->getMultiSamples();
   traits->samples = ds->getNumMultiSamples();
   traits->inheritedWindowData = deltaTraits.inheritedWindowData;
   traits->setInheritedWindowPixelFormat = true;
   traits->vsync = deltaTraits.vsync;

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

   //create the Window fullscreen, if required
   if (deltaTraits.fullScreen == true)
   {
      osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();

      if (wsi != NULL)
      {
         unsigned int screenWidth;
         unsigned int screenHeight;

         wsi->getScreenResolution(*traits, screenWidth, screenHeight);
         traits->windowDecoration = false;
         traits->x = 0;
         traits->y = 0;
         traits->width = screenWidth;
         traits->height = screenHeight;
      }
   }

   return traits;
}


//////////////////////////////////////////////////////////////////////////
void dtCore::DeltaWin::AddResizeCallback(WindowResizeCallback& cb)
{
   mResizeCallbackContainer->AddCallback(cb);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::DeltaWin::RemoveResizeCallback(WindowResizeCallback& cb)
{
   mResizeCallbackContainer->RemoveCallback(cb);
}

