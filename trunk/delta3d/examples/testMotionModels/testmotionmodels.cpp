#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;

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
   }

   virtual void TestMotionModelsApp::Config()
   {
      //make sure to call the Base Config() as well.
      Application::Config();

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

      Transform xform( 0.0f, 0.0f, mTerrain->GetVerticalScale() + 15.0f );
      GetCamera()->SetTransform( &xform );

      for( unsigned int i = 0; i < mMotionModels.size(); i++ )
      {  
         mMotionModels[i]->SetTarget( GetCamera() );
      }

      SetMotionModel(WALK);
      LOG_ALWAYS("Walk");
   }

protected:

   /**
   * Key press callback.
   *
   * @param keyboard the keyboard object generating the event
   * @param key the key pressed
   * @param character the corresponding character
   */
   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      bool verdict = Application::KeyPressed(keyboard, key, character);
      if( verdict == true )
      {
         return verdict;
      }

      switch(key)
      {
      case Producer::Key_1:
         LOG_ALWAYS("Walk");
         SetMotionModel(WALK);
         verdict = true;
         break;
      case Producer::Key_2:
         LOG_ALWAYS("Fly");
         SetMotionModel(FLY);
         verdict = true;
         break;
      case Producer::Key_3:
         LOG_ALWAYS("UFO");
         SetMotionModel(UFO);
         verdict = true;
         break;
      case Producer::Key_4:
         LOG_ALWAYS("Orbit");
         SetMotionModel(ORBIT);
         verdict = true;
         break;
      case Producer::Key_5:
         LOG_ALWAYS("FPS");
         SetMotionModel(FPS);
         verdict = true;
         break;
      default:
         break;
      }

      return verdict;
   }


private:

   /**
   * Enables one of the four motion models and disables
   * the rest.
   *
   * @param index the index of the motion model to enable
   */
   void SetMotionModel( unsigned int index )
   {
      for( unsigned int i = 0; i < mMotionModels.size(); i++ )
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
