#include <dtInspector/inspector.h>

#include <dtCore/environment.h>
#include <dtCore/globals.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/refptr.h>

#include <dtABC/application.h>
#include <dtABC/weather.h>

using namespace dtABC;
using namespace dtCore;

class TestWeatherApp : public dtABC::Application
{

   DECLARE_MANAGEMENT_LAYER( TestWeatherApp )

public:
   TestWeatherApp( const std::string& configFile = "config.xml" ) : Application( configFile )
   {
      terr = new dtCore::InfiniteTerrain();
      terr->SetHorizontalScale(0.005);
      terr->SetVerticalScale(35.f);
      terr->SetSegmentDivisions(64);

      Transform trans = Transform( 0.0f, 0.0f, terr->GetVerticalScale() + 15.0f );
      GetCamera()->SetTransform(trans);

      weather = new dtABC::Weather();   
      weather->AddChild( terr.get() );

      AddDrawable( weather->GetEnvironment());

      orbit = new dtCore::OrbitMotionModel( GetKeyboard(), GetMouse() );
      orbit->SetTarget( GetCamera() );

      dtInspector::Inspector *ui = new dtInspector::Inspector();
      ui = ui; //no-op to prevent warning for unused variable
   }
protected:
	virtual ~TestWeatherApp() {}

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      bool verdict(false);
      switch(key)
      {
      case osgGA::GUIEventAdapter::KEY_Escape:    Quit();    	 verdict=true;  break;

      case osgGA::GUIEventAdapter::KEY_F1: weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED);    verdict=true;  break;
      case osgGA::GUIEventAdapter::KEY_F2: weather->SetBasicVisibilityType(Weather::VIS_FAR);    verdict=true;  break;
      case osgGA::GUIEventAdapter::KEY_F3: weather->SetBasicVisibilityType(Weather::VIS_MODERATE); verdict=true;  break;
      case osgGA::GUIEventAdapter::KEY_F4: weather->SetBasicVisibilityType(Weather::VIS_LIMITED);  verdict=true;  break;
      case osgGA::GUIEventAdapter::KEY_F5: weather->SetBasicVisibilityType(Weather::VIS_CLOSE);    verdict=true;  break;

      case '1': weather->SetBasicCloudType(Weather::CLOUD_CLEAR);      verdict=true;  break;
      case '2': weather->SetBasicCloudType(Weather::CLOUD_FEW);        verdict=true;  break;
      case '3': weather->SetBasicCloudType(Weather::CLOUD_SCATTERED);  verdict=true;  break;
      case '4': weather->SetBasicCloudType(Weather::CLOUD_BROKEN);     verdict=true;  break;
      case '5': weather->SetBasicCloudType(Weather::CLOUD_OVERCAST);  verdict=true;  break;
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

int main()
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" +
      dtCore::GetDeltaRootPath() + "/examples/testWeather" + ";");

   RefPtr<TestWeatherApp> app = new TestWeatherApp( "config.xml" );
   app->Config();
   app->Run();

   return 0;
}
