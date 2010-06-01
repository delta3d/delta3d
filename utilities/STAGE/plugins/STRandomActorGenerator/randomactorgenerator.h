#ifndef STAGE_RANDOM_ACTOR_GENERATOR_PLUGIN__H_
#define STAGE_RANDOM_ACTOR_GENERATOR_PLUGIN__H_

#include "export.h"
#include "ui_randomactorgenerator.h"

#include <dtCore/refptr.h>
#include <dtEditQt/plugininterface.h>
#include <dtQt/typedefs.h>

#include <osg/Vec3>

#include <QtGui/QDockWidget>

//forward declarations
class Ui_RandomActorGenerator;

namespace dtDAL
{
   class ActorProxy;
}

namespace dtEditQt
{
   class MainWindow;
}

/**
   The random actor generator is a STAGE plugin that can be used to import
   randomly generate actors in a given area.

   We'll be using the STAGE VolumeEditActor as the location where random
   actors get spawned.
*/
class STAGE_RANDOM_ACTOR_GENERATOR_EXPORT RandomActorGeneratorPlugin
   : public QDockWidget
   , public dtEditQt::Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;

   RandomActorGeneratorPlugin(dtEditQt::MainWindow* mw);

   ~RandomActorGeneratorPlugin();

   virtual void Destroy();

   /** override close event to get notified when user closes the dock */
   virtual void closeEvent(QCloseEvent* event);

public slots:
   void OnGeneratePushed();
   void OnRefreshActorList();
   void OnSelectedActorChange(ActorProxyRefPtrVector& actors);

private:
   void NewActorProxyInsideVolumeEditor(dtDAL::ActorProxy* proxyToCopy);
   osg::Vec3 RandomPointInsideBox();
   osg::Vec3 RandomPointInsideSphere();

   Ui_RandomActorGenerator mUI;

   dtEditQt::MainWindow* mMainWindow;

};

#endif //STAGE_RANDOM_ACTOR_GENERATOR_PLUGIN__H_
