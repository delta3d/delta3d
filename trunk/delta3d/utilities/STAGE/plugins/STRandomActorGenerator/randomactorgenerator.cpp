#include "randomactorgenerator.h"

#include <ctime>

#include <dtActors/volumeeditactor.h>
#include <dtActors/gamemeshactor.h>

#include <dtCore/isector.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/undomanager.h>
#include <dtUtil/fileutils.h>

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QSpinBox>

const std::string RandomActorGeneratorPlugin::PLUGIN_NAME = "Random Actor Generator";

////////////////////////////////////////////////////////////////////////////////
RandomActorGeneratorPlugin::RandomActorGeneratorPlugin(dtEditQt::MainWindow* mw)
   : mUI()
   , mMainWindow(mw)
{
   // apply layout made with QT designer
   mUI.setupUi(this);

   // add dock widget to STAGE main window
   mw->addDockWidget(Qt::RightDockWidgetArea, this);

   connect(mUI.mGenerateBtn, SIGNAL(clicked()), this, SLOT(OnGeneratePushed()));
   //May want to put this back later
   //connect(mUI.mRefreshBtn, SIGNAL(clicked()), this, SLOT(OnRefreshActorList()));

   connect(&(dtEditQt::EditorEvents::GetInstance()), SIGNAL(selectedActors(ActorProxyRefPtrVector&)),
            this, SLOT(OnSelectedActorChange(ActorProxyRefPtrVector&)));

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
   //get the VolumeEdit "Actor" from STAGE.  This is the area to which the
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

   //Don't want to just make a bunch of copies of the volume editor:
   if(selectionList[0]->GetActor() == volEditActor)
   {
      QMessageBox::warning(this, tr("No actor selected."),
         tr("Please select an actor to be generated."), QMessageBox::Ok);

      return;
   }

   dtEditQt::EditorData::GetInstance().getUndoManager().beginMultipleUndo();
   size_t index;
   for (int i = 0; i < mUI.mNumActorsToGenerate->value(); ++i)
   {
      //Randomly pick from the selectionList which ActorProxy to generate
      //(e.g. if there is a tree Actor, a bush Actor, and a shrub,
      // randomly pick which one gets generated next)
      index = (int)( ((double) rand() / ((double)RAND_MAX + double(1)))
                      * selectionList.size());
      //previous line generates an int between 0 and (selectionList.size() - 1)

      NewActorProxyInsideVolumeEditor(selectionList[index].get());
   }
   dtEditQt::EditorData::GetInstance().getUndoManager().endMultipleUndo();

   dtEditQt::ViewportManager::GetInstance().refreshAllViewports();
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
void RandomActorGeneratorPlugin::OnSelectedActorChange(ActorProxyRefPtrVector& actors)
{
   if (!actors.empty())
   {
      //ensure we can't duplicate the STAGE Brush
      if (! dynamic_cast<dtActors::VolumeEditActorProxy*>(actors[0].get()))
      {
         mUI.mGenerateBtn->setEnabled(true);
         return;
      }
   }

   mUI.mGenerateBtn->setEnabled(false);
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

   //Add cloned actor to map (with a unique name)
   currMap->AddProxy(*aCloneProxy, true);

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

   //apply random rotation on the specified axes
   float randAngle;
   if (mUI.mRandomXRotationCheckBox->isChecked()) //X-Axis
   {
      //get a random angle (between 0 and 360 degrees.
      randAngle = (360.0f * rand()) / RAND_MAX;
      p += randAngle;
   }
   if (mUI.mRandomYRotationCheckBox->isChecked()) //Y-Axis
   {
      randAngle = (360.0f * rand()) / RAND_MAX;
      r += randAngle;
   }
   if (mUI.mRandomZRotationCheckBox->isChecked()) //Z-Axis
   {
      randAngle = (360.0f * rand()) / RAND_MAX;
      h += randAngle;
   }
   cloneXForm.SetRotation(h,p,r);

   //Applying translation and roation.
   aClonePtr->SetTransform(cloneXForm);

   //random scale?
   if (mUI.mRandomScaleCheckBox->isChecked())
   {
      //random scale between scaleMin and scaleMax
      float randScale = ((mUI.mScaleMaxSpinBox->value() * rand()) / RAND_MAX)
                           + mUI.mScaleMinSpinBox->value();

      dtDAL::ActorProperty* prop = aCloneProxy->GetProperty("Scale");
      dtDAL::Vec3ActorProperty* scaleProp = dynamic_cast<dtDAL::Vec3ActorProperty*>(prop);

      if (scaleProp) //scaling is possible with this actor
      {
         osg::Vec3 newScale(randScale, randScale, randScale);

         // It causes problems when the scale is negative in the current version of OSG
         if (newScale.x() < 0.0001f)
         {
            newScale.x() = 0.0001f;
         }
         if (newScale.y() < 0.0001f)
         {
            newScale.y() = 0.0001f;
         }
         if (newScale.z() < 0.0001f)
         {
            newScale.z() = 0.0001f;
         }

         scaleProp->SetValue(newScale);
      }
   }

   //Apply rotation and translation from VolEditActor (remember: scale doesn't
   //exist inside Delta Transforms.... if that ever changes this could break)
   osg::Matrixd volEAXMatrix;
   volEAXMatrix = volEditActor->GetMatrix();
   aClonePtr->SetMatrix(aClonePtr->GetMatrix() * volEAXMatrix);

   if (mUI.mGroundClampCheckBox->checkState())
   {
      aClonePtr->GetTransform(cloneXForm);
      osg::Vec3 translatedSpawnPoint = cloneXForm.GetTranslation();
      osg::Vec3 startPoint;

      dtCore::Scene* masterScene = dtEditQt::ViewportManager::GetInstance().getMasterScene();
      //Don't want the new Actor's Drawable to be part of the intersection tests
      masterScene->RemoveDrawable(aClonePtr);

      //first find "top" of brush
      dtCore::RefPtr<dtCore::Isector> groundFinder = new dtCore::Isector();
      groundFinder->SetGeometry(dtEditQt::EditorData::GetInstance().getMainWindow()->GetVolumeEditActor());
      groundFinder->SetStartPosition(translatedSpawnPoint);
      groundFinder->SetDirection(osg::Vec3(0.0, 0.0, 1.0));
      groundFinder->Update();
      if (groundFinder->GetNumberOfHits() > 0)
      {
         groundFinder->GetHitPoint(startPoint, 0);
      }
      else
      {
         startPoint = translatedSpawnPoint;
      }

      //Now go from startPoint to the first poly "below" it
      groundFinder->SetScene(masterScene);
      groundFinder->SetGeometry(NULL);
      groundFinder->SetStartPosition(startPoint);
      //Down in Delta3D is -z so we're going to clamp in that direction
      //(clamp towards toward the "ground."
      groundFinder->SetDirection(osg::Vec3(0.0, 0.0, -1.0));

      groundFinder->Update();
      osg::Vec3 groundHit;
      for (int i = 0; i < groundFinder->GetNumberOfHits(); ++i)
      {
         groundFinder->GetHitPoint(groundHit, i);

         //Take first hit point that is far enough away from the top of the brush
         //(experiments have shown that the first 1 or 2 points are usually the
         // top of the brush)
         if (startPoint[2] - groundHit[2] >= 0.0001)
         {
            cloneXForm.SetTranslation(groundHit);
            aClonePtr->SetTransform(cloneXForm);
            break;
         }
      }

      //put new Actor's Drawable back in Scene
      masterScene->AddDrawable(aClonePtr);
   }

   //send out a created event.
   dtEditQt::EditorEvents::GetInstance().emitActorProxyCreated(aCloneProxy, false);
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
