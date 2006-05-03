#include <dtCore/dt.h>
#include <dtABC/dtabc.h>
#include <dtInspector/inspector.h>

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

      dtInspector::Inspector *ui = new dtInspector::Inspector();
      ui = ui; //no-op to prevent warning for unused variable

 }
	~TestWeatherApp()
   {
   }

protected:
   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      bool verdict(false);
      switch(key)
      {
      case Producer::Key_Escape:    Quit();    	 verdict=true;  break;

      case Producer::Key_F1: weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED);    verdict=true;  break;
      case Producer::Key_F2: weather->SetBasicVisibilityType(Weather::VIS_FAR);    verdict=true;  break;
      case Producer::Key_F3: weather->SetBasicVisibilityType(Weather::VIS_MODERATE); verdict=true;  break;
      case Producer::Key_F4: weather->SetBasicVisibilityType(Weather::VIS_LIMITED);  verdict=true;  break;
      case Producer::Key_F5: weather->SetBasicVisibilityType(Weather::VIS_CLOSE);    verdict=true;  break;

      case Producer::Key_1: weather->SetBasicCloudType(Weather::CLOUD_CLEAR);      verdict=true;  break;
      case Producer::Key_2: weather->SetBasicCloudType(Weather::CLOUD_FEW);        verdict=true;  break;
      case Producer::Key_3: weather->SetBasicCloudType(Weather::CLOUD_SCATTERED);  verdict=true;  break;
      case Producer::Key_4: weather->SetBasicCloudType(Weather::CLOUD_BROKEN);     verdict=true;  break;
      case Producer::Key_5: weather->SetBasicCloudType(Weather::CLOUD_OVERCAST);  verdict=true;  break;
      default:
         break;
      }

      return verdict;
   }

private:
   RefPtr<InfiniteTerrain> terr;
   RefPtr<Weather> weather;
   RefPtr<OrbitMotionModel> orbit;

};

IMPLEMENT_MANAGEMENT_LAYER( TestWeatherApp )

int main(int argc, char* argv[])
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testWeather/;" +
                        GetDeltaDataPathList()  );

   RefPtr<TestWeatherApp> app = new TestWeatherApp( "config.xml" );
   app->Config();
   app->Run();

   return 0;
}
