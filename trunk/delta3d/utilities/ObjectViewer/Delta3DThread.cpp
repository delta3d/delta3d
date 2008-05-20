#include <QtOpenGL/QGLContext>

#include "Delta3DThread.h"
#include "ObjectViewer.h"
#include "ObjectWorkspace.h"
#include <dtCore/system.h>
#include <dtCore/deltawin.h>

#include "OSGAdapterWidget.h"


/////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////
Delta3DThread::Delta3DThread(QObject *parent):
QThread(parent)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
Delta3DThread::~Delta3DThread()
{
   if (isRunning())
   {
      this->quit();
   }
   dtCore::System::GetInstance().Stop();
}

/////////////////////////////////////////////////////////////////////////////////////////
void Delta3DThread::run()
{
   osg::GraphicsContext::WindowingSystemInterface* winSys = osg::GraphicsContext::getWindowingSystemInterface();

   if (winSys != NULL)
   {
      osg::GraphicsContext::setWindowingSystemInterface(new EmbeddedWindowSystemWrapper(*winSys));
   }

   //need to set the current context so that all the open gl stuff in osg can initialize.
   dtQt::OSGAdapterWidget& glWidget = *mWin->GetGLWidget();
   
   mViewer = new ObjectViewer();

   glWidget.SetGraphicsWindow(*mViewer->GetWindow()->GetOsgViewerGraphicsWindow());
   
   mViewer->Config();

   connect(mWin, SIGNAL(FileToLoad(const QString&)), mViewer.get(), SLOT(OnLoadCharFile(const QString&)) );  
   connect(mWin, SIGNAL(LoadShaderDefinition(const QString &)), mViewer.get(), SLOT(OnLoadShaderFile(const QString &)));
   connect(mWin, SIGNAL(LoadGeometry(const QString &)), mViewer.get(), SLOT(OnLoadGeometryFile(const QString &)));

   connect(mViewer.get(), SIGNAL(ShaderLoaded(const std::string &, const std::string &)), 
           mWin, SLOT(OnNewShader(const std::string &, const std::string &)));

   connect(mViewer.get(), SIGNAL(PoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*>&, dtAnim::CharDrawable*)), 
           mWin, SLOT(OnPoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*>&, dtAnim::CharDrawable*))); 
   
   connect(mWin, SIGNAL(StartAction(unsigned int,float,float)), mViewer.get(), SLOT(OnStartAction(unsigned int,float,float)));

   connect(&mTimer, SIGNAL(timeout()), mViewer.get(), SLOT(OnTimeout()));

   // Toolbar connections
   connect((QObject*)mWin->mShadedAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetShaded()));
   connect((QObject*)mWin->mWireframeAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetWireframe()));
   connect((QObject*)mWin->mShadedWireAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetShadedWireframe()));

   dtCore::System::GetInstance().Start();
   mTimer.start(10);

   mWin->OnInitialization();
}
