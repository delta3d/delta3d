#include "testexport.h"
#include "testactorlibraryregistry.h"
#include "textlabelcomponent.h"
#include "componentgameactor.h"

#include <dtABC/labelactor.h>
#include <dtCore/globals.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtDAL/project.h>
#include <dtGame/baseinputcomponent.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameapplication.h>
#include <dtGame/gameentrypoint.h>

static const osg::Vec3 TARGET_XYZ(0, 6, 5);

// for key input handling
class TestInput : public dtGame::BaseInputComponent
{
public:

   TestInput() : dtGame::BaseInputComponent("TestInput") { }

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
            typedef std::vector<dtDAL::ActorProxy*> PLIST;
            PLIST proxies;
            GetGameManager()->FindActorsByType(*TestActorLibraryRegistry::COMPONENT_GAME_ACTOR_TYPE.get(), proxies);

            // for each game actor
            for(PLIST::iterator i = proxies.begin(); i != proxies.end(); ++i)
            {
               // get the game actor
               ComponentGameActor* actor;
               (*i)->GetActor(actor);

               // get its text label component
               TextLabelComponent* tcomp;
               actor->GetComponent(tcomp);

               // access text label component
               tcomp->SetFlashing(!tcomp->IsFlashing());
               tcomp->SetFlashInterval(0.2f);
            }
            return true;
         }
      }
      return false;
   }
};


class TEST_GA_COMPONENTS_EXPORT TestGameActorComponents : public dtGame::GameEntryPoint
{
public:

   TestGameActorComponents() {}
   
   virtual void Initialize(dtGame::GameApplication& app, int argc, char** argv)
   {
      // add a motion model
      dtCore::OrbitMotionModel* motionModel = new dtCore::OrbitMotionModel(app.GetKeyboard(), app.GetMouse());
      motionModel->SetTarget(app.GetCamera());
      motionModel->SetDistance(20.f);
      motionModel->SetFocalPoint(TARGET_XYZ);
   }


   virtual void OnStartup(dtGame::GameApplication& app)
   {
      std::string dataPath = dtCore::GetDeltaDataPathList();
      dtCore::SetDataFilePathList(dataPath + ";" + dtCore::GetDeltaRootPath() + "/examples/data"); 

      dtGame::GameManager& gameManager = *app.GetGameManager();
      gameManager.LoadActorRegistry("testGameActorComponents");

      try
      {
       
         // Get the screen size
         app.GetWindow()->SetWindowTitle("TestGameActorComponents");

         std::string context = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";
         dtDAL::Project::GetInstance().SetContext(context, true);

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
      dtCore::RefPtr<ComponentGameActorProxy> proxy;
      app.GetGameManager()->CreateActor(*TestActorLibraryRegistry::COMPONENT_GAME_ACTOR_TYPE.get(), proxy);
      
      // set mesh property and translation of component game actor
      ComponentGameActor* actor;
      proxy->GetActor(actor);
      actor->SetMesh("models/physics_happy_sphere.ive");
      dtCore::Transform xform;
      xform.SetTranslation(TARGET_XYZ);
      actor->SetTransform(xform);

      // access text label component of actor
      TextLabelComponent* textcomp;
      actor->GetComponent(textcomp);
      textcomp->SetText("LOOK AT MEEEE!!!!!!!");
      textcomp->SetHeight(1);
      textcomp->SetFontSize(1);

      // WARNING: if you call AddACtor without the boolean arguments,
      // the OnEnteredWorld method of the game actor is not called!
      // This sucks!
      app.GetGameManager()->AddActor(*proxy.get(),false, false);

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

