#include "testexport.h"
#include "testactorlibraryregistry.h"
#include "textlabelcomponent.h"
#include "componentgameactor.h"

#include <dtABC/labelactor.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/object.h>
#include <dtCore/project.h>
#include <dtUtil/datapathutils.h>
#include <dtGame/baseinputcomponent.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/gameentrypoint.h>

static const osg::Vec3 TARGET_XYZ(0, 6, 5);

// for key input handling
class TestInput : public dtGame::BaseInputComponent
{
public:
   static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;

   TestInput() : dtGame::BaseInputComponent(*TYPE) { }

   virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      switch (key)
      {
         case osgGA::GUIEventAdapter::KEY_Escape:
         {

            GetGameManager()->GetApplication().Quit();
            return true;

         }
         case osgGA::GUIEventAdapter::KEY_Space:
         {
            // loop through all component game actors and
            // get their text label components.
            // Toggle flashing property on components
            typedef dtCore::ActorPtrVector ProxyList;

            ProxyList proxies;
            GetGameManager()->FindActorsByType(*TestActorLibraryRegistry::COMPONENT_GAME_ACTOR_TYPE.get(), proxies);

            // for each game actor
            for(ProxyList::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); ++proxyIter)
            {
               // get the game actor
               ComponentGameActor* actor;
               (*proxyIter)->GetDrawable(actor);

               // get its text label component
               TextLabelComponent* textComp;
               actor->GetComponent(textComp);

               // access text label component
               textComp->SetFlashing(!textComp->IsFlashing());
               textComp->SetFlashInterval(0.2f);
            }
            return true;
         }
      }
      return false;
   }
};

const dtCore::RefPtr<dtCore::SystemComponentType> TestInput::TYPE(new dtCore::SystemComponentType("TestInput","GMComponents", "",dtGame::GMComponent::BaseGMComponentType));


class TEST_GA_COMPONENTS_EXPORT TestGameActorComponents : public dtGame::GameEntryPoint
{
public:

   TestGameActorComponents() {}
   
   virtual void Initialize(dtABC::BaseABC& app, int argc, char** argv)
   {
      // add a motion model
      dtCore::OrbitMotionModel* motionModel = new dtCore::OrbitMotionModel(app.GetKeyboard(), app.GetMouse());
      motionModel->SetTarget(app.GetCamera());
      motionModel->SetDistance(20.0f);
      motionModel->SetFocalPoint(TARGET_XYZ);
   }

   virtual void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager)
   {
      std::string dataPath = dtUtil::GetDeltaDataPathList();
      dtUtil::SetDataFilePathList(dataPath + ";" + dtUtil::GetDeltaRootPath() + "/examples/data"); 

      gameManager.LoadActorRegistry("testGameActorComponents");

      try
      {
         // Get the screen size
         app.GetWindow()->SetWindowTitle("TestGameActorComponents");

         std::string context = dtUtil::GetDeltaRootPath() + "/examples/data";
         dtCore::Project::GetInstance().SetContext(context, true);

         gameManager.ChangeMap("MyCoolMap");
      }
      catch (dtUtil::Exception& e)
      {
         LOG_ERROR("Can't find the project context or load the map. Exception follows.");
         e.LogException(dtUtil::Log::LOG_ERROR);
      }

      // add the input handler
      gameManager.AddComponent(*new TestInput(), dtGame::GameManager::ComponentPriority::NORMAL);

      // create help label
      dtABC::LabelActor* label = new dtABC::LabelActor();
      osg::Vec2 testSize(32.0f, 2.5f);
      label->SetBackSize(testSize);
      label->SetFontSize(0.8f);
      label->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
      label->SetText("Press space to let label blink");
      label->SetEnableDepthTesting(false);
      label->SetEnableLighting(false);
      app.GetCamera()->AddChild(label);
      dtCore::Transform labelOffset(-17.0f, 50.0f, 10.5f, 0.0f, 90.0f, 0.0f);
      label->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
      app.AddDrawable(app.GetCamera());

      // make sure map is loaded
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      // create a component game actor
      dtCore::RefPtr<ComponentGameActor> actor;
      gameManager.CreateActor(*TestActorLibraryRegistry::COMPONENT_GAME_ACTOR_TYPE, actor);
      
      // set mesh property and translation of component game actor
      actor->GetDrawable<dtCore::Object>()->SetMeshResource(dtCore::ResourceDescriptor("StaticMeshes:physics_happy_sphere.ive"));
      dtCore::Transform xform;
      xform.SetTranslation(TARGET_XYZ);
      actor->GetDrawable<dtCore::Object>()->SetTransform(xform);

      // access text label component of actor
      TextLabelComponent* textcomp;
      actor->GetComponent(textcomp);
      textcomp->SetText("LOOK AT MEEEE!!!!!!!");
      textcomp->SetHeight(1);
      textcomp->SetFontSize(1);

      // WARNING: if you call AddACtor without the boolean arguments,
      // the OnEnteredWorld method of the game actor is not called!
      gameManager.AddActor(*actor,false, false);
   }

};

extern "C" TEST_GA_COMPONENTS_EXPORT dtGame::GameEntryPoint* CreateGameEntryPoint()
{
   return new TestGameActorComponents;
}

//////////////////////////////////////////////////////////////////////////
extern "C" TEST_GA_COMPONENTS_EXPORT void DestroyGameEntryPoint(dtGame::GameEntryPoint* entryPoint)
{
   delete entryPoint;
}

