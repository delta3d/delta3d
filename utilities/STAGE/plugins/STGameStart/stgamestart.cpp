#include "stgamestart.h"
#include "ui_stgamestart.h"

#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/editordata.h>

#include <dtCore/deltadrawable.h>
#include <dtCore/map.h>

#include <dtUtil/stringutils.h>
#include <dtUtil/datapathutils.h>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Version>

#include <QtCore/QDebug>
#include <QtCore/QProcess>


const std::string STGameStartPlugin::PLUGIN_NAME = "Game Start";


////////////////////////////////////////////////////////////////////////////////
STGameStartPlugin::STGameStartPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mRunButton(NULL)
{
   // apply layout made with QT designer
   Ui_GameStart ui;
   ui.setupUi(this);

   mMainWindow->addDockWidget(Qt::RightDockWidgetArea, this);

   mRunButton = ui.runButton;

   mLibraryNameEdit = ui.libraryNameEdit;
   mMapNameEdit = ui.mapNameEdit;
   mBaseMapEdit = ui.baseMapEdit;
   mWorkingDirEdit = ui.workingDirectoryEdit;
   mOutputWindow = ui.outputWindow;

   SetDefaults();


   connect(mRunButton, SIGNAL(clicked()), this, SLOT(RunGameStart()));
}

////////////////////////////////////////////////////////////////////////////////
STGameStartPlugin::~STGameStartPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void STGameStartPlugin::Destroy()
{
   mRunButton = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void STGameStartPlugin::SetDefaults()
{
   //todo- find libraries loaded
   mLibraryNameEdit->setText("testApp");

   dtCore::Map* map = EditorData::GetInstance().getCurrentMap();
   if (map)
   {
      mMapNameEdit->setText(map->GetName().c_str());
   }
   else
   {
      mMapNameEdit->setText("TestApp");
   }

   mBaseMapEdit->setText("BaseMap");

   QString workingDir = dtUtil::GetDeltaRootPath().c_str();
   workingDir.append("/examples/testApp");
   mWorkingDirEdit->setText(workingDir);
}

////////////////////////////////////////////////////////////////////////////////
void STGameStartPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->GetPluginManager()->StopPlugin(PLUGIN_NAME);
}

////////////////////////////////////////////////////////////////////////////////
void STGameStartPlugin::RunGameStart()
{
   
      QProcess* gamestartRunner = new QProcess(this);

      QString program(QApplication::applicationDirPath());

#if  defined(DELTA_WIN32)
#ifdef _DEBUG
      program.append("\\GameStartd.exe");
#else
      program.append("\\GameStart.exe");
#endif
#elif defined(__APPLE__)
      if (program.indexOf(QString(".app")) >= 0)
      {
         program = (dtUtil::GetBundlePath() + "/../GameStart.app/Contents/MacOS/GameStart").c_str();
      }
      else
      {
         program.append("/GameStart");
      }
#else
      program.append("/GameStart");
#endif

      gamestartRunner->setWorkingDirectory(mWorkingDirEdit->text());
      
      QString args;
      args.append(mLibraryNameEdit->text());

      QString args2(mMapNameEdit->text());

      QString args3(mBaseMapEdit->text());

      QString args4(dtEditQt::EditorData::GetInstance().getCurrentProjectContext().c_str());

      gamestartRunner->setEnvironment( QProcess::systemEnvironment() );

      QStringList arguments;
      if(!args3.isEmpty())
      {
         arguments << args << "--mapName" << args2  << "--baseMap" << args3 << "--projectPath" << args4;
      }
      else
      {
         arguments << args << "--mapName" << args2 << "--projectPath" << args4;
      }

      gamestartRunner->start(program, arguments);

      if (!gamestartRunner->waitForStarted())
      {
         qDebug() << "Failed to launch " << program << ", with cmd args " << arguments;
      }

      
      mOutputWindow->clear();

      mOutputWindow->append(QString("Application started."));

      connect(gamestartRunner, SIGNAL(readyReadStandardOutput()), this, SLOT(ReadOut()) );
      connect(gamestartRunner, SIGNAL(readyReadStandardError()), this, SLOT(ReadErr()) );

}



//////////////////////////////////////////////////////////////////////////////
void STGameStartPlugin::ReadOut()
{  
   QProcess *p = dynamic_cast<QProcess *>( sender() );

   if (p)
      mOutputWindow->append( p->readAllStandardOutput() ); 
}

//////////////////////////////////////////////////////////////////////////////
void STGameStartPlugin::ReadErr()
{  
   QProcess *p = dynamic_cast<QProcess *>( sender() );

   if (p)
      mOutputWindow->append( p->readAllStandardOutput() );  
}



namespace GameStartPlugin
{
   class DT_ST_GAME_START_EXPORT PluginFactory : public dtEditQt::PluginFactory
   {
   public:
      PluginFactory(): mPlugin(NULL) {}
      ~PluginFactory() {}

      /** get the name of the plugin */
      virtual std::string GetName() { return STGameStartPlugin::PLUGIN_NAME; }

      /** get a description of the plugin */
      virtual std::string GetDescription() { return "Runs the map using the game entry point of the selected actor library."; }

      virtual void GetDependencies(std::list<std::string>& deps)
      {
      }

      /** construct the plugin and return a pointer to it */
      virtual Plugin* Create(MainWindow* mw)
      {
         mPlugin = new STGameStartPlugin(mw);
         return mPlugin;
      }

      virtual void Destroy()
      {
         delete mPlugin;
      }

   private:
      Plugin* mPlugin;
   };
} //namespace MapExporterPlugin

extern "C" DT_ST_GAME_START_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new GameStartPlugin::PluginFactory;
}
