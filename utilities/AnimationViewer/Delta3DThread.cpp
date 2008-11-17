#include <QtOpenGL/QGLContext>

#include "Delta3DThread.h"
#include "MainWindow.h"
#include "Viewer.h"
#include <dtCore/system.h>
#include <dtCore/deltawin.h>

#include "OSGAdapterWidget.h"


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
   dtQt::OSGAdapterWidget& glWidget = *mWin->GetGLWidget();
   //glWidget.ThreadedInitializeGL();
   //glWidget.ThreadedMakeCurrent();
   
   mViewer = new Viewer();

   glWidget.SetGraphicsWindow(*mViewer->GetWindow()->GetOsgViewerGraphicsWindow());
   
   mViewer->Config();

   connect(mWin, SIGNAL(FileToLoad(const QString&)), mViewer.get(), SLOT(OnLoadCharFile(const QString&)) );

   connect(mViewer.get(), SIGNAL(AnimationLoaded(unsigned int,const QString &,unsigned int,unsigned int,float)),
           mWin, SLOT(OnNewAnimation(unsigned int,const QString&,unsigned int,unsigned int,float)));

   connect(mViewer.get(), SIGNAL(MeshLoaded(int,const QString&)), mWin, SLOT(OnNewMesh(int,const QString&)));

   connect(mViewer.get(), SIGNAL(PoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*>&, dtAnim::CharDrawable*)), 
           mWin, SLOT(OnPoseMeshesLoaded(const std::vector<dtAnim::PoseMesh*>&, dtAnim::CharDrawable*)));

   connect(mViewer.get(), SIGNAL(MaterialLoaded(int,const QString&,const QColor&,const QColor&,const QColor&,float )), 
           mWin, SLOT(OnNewMaterial(int,const QString&,const QColor&,const QColor&,const QColor&,float)));   

   connect(mWin, SIGNAL(ShowMesh(int)), mViewer.get(), SLOT(OnShowMesh(int)));
   connect(mWin, SIGNAL(HideMesh(int)), mViewer.get(), SLOT(OnHideMesh(int)));

   connect(mViewer.get(), SIGNAL(ErrorOccured(const QString&)), mWin, SLOT(OnDisplayError(const QString&)) );

   connect(mWin, SIGNAL(StartAnimation(unsigned int,float,float)), mViewer.get(), SLOT(OnStartAnimation(unsigned int,float,float)));
   connect(mWin, SIGNAL(StopAnimation(unsigned int,float)), mViewer.get(), SLOT(OnStopAnimation(unsigned int,float)));
   connect(mWin, SIGNAL(StartAction(unsigned int,float,float)), mViewer.get(), SLOT(OnStartAction(unsigned int,float,float)));
   connect(mWin, SIGNAL(LODScale_Changed(float)), mViewer.get(), SLOT(OnLODScale_Changed(float)));
   connect(mWin, SIGNAL(SpeedChanged(float)), mViewer.get(), SLOT(OnSpeedChanged(float)));
   connect(mWin, SIGNAL(ScaleFactorChanged(float)), mViewer.get(), SLOT(OnScaleFactorChanged(float)));

   connect(&mTimer, SIGNAL(timeout()), mViewer.get(), SLOT(OnTimeout()));
   connect(mViewer.get(), SIGNAL(BlendUpdate(const std::vector<float>&)), mWin, SLOT(OnBlendUpdate(const std::vector<float>&)));

   connect((QObject*)mWin->mShadedAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetShaded()));
   connect((QObject*)mWin->mWireframeAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetWireframe()));
   connect((QObject*)mWin->mShadedWireAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetShadedWireframe()));
   connect((QObject*)mWin->mBoneBasisAction, SIGNAL(toggled(bool)), mViewer.get(), SLOT(OnSetBoneBasisDisplay(bool)));

   dtCore::System::GetInstance().Start();
   mTimer.start(10);

   //this->exec();

   mWin->OnConfiged();
}
