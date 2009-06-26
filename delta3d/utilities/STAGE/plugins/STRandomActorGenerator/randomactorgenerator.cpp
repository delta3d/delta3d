#include "randomactorgenerator.h"

#include <ctime>

#include <dtActors/volumeeditactor.h>
#include <dtCore/transformable.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/librarymanager.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtUtil/fileutils.h>

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QSpinBox>

const std::string RandomActorGeneratorPlugin::PLUGIN_NAME = "Random Actor Generator";

////////////////////////////////////////////////////////////////////////////////
RandomActorGeneratorPlugin::RandomActorGeneratorPlugin(dtEditQt::MainWindow* mw)
   : mMainWindow(mw)
   , mUI()
{
   // apply layout made with QT designer   
   mUI.setupUi(this);

   // add dock widget to STAGE main window
   mw->addDockWidget(Qt::LeftDockWidgetArea, this);    

   connect(mUI.mGenerateBtn, SIGNAL(clicked()), this, SLOT(OnGeneratePushed()));
   //May want to put this back later
   //connect(mUI.mRefreshBtn, SIGNAL(clicked()), this, SLOT(OnRefreshActorList()));   

   //seed the random number generator
   srand(time(NULL));
}

////////////////////////////////////////////////////////////////////////////////
RandomActorGeneratorPlugin::~RandomActorGeneratorPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
// remove our stuff from GUI
void RandomActorGeneratorPlugin::Destroy()
{  
   mMainWindow->removeDockWidget(this);
}


////////////////////////////////////////////////////////////////////////////////
// user has closed the dock. Stop the plugin
void RandomActorGeneratorPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->GetPluginManager()->StopPlugin(PLUGIN_NAME);
}

////////////////////////////////////////////////////////////////////////////////
void RandomActorGeneratorPlugin::OnGeneratePushed() 
{
   //get the VolumeEdit "Actor" from STAGE.  This is the area in which the
   //randomly generated actors will be constrained.
   dtActors::VolumeEditActor* volEditActor = mMainWindow->GetVolumeEditActor();   

   dtEditQt::ViewportOverlay::ActorProxyList& selectionList = 
      dtEditQt::ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
   if(selectionList.size() < 1)
   {      
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,__FUNCTION__,
                                                __LINE__, "No actor selected.");

      QMessageBox::warning(this, tr("No actor selected."), 
         tr("Please select an actor to be generated."), QMessageBox::Ok);     

      return;
   }

   //not going to worry about multiple selections -- just use the first in the list 
   dtDAL::ActorProxy* selectedActorProxy = (selectionList[0]).get();
   //Don't want to just make a bunch of copies of the volume editor:
   if(selectedActorProxy->GetActor() == volEditActor)
   {      
      QMessageBox::warning(this, tr("No actor selected."), 
         tr("Please select an actor to be generated."), QMessageBox::Ok);     

      return;
   }

   for (int i = 0; i < mUI.mNumActorsToGenerate->value(); ++i)
   {
      NewActorProxyInsideVolumeEditor(selectedActorProxy);
   }   
}

////////////////////////////////////////////////////////////////////////////////
void RandomActorGeneratorPlugin::OnRefreshActorList()
{
/* May want this button back in the future...

   dtEditQt::EditorData::GetInstance().getMainWindow()->startWaitCursor();

   dtCore::RefPtr<dtDAL::Map> mapPtr = dtEditQt::EditorData::GetInstance().getCurrentMap();  
   std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > foundProxies;
   mapPtr->FindProxies(foundProxies, std::string(""), std::string(""), 
   std::string(""), std::string(""), dtDAL::Map::Either);

   std::vector< dtCore::RefPtr<dtDAL::ActorProxy> >::iterator it;
   for(it = foundProxies.begin(); it != foundProxies.end(); ++it)
   {
      mUI.mActorToGenerate->addItem((*it).get()->GetActor()->GetName().c_str(),
                                    (*it).get()->GetId().ToString().c_str());      
   }

   dtEditQt::EditorData::GetInstance().getMainWindow()->endWaitCursor();
*/
}

