#include <QtGui/QApplication>
#include <QtOpenGL/QGLContext>
#include <QtGui/QDockWidget>
#include <QtCore/QFileInfo>

#include "Delta3DThread.h"
#include "ObjectViewer.h"
#include "ObjectWorkspace.h"
#include <dtCore/system.h>
#include <dtCore/deltawin.h>

#include <dtQt/viewwindow.h>


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
Delta3DThread::Delta3DThread(QApplication* parent)
   : QThread(parent)
{
   QStringList argList = parent->arguments();

   // If we have an argument(possibly from drag and drop)
   if (argList.size() > 1)
   {
      mStartupFile = argList.at(1).toStdString();
   }
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
   
   mViewer = new ObjectViewer();

   glWidget.SetGraphicsWindow(*mViewer->GetWindow()->GetOsgViewerGraphicsWindow());
   
   mViewer->Config();

   MakeConnections();    

   dtCore::System::GetInstance().Start();  
   QFileInfo fileInfo(mStartupFile.c_str());
   
   mWin->OnInitialization();

   if (fileInfo.fileName().endsWith(QString("xml"), Qt::CaseInsensitive))
   {
      mWin->OnLoadMap(fileInfo.baseName().toStdString());
   }
   else
   {
      mWin->OnLoadGeometry(mStartupFile.c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////
void Delta3DThread::MakeConnections()
{
   // Menu connections
   connect(mWin, SIGNAL(LoadShaderDefinition(const QString&)), 
      mViewer.get(), SLOT(OnLoadShaderFile(const QString&)));
   
   connect((QObject*)mWin, SIGNAL(SetGenerateTangentAttribute(bool)), 
      mViewer.get(), SLOT(OnSetGenerateTangentAttribute(bool)));

   connect(mWin, SIGNAL(ReloadShaderDefinition()), 
      mWin->GetResourceObject(), SLOT(OnReloadShaderFiles()));

   // Resource dock connections
   connect(mWin->GetResourceObject(), SIGNAL(LoadGeometry(const std::string&)),
      mViewer.get(), SLOT(OnLoadGeometryFile(const std::string&)));

   connect(mWin->GetResourceObject(), SIGNAL(LoadMap(const std::string&)),
      mViewer.get(), SLOT(OnLoadMapFile(const std::string&)));

   connect(mWin->GetResourceObject(), SIGNAL(UnloadGeometry()),
      mViewer.get(), SLOT(OnUnloadGeometryFile()));

   connect(mViewer.get(), SIGNAL(ShaderLoaded(const std::string&, const std::string&, const std::string&)), 
      mWin->GetResourceObject(), SLOT(OnNewShader(const std::string&, const std::string&, const std::string&)));

   connect(mWin->GetResourceObject(), SIGNAL(ApplyShader(const std::string&, const std::string&)), 
      mViewer.get(), SLOT(OnApplyShader(const std::string&, const std::string&)));  

   connect(mViewer.get(), SIGNAL(LightUpdate(const LightInfo&)),
      mWin->GetResourceObject(), SLOT(OnLightUpdate(const LightInfo&)));

   connect(mWin->GetResourceObject(), SIGNAL(RemoveShader()),
      mViewer.get(), SLOT(OnRemoveShader()));

   connect(mWin->GetResourceObject(), SIGNAL(FixLights()),
      mViewer.get(), SLOT(OnFixLights()));

   connect(mWin->GetResourceObject(), SIGNAL(SetCurrentLight(int)),
      mViewer.get(), SLOT(OnSetCurrentLight(int)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightEnabled(int, bool)),
      mViewer.get(), SLOT(OnSetLightEnabled(int, bool)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightType(int, int)),
      mViewer.get(), SLOT(OnSetLightType(int, int)));

   connect(mWin->GetResourceObject(), SIGNAL(SetAmbient(int, const osg::Vec4&)),
           mViewer.get(), SLOT(OnSetAmbient(int, const osg::Vec4&)));

   connect(mWin->GetResourceObject(), SIGNAL(SetDiffuse(int, const osg::Vec4&)),
           mViewer.get(), SLOT(OnSetDiffuse(int, const osg::Vec4&)));

   connect(mWin->GetResourceObject(), SIGNAL(SetSpecular(int, const osg::Vec4&)),
           mViewer.get(), SLOT(OnSetSpecular(int, const osg::Vec4&)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightAzimuth(int, float)),
      mViewer.get(), SLOT(OnSetLightAzimuth(int, float)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightElevation(int, float)),
      mViewer.get(), SLOT(OnSetLightElevation(int, float)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightCutoff(int, float)),
      mViewer.get(), SLOT(OnSetLightCutoff(int, float)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightExponent(int, float)),
      mViewer.get(), SLOT(OnSetLightExponent(int, float)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightConstant(int, float)),
      mViewer.get(), SLOT(OnSetLightConstant(int, float)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightLinear(int, float)),
      mViewer.get(), SLOT(OnSetLightLinear(int, float)));

   connect(mWin->GetResourceObject(), SIGNAL(SetLightQuadratic(int, float)),
      mViewer.get(), SLOT(OnSetLightQuadratic(int, float)));

   connect(mWin, SIGNAL(ToggleGrid(bool)), mViewer.get(), SLOT(OnToggleGrid(bool)));

   connect(mWin->GetResourceObject(), SIGNAL(ToggleVertexShaderSources(bool)),
      mWin, SLOT(OnToggleVertexShaderSource(bool)));

   connect(mWin->GetResourceObject(), SIGNAL(ToggleFragmentShaderSources(bool)),
      mWin, SLOT(OnToggleFragmentShaderSource(bool)));

   connect(mWin->GetResourceObject(), SIGNAL(RemoveShaderDef(const std::string&)),
      mWin, SLOT(OnRemoveShaderDef(const std::string&)));

   // Toolbar connections
   connect((QObject*)mWin->mShadedAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetShaded()));
   connect((QObject*)mWin->mWireframeAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetWireframe()));
   connect((QObject*)mWin->mShadedWireAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnSetShadedWireframe()));
   connect((QObject*)mWin->mLightModeAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnEnterLightMode()));
   connect((QObject*)mWin->mObjectModeAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnEnterObjectMode()));   

   // Editing connections
   connect((QObject*)mWin->mWorldSpaceAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnWorldSpaceMode()));
   connect((QObject*)mWin->mLocalSpaceAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnLocalSpaceMode()));
   connect((QObject*)mWin->mTranslationModeAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnTranslateMode()));
   connect((QObject*)mWin->mRotationModeAction, SIGNAL(triggered()), mViewer.get(), SLOT(OnRotateMode()));
}

////////////////////////////////////////////////////////////////////////////////
