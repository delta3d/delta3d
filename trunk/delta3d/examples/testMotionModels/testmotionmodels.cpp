#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

using namespace dtCore;
using namespace dtABC;

/**
* The motion model test application.
*/
class TestMotionModelsApp : public Application
{
   DECLARE_MANAGEMENT_LAYER( TestMotionModelsApp )

public:

   /**
   * Constructor.
   */
   TestMotionModelsApp( std::string configFile = "config.xml" )
      : Application( configFile )
   {

      mTerrain = new InfiniteTerrain;
      AddDrawable(mTerrain.get());

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
      Notify(ALWAYS,"Walk");
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
         Notify(ALWAYS,"Walk");
         SetMotionModel(0);
         break;

      case Producer::Key_2:
         Notify(ALWAYS,"Fly");
         SetMotionModel(1);
         break;

      case Producer::Key_3:
         Notify(ALWAYS,"UFO");
         SetMotionModel(2);
         break;

      case Producer::Key_4:
         Notify(ALWAYS,"Orbit");
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
   RefPtr<InfiniteTerrain> mTerrain;

   /**
   * The four motion models.
   */
   RefPtr<MotionModel> mMotionModels[4];
};

IMPLEMENT_MANAGEMENT_LAYER( TestMotionModelsApp )

int main( int argc, char **argv )
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   TestMotionModelsApp* app = new TestMotionModelsApp( "config.xml" );

   app->Config();
   app->Run();

   delete app;

   return 0;
}
