#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include "gui_fl/guimgr.h"

using namespace dtABC;
using namespace dtCore;

class TestWeatherApp : public dtABC::Application
{

   DECLARE_MANAGEMENT_LAYER( TestWeatherApp )

public:
   TestWeatherApp( std::string configFile = "config.xml" )
  : Application( configFile )
   {
      terr = new dtCore::InfiniteTerrain();
      terr->SetHorizontalScale(0.005);
      terr->SetVerticalScale(35.f);
      terr->SetSegmentDivisions(64);

      Transform trans = Transform( 0.0f, 0.0f, terr->GetVerticalScale() + 15.0f );
      GetCamera()->SetTransform(&trans);

      weather = new dtABC::Weather();   
      weather->AddChild( terr.get() );

      AddDrawable( weather->GetEnvironment());

      orbit = new dtCore::OrbitMotionModel( GetKeyboard(), GetMouse() );
      orbit->SetTarget( GetCamera() );

      GUI *ui = new GUI();

 }
	~TestWeatherApp()
   {
   }

protected:
   virtual void KeyPressed(dtCore::Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      switch(key) {
      case Producer::Key_Escape:    Quit();    	 break;

      case Producer::Key_F1: weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED);    break;
      case Producer::Key_F2: weather->SetBasicVisibilityType(Weather::VIS_FAR);    break;
      case Producer::Key_F3: weather->SetBasicVisibilityType(Weather::VIS_MODERATE); break;
      case Producer::Key_F4: weather->SetBasicVisibilityType(Weather::VIS_LIMITED);  break;
      case Producer::Key_F5: weather->SetBasicVisibilityType(Weather::VIS_CLOSE);    break;

      case Producer::Key_1: weather->SetBasicCloudType(Weather::CLOUD_CLEAR);      break;
      case Producer::Key_2: weather->SetBasicCloudType(Weather::CLOUD_FEW);        break;
      case Producer::Key_3: weather->SetBasicCloudType(Weather::CLOUD_SCATTERED);  break;
      case Producer::Key_4: weather->SetBasicCloudType(Weather::CLOUD_BROKEN);     break;
      case Producer::Key_5: weather->SetBasicCloudType(Weather::CLOUD_OVERCAST);  break;
      default:
         break;
      }
   }
	
private:
   RefPtr<InfiniteTerrain> terr;
   RefPtr<Weather> weather;
   RefPtr<OrbitMotionModel> orbit;

};

IMPLEMENT_MANAGEMENT_LAYER( TestWeatherApp )

int main(int argc, char* argv[])
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   RefPtr<TestWeatherApp> app = new TestWeatherApp( "config.xml" );
   app->Config();
   app->Run();

   return 0;
}
