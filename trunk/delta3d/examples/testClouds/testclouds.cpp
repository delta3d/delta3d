#include <dtCore/dt.h>
#include <dtABC/dtabc.h>
#include <dtInspector/inspector.h>

using namespace dtABC;
using namespace dtCore;

class TestCloudsApp : public Application
{

DECLARE_MANAGEMENT_LAYER( TestCloudsApp )

public:
   TestCloudsApp( std::string configFilename = "config.xml" )
      : Application( configFilename )
   {
      terr = new InfiniteTerrain();
      terr->SetHorizontalScale(0.01f);
      terr->SetVerticalScale(25.f);
      terr->Regenerate();

      LOG_ALWAYS("Creating clouds...");
      cd = new CloudDome(6, 2, 0.7, 0.5, 0.7, 5, 5500.f, 20);
      cp[0] = new CloudPlane(6, 0.5, 6, 1, .3, 0.96, 256, 1800);
      cp[1] = new CloudPlane(6, 0.7, 12, 1, .4, 0.95, 512, 1000);
      cp[2] = new CloudPlane(6, 0.8, 20, 1, .2, 0.96, 512, 600);
      LOG_ALWAYS("...done creating clouds.");

      weather = new Weather();   
      weather->AddChild(terr.get());

      cloudLayers = 1;
      isDomeEnabled = false;
      weather->GetEnvironment()->AddEffect(cp[0].get());
      this->AddDrawable(weather->GetEnvironment());

      Transform xform(0.f, 00.f, 30.f, 0.f, 10.f, 0.f);
      GetCamera()->SetTransform(xform);

      orbit = new OrbitMotionModel( GetKeyboard(), GetMouse() );
      orbit->SetTarget(GetCamera());
   }

   ~TestCloudsApp()
   {
   }

protected:
   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character)
   {
      bool verdict(false);
      switch(key)
      {
      case Producer::Key_Escape:
         {
            this->Quit();
            verdict = true;
         } break;
      case Producer::Key_H:
         {
            dtInspector::Inspector *ui = new dtInspector::Inspector();
            ui->Show(); //to prevent unused variable warnings
            verdict = true;
         } break;
      case Producer::Key_F1: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED); break;
      case Producer::Key_F2: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_FAR);       break;
      case Producer::Key_F3: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_MODERATE);  break;
      case Producer::Key_F4: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_LIMITED);   break;
      case Producer::Key_F5: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_CLOSE);     break;
      case Producer::Key_P:
         {
            if(isDomeEnabled)
            {
               for(int i = 0; i < cloudLayers; ++i)
                  weather->GetEnvironment()->AddEffect(cp[i].get());

               weather->GetEnvironment()->RemEffect(cd.get());
               isDomeEnabled = false;
            }
            verdict = true;
         } break;
      case Producer::Key_D:
         if(!isDomeEnabled)
         {
            weather->GetEnvironment()->AddEffect(cd.get());
            isDomeEnabled = true;
            for(int i = 0; i < cloudLayers; ++i)
               weather->GetEnvironment()->RemEffect(cp[i].get());
            verdict = true;
         } break;
      case Producer::Key_KP_Add:
      case Producer::Key_equal:
         if (!isDomeEnabled && cloudLayers >= 0 && cloudLayers < 3)
         {	
            weather->GetEnvironment()->AddEffect(cp[cloudLayers].get());
            ++cloudLayers;
            verdict = true;
         } break;
      case Producer::Key_KP_Subtract:
      case Producer::Key_minus:
         if (!isDomeEnabled && cloudLayers > 0)
         {
            --cloudLayers;
            weather->GetEnvironment()->RemEffect(cp[cloudLayers].get());
            verdict = true;
         } break;
      default:
         break;
      }
      return verdict;
   }

private:
   RefPtr<InfiniteTerrain> terr;
   RefPtr<Weather> weather;
   RefPtr<OrbitMotionModel> orbit;

   RefPtr<dtCore::CloudDome>  cd;
   RefPtr<dtCore::CloudPlane> cp[3];
   int cloudLayers;
   bool isDomeEnabled;

};

IMPLEMENT_MANAGEMENT_LAYER( TestCloudsApp )

int main(int argc, char* argv[])
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" + 
      dtCore::GetDeltaRootPath() + "/examples/data" + ";" +
      dataPath + "/gui");

   RefPtr<TestCloudsApp> app = new TestCloudsApp( "config.xml" );
   app->Config();
   app->Run();


   return 0;
}
