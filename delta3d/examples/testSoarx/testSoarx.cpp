#include "testSoarx.h"

using namespace dtCore;
using namespace dtABC;
using namespace dtSOARX;
using namespace std;



TestTerrainApp::TestTerrainApp( std::string configFile )
:  Application( configFile ),
mWireframe(false),
mSOARXTerrain(NULL)
{}

TestTerrainApp::~TestTerrainApp()
{
}


void TestTerrainApp::Config()
{
   mEnvironment = new Environment();
   mEnvironment->SetVisibility(30000.f);
   mEnvironment->SetDateTime(2005, 5, 5, 14, 0, 0);
   GetScene()->AddDrawable(mEnvironment.get());

   RefPtr<SkyDome> dome = new SkyDome();
   mEnvironment->AddEffect( dome.get() );

   mSOARXTerrain = new SOARXTerrain();
   mSOARXTerrain->LoadConfiguration("soarxterrain.xml");
   mEnvironment->AddChild(mSOARXTerrain.get());

   mMotionModel = new FlyMotionModel( GetKeyboard(), GetMouse() );
   mMotionModel->SetTarget( GetCamera() );

   Transform transform(0, 0, 1000.0);  // establish initial starting position
   GetCamera()->SetTransform(&transform);

   Application::Config();
}


/**
* Pre-frame callback.
*
* @param deltaFrameTime the amount of time elapsed since the last frame
*/
void TestTerrainApp::PreFrame(const double deltaFrameTime)
{
   if(GetKeyboard()->GetKeyState(Producer::Key_minus))
   {
      mSOARXTerrain->SetThreshold(
         clamp((float)(mSOARXTerrain->GetThreshold() - deltaFrameTime*5.0), 1.0f, 10.0f)
         );
      Notify(ALWAYS, "Threshold decreased to %5.2f", mSOARXTerrain->GetThreshold());
   }

   if(GetKeyboard()->GetKeyState(Producer::Key_equal))
   {
      mSOARXTerrain->SetThreshold(
         clamp((float)(mSOARXTerrain->GetThreshold() + deltaFrameTime*5.0), 1.0f, 10.0f)
         );
      Notify(ALWAYS, "Threshold increased to %5.2f", mSOARXTerrain->GetThreshold());
   }

   if(GetKeyboard()->GetKeyState(Producer::Key_bracketleft))
   {
      mSOARXTerrain->SetDetailMultiplier(
         clamp((float)(mSOARXTerrain->GetDetailMultiplier() - deltaFrameTime*5.0), 1.0f, 20.0f)
         );
      Notify(ALWAYS, "Detail decreased to %5.2f", mSOARXTerrain->GetDetailMultiplier());
   }

   if(GetKeyboard()->GetKeyState(Producer::Key_bracketright))
   {
      mSOARXTerrain->SetDetailMultiplier(
         clamp((float)(mSOARXTerrain->GetDetailMultiplier() + deltaFrameTime*5.0), 1.0f, 20.0f)
         );
      Notify(ALWAYS, "Detail increased to %5.2f", mSOARXTerrain->GetDetailMultiplier());
   }
}

/**
* KeyboardListener override
* Called when a key is pressed.
*
* @param keyboard the source of the event
* @param key the key pressed
* @param character the corresponding character
*/
void TestTerrainApp::KeyPressed(dtCore::Keyboard* keyboard,
                                Producer::KeyboardKey key,
                                Producer::KeyCharacter character)
{
   switch(key)
   {

   case Producer::Key_space:
      mWireframe = !mWireframe;

      if (mWireframe)
         GetScene()->SetRenderState(Scene::FRONT_AND_BACK, Scene::LINE); 
      else
         GetScene()->SetRenderState(Scene::FRONT, Scene::FILL); 

      break;

   case Producer::Key_Return:
      GetCamera()->SetNextStatisticsType();
      break;

   case Producer::Key_Shift_L:
   case Producer::Key_Shift_R:
      mMotionModel->SetMaximumFlySpeed( 1000.f );
         break;

   case Producer::Key_Escape:
      System::Instance()->Stop();
      break;

   default: break;
   }
}



void TestTerrainApp::KeyReleased( dtCore::Keyboard*       keyboard, 
                                          Producer::KeyboardKey   key,
                                          Producer::KeyCharacter  character)
{
   switch(key)
   {
   case Producer::Key_Shift_R:
   case Producer::Key_Shift_L:
      mMotionModel->SetMaximumFlySpeed( 100.f );
      break;

   default: break;
   }
}
