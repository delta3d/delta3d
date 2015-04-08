#ifndef STGAMESTART_PLUGIN
#define STGAMESTART_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

using namespace dtEditQt;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace dtEditQt
{
   class Viewport;
}

namespace dtCore
{
   class ActorProperty;
}

/// @endcond

class DT_ST_GAME_START_EXPORT STGameStartPlugin : public QDockWidget, public Plugin
{
   Q_OBJECT

public:
   const static std::string PLUGIN_NAME;

   STGameStartPlugin(MainWindow* mw);

   ~STGameStartPlugin();

   void SetDefaults();

   virtual void Destroy();

   virtual void closeEvent(QCloseEvent* event);

public slots:
  
   void RunGameStart();
   void ReadOut();
   void ReadErr();

private:
   MainWindow*   mMainWindow;
   QPushButton*  mRunButton;
   QLineEdit*    mLibraryNameEdit;
   QLineEdit*    mMapNameEdit;
   QLineEdit*    mBaseMapEdit;
   QLineEdit*    mWorkingDirEdit;
   QTextEdit*    mOutputWindow;

};

#endif // STGAMESTART_PLUGIN