////////////////////////////////////////////////////////////////////////////////
void RandomActorGeneratorPlugin::NewActorProxyInsideVolumeEditor(dtDAL::ActorProxy* proxyToCopy)
{   
   dtActors::VolumeEditActor* volEditActor = mMainWindow->GetVolumeEditActor();
   dtCore::RefPtr<dtDAL::Map> currMap = dtEditQt::EditorData::GetInstance().getCurrentMap();
   osg::Vec3 spawnPoint;   

   if(volEditActor->GetShape() == dtActors::VolumeEditActor::VolumeShapeType::BOX)
   {      
      spawnPoint = RandomPointInsideBox();
   }
   else if(volEditActor->GetShape() == dtActors::VolumeEditActor::VolumeShapeType::SPHERE)
   {
      spawnPoint = RandomPointInsideSphere();
   } 
   else 
   {
      //TODO change this to a Qt error dialog
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,__FUNCTION__,
         __LINE__, "This Volume Editor shape not supported yet (try BOX or SPHERE).");
      return;
   }
   //TODO remainder of shapes:

   //spawn a copy of the currently selected actor:
   dtCore::RefPtr<dtDAL::ActorProxy> aCloneProxy = proxyToCopy->Clone();
   dtCore::Transformable *aClonePtr = dynamic_cast<dtCore::Transformable*>(aCloneProxy->GetActor());
   if(aClonePtr == NULL)
   {
      //TODO change this to a Qt error dialog
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,__FUNCTION__,
         __LINE__, "Actor to create is not a Transformable.");
      return;
   }

   //Add cloned actor to map send out a create event.      
   currMap->AddProxy(*aCloneProxy);
   dtEditQt::EditorEvents::GetInstance().emitActorProxyCreated(aCloneProxy, false);

   //Apply rotation (NOT translation) transformations from original actor to the cloned actor
   //(scale has already been copied during the clone step)
   dtCore::Transform cloneXForm;  
   aClonePtr->GetTransform(cloneXForm);
   dtCore::Transformable* originalTransformable = dynamic_cast<dtCore::Transformable*>(proxyToCopy->GetActor());      
   dtCore::Transform originalTransform;      
   originalTransformable->GetTransform(originalTransform);
   float h,p,r;
   originalTransform.GetRotation(h, p, r);
   cloneXForm.SetRotation(h, p, r);      

   //apply the random translations to the cloned actor's matrix
   cloneXForm.SetTranslation(spawnPoint[0], spawnPoint[1], spawnPoint[2]);
   aClonePtr->SetTransform(cloneXForm);
   
   //Apply rotation and translation from VolEditActor (remember: scale doesn't
   //exist inside Delta Transforms.... if that ever changes this could break)
   osg::Matrixd volEAXMatrix;
   volEAXMatrix = volEditActor->GetMatrix();
   aClonePtr->SetMatrix(aClonePtr->GetMatrix() * volEAXMatrix);

   dtEditQt::ViewportManager::GetInstance().refreshAllViewports();         
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 RandomActorGeneratorPlugin::RandomPointInsideBox()
{
   osg::Vec3 spawnPoint;
   dtActors::VolumeEditActor* volEditActor = mMainWindow->GetVolumeEditActor();
   osg::Vec3 volEAScale = volEditActor->GetScale();   
   double baseLength = volEditActor->GetBaseLength();
   
   //apply volEditActor's scale to each dimension of the box      
   double xLen = baseLength * volEAScale[0];      
   double yLen = baseLength * volEAScale[1];      
   double zLen = baseLength * volEAScale[2];

   double halfX = xLen * 0.5;
   double halfY = yLen * 0.5;
   double halfZ = zLen * 0.5;
   
   spawnPoint[0] = (xLen * rand() / RAND_MAX) - halfX;
   spawnPoint[1] = (yLen * rand() / RAND_MAX) - halfY;
   spawnPoint[2] = (zLen * rand() / RAND_MAX) - halfZ;

   return spawnPoint;
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 RandomActorGeneratorPlugin::RandomPointInsideSphere()
{
   //Method stolen from http://mathworld.wolfram.com/SphereLinePicking.html

   //First generate a random line segment inside the unit sphere.  This is
   //done by choosing the north pole as the first point (0,0,1) (NOTE: OSG's
   //sphere has has north pole at (0,0,0)! ) and using
   //sphere point picking on the unit sphere as the second point.  So:
   //
   //x = sqrt(1 - u^2) * cos(theta)
   //y = sqrt(1 - u^2) * sin(theta)
   //z = u
   //
   //Randomizing u and theta produce our point.  For the unit sphere, u is
   //between -1.0 and 1.0 and theta is between 0.0 and 2.0*PI
   //
   //Once the line segment is generated I just choose its center point for
   //the place where an actor is to be generated.

   osg::Vec3 spawnPoint;
   dtActors::VolumeEditActor* volEditActor = mMainWindow->GetVolumeEditActor();
   osg::Vec3 volEAScale = volEditActor->GetScale();   
   double baseRadius = volEditActor->GetBaseRadius(); 

   double u = ((2.0 * rand()) / RAND_MAX) - 1.0;
   double theta = ((2.0 * osg::PI * rand()) / RAND_MAX);

   //osg's unit Sphere has top at origin
   //osg::Vec3 firstPoint(0.0, 0.0, 0.0);  //not needed 0 + secondPoint is easy

   osg::Vec3 secondPoint;      
   double squareRoot = sqrt(1.0 - u*u);      
   secondPoint[0] = squareRoot * cos(theta);
   secondPoint[1] = squareRoot * sin(theta);
   secondPoint[2] = u;

   //We've got the two points on the line segment... now randomly pick a point on the line segment:
   double lineParam = (1.0 * rand()) / RAND_MAX;  //gives number between 0.0 and 1.0

   spawnPoint[0] = secondPoint[0] * lineParam; //firstPoint[0] is always 0.0
   spawnPoint[1] = secondPoint[1] * lineParam; //firstPoint[1] is always 0.0
   //spawnPoint[2] = (firstPoint[2] + secondPoint[2]) * lineParam;
   spawnPoint[2] = secondPoint[2] * lineParam; //firstPoint[2] is always 0.0

   //Need to scale based on scale matrix for the VolumeEditActor and the radius 
   spawnPoint[0] *= baseRadius * volEAScale[0];
   spawnPoint[1] *= baseRadius * volEAScale[1];
   spawnPoint[2] *= baseRadius * volEAScale[2];

   return spawnPoint;
}

namespace RandomActorGenerator
{
class STAGE_RANDOM_ACTOR_GENERATOR_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return RandomActorGeneratorPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Randomly creates a number of actors."; }

   virtual void GetDependencies(std::list<std::string>& deps) 
   {
      // just for testing
      deps.push_back("Plugin Manager");
   }

    /** construct the plugin and return a pointer to it */
   virtual dtEditQt::Plugin* Create(dtEditQt::MainWindow* mw) 
   { 
      mPlugin = new RandomActorGeneratorPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() 
   {
      delete mPlugin;
   }

private:

   dtEditQt::Plugin* mPlugin;
}; 
} //namespace RandomActorGenerator

extern "C" STAGE_RANDOM_ACTOR_GENERATOR_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new RandomActorGenerator::PluginFactory;
}
