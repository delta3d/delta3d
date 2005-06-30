#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

using namespace dtCore;
using namespace dtABC;

enum MotionModelType
{
   WALK = 0L,
   FLY,
   UFO,
   ORBIT,
   FPS
};

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
      AddDrawable( mTerrain.get() );

      RefPtr<WalkMotionModel> wmm = new WalkMotionModel( GetKeyboard(), GetMouse() );
      wmm->SetScene( GetScene() );
      mMotionModels.push_back( wmm.get() );

      mMotionModels.push_back( new FlyMotionModel( GetKeyboard(), GetMouse() ) );
      mMotionModels.push_back( new UFOMotionModel( GetKeyboard(), GetMouse() ) );
      mMotionModels.push_back( new OrbitMotionModel( GetKeyboard(), GetMouse() ) );
    
      RefPtr<FPSMotionModel> fmm = new FPSMotionModel( GetKeyboard(), GetMouse() );
      fmm->SetScene( GetScene() );
      mMotionModels.push_back( fmm.get() );
      
      for( int i = 0; i < mMotionModels.size(); i++ )
      {  
         mMotionModels[i]->SetTarget( GetCamera() );
      }

      SetMotionModel(WALK);
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
         SetMotionModel(WALK);
         break;
      case Producer::Key_2:
         Notify(ALWAYS,"Fly");
         SetMotionModel(FLY);
         break;
      case Producer::Key_3:
         Notify(ALWAYS,"UFO");
         SetMotionModel(UFO);
         break;
      case Producer::Key_4:
         Notify(ALWAYS,"Orbit");
         SetMotionModel(ORBIT);
         break;
      case Producer::Key_5:
         Notify(ALWAYS,"FPS");
         SetMotionModel(FPS);
         break;
      default:
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
   void SetMotionModel( int index )
   {
      for( int i = 0; i < mMotionModels.size(); i++ )
      {
         mMotionModels[i]->SetEnabled(i == index);
      }

      //turn off cursor for FPS motion model
      GetWindow()->ShowCursor(index != FPS); 
   }

   /**
   * The terrain object.
   */
   RefPtr<InfiniteTerrain> mTerrain;

   /**
   * The five motion models.
   */
   std::vector< RefPtr<MotionModel> > mMotionModels;
};

IMPLEMENT_MANAGEMENT_LAYER( TestMotionModelsApp )

int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testMotionModels/;" +
                        GetDeltaDataPathList()  );


   RefPtr<TestMotionModelsApp> app = new TestMotionModelsApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
