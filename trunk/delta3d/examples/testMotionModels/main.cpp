#include "dt.h"
#include "dtabc.h"

/*
#include "application.h"
#include "globals.h"
#include "object.h"
#include "notify.h"
#include "infiniteterrain.h"
#include "flymotionmodel.h"
#include "orbitmotionmodel.h"
#include "ufomotionmodel.h"
#include "walkmotionmodel.h"*/


using namespace dtCore;
using namespace dtABC;

/**
 * The motion model test application.
 */
class TestMotionModelsApplication : public Application
{
   DECLARE_MANAGEMENT_LAYER(TestMotionModelsApplication)
   
   
   public:
   
      /**
       * Constructor.
       */
      TestMotionModelsApplication()
         : Application("config.xml")
      {
         SetDataFilePathList("../../data/");
         
         mTerrain = new InfiniteTerrain;
         
         GetScene()->AddDrawable(mTerrain.get());
         
         mObject = new Object;
         
         mObject->LoadFile("cessna.osg");
         
         Transform transform(0.0f, 100.0f, 0.0f);
         
         mObject->SetTransform(&transform);
         
         GetScene()->AddDrawable(mObject.get());
         
         WalkMotionModel* wmm;
         
         mMotionModels[0] = wmm = new WalkMotionModel(
            GetKeyboard(),
            GetMouse()
         );
         
         wmm->SetScene(GetScene());
         
         mMotionModels[1] = new FlyMotionModel(
            GetKeyboard(),
            GetMouse()
         );
         
         mMotionModels[2] = new UFOMotionModel(
            GetKeyboard(),
            GetMouse()
         );
         
         mMotionModels[3] = new OrbitMotionModel(
            GetKeyboard(),
            GetMouse()
         );
         
         for(int i=0;i<4;i++)
         {  
            mMotionModels[i]->SetTarget(GetCamera());
         }
         
         SetMotionModel(0);
         
         GetWindow()->SetWindowTitle("1: walk, 2: fly, 3: ufo, 4: orbit");
      }
   
      
   protected:
      
      /**
       * Key press callback.
       *
       * @param keyboard the keyboard object generating the event
       * @param key the key pressed
       * @param character the corresponding character
       */
      virtual void KeyPressed(dtCore::Keyboard* keyboard, 
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character)
      {
         Application::KeyPressed(keyboard, key, character);
         
         switch(key)
         {
            case Producer::Key_1:
               SetMotionModel(0);
               break;
               
            case Producer::Key_2:
               SetMotionModel(1);
               break;
               
            case Producer::Key_3:
               SetMotionModel(2);
               break;
               
            case Producer::Key_4:
               SetMotionModel(3);
               break;
         }
      }
   
   
   private:
   
      /**
       * Enables one of the four motion models and disables
       * the rest.
       *
       * @param index the index of the motion model to enable
       */
      void SetMotionModel(int index)
      {
         for(int i=0;i<4;i++)
         {
            mMotionModels[i]->SetEnabled(i == index);
         }
      }
      
      /**
       * The terrain object.
       */
      osg::ref_ptr<InfiniteTerrain> mTerrain;
      
      /**
       * A test object.
       */
      osg::ref_ptr<Object> mObject;
      
      /**
       * The four motion models.
       */
      osg::ref_ptr<MotionModel> mMotionModels[4];
};

IMPLEMENT_MANAGEMENT_LAYER(TestMotionModelsApplication)

int main( int argc, char **argv )
{
   TestMotionModelsApplication* testMotionModelsApp =
      new TestMotionModelsApplication;
   
   testMotionModelsApp->Config();
   testMotionModelsApp->Run();

   return 0;
}