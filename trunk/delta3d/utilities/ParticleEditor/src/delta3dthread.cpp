#include <delta3dthread.h>
#include <dtCore/deltawin.h>

#include <osg/Version>

#include <osgViewer/GraphicsWindow>

////////////////////////////////////////////////////////////////////////////////
class EmbeddedWindowSystemWrapper: public osg::GraphicsContext::WindowingSystemInterface
{
   public:
      EmbeddedWindowSystemWrapper(osg::GraphicsContext::WindowingSystemInterface& oldInterface):
         mInterface(&oldInterface)
      {
      }

      virtual unsigned int getNumScreens(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier =
         osg::GraphicsContext::ScreenIdentifier())
      {
         return mInterface->getNumScreens(screenIdentifier);
      }

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION >= 8
      virtual void getScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettings & resolution)
      {
         mInterface->getScreenSettings(si, resolution);
      }

      virtual void enumerateScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettingsList & rl)
      {
         mInterface->enumerateScreenSettings(si, rl);
      }
#endif

      virtual void getScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               unsigned int& width, unsigned int& height)
      {
         mInterface->getScreenResolution(screenIdentifier, width, height);
      }

      virtual bool setScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               unsigned int width, unsigned int height)
      {
         return mInterface->setScreenResolution(screenIdentifier, width, height);
      }

      virtual bool setScreenRefreshRate(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               double refreshRate)
      {
         return mInterface->setScreenRefreshRate(screenIdentifier, refreshRate);
      }

      virtual osg::GraphicsContext* createGraphicsContext(osg::GraphicsContext::Traits* traits)
      {
         return new osgViewer::GraphicsWindowEmbedded(traits);
      }

   protected:
      virtual ~EmbeddedWindowSystemWrapper() {};
   private:
      dtCore::RefPtr<osg::GraphicsContext::WindowingSystemInterface> mInterface;
};

////////////////////////////////////////////////////////////////////////////////
Delta3DThread::Delta3DThread(QObject* parent):
QThread(parent)
{
}

////////////////////////////////////////////////////////////////////////////////
Delta3DThread::~Delta3DThread()
{
   if (isRunning())
   {
      this->quit();
   }

   dtCore::System::GetInstance().Stop();
}

////////////////////////////////////////////////////////////////////////////////
void Delta3DThread::run()
{
   osg::GraphicsContext::WindowingSystemInterface* winSys = osg::GraphicsContext::getWindowingSystemInterface();

   if (winSys != NULL)
   {
      osg::GraphicsContext::setWindowingSystemInterface(new EmbeddedWindowSystemWrapper(*winSys));
   }

   //need to set the current context so that all the open gl stuff in osg can initialize.
   dtQt::ViewWindow& glWidget = *mWin->GetGLWidget();
   glWidget.setMinimumWidth(glWidget.parentWidget()->width());
   glWidget.setMinimumHeight(glWidget.parentWidget()->height());
   //glWidget.ThreadedInitializeGL();
   //glWidget.ThreadedMakeCurrent();

   mpViewer = new ParticleViewer();

   glWidget.SetGraphicsWindow(*mpViewer->GetWindow()->GetOsgViewerGraphicsWindow());

   mpViewer->Config();

   mWin->SetParticleViewer(mpViewer);
   mWin->SetupUI();

   dtCore::System::GetInstance().Start();
   mTimer.start(10);
}

////////////////////////////////////////////////////////////////////////////////
