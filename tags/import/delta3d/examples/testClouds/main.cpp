#include "camera.h"
#include "globals.h"
#include "notify.h"

#include "joystick.h"
#include "keyboard.h"
#include "logicalinputdevice.h"
#include "mouse.h"
#include "tracker.h"

#include "guimgr.h"
#include "system.h"

#include "environment.h"
#include "skydome.h"
#include "cloudplane.h"
#include "cloudbillboards.h"

#include <osg/Image>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <osg/BlendFunc>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgNVCg/Context>
#include <osgNVCg/Program>
#include <osgNVCg/VectorParameter>
#include <osgNVCg/TextureParameter>
#include <osgNVCg/StateMatrixParameter>

#include "application.h"
#include "infiniteterrain.h"
#include "weather.h"

using namespace P51;

const int SIZE_X = 100;
const int SIZE_Y = 100;

const int MAX_HEIGHT = 100;
const int HEIGHT_DIFF = 10;

using namespace P51ABC;


class CloudApp : public P51ABC::Application
{

public:
	CloudApp()
   {
      terr = new P51::InfiniteTerrain();
      terr->SetHorizontalScale(0.01f);
      terr->SetVerticalScale(25.f);
      terr->Regenerate();

	  cloudBlbs = new P51::CloudBillboards();

      weather = new P51ABC::Weather();   
      weather->AddDrawable(terr);
      this->AddDrawable(weather->GetEnvironment());

	  

   }
	~CloudApp()
   {
   }

protected:
   virtual void KeyPressed(P51::Keyboard* keyboard, 
      Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      switch(key) {
      case Producer::Key_Escape:    this->Quit();    	 break;

      case Producer::Key_H:   DisplayDebugGUI(true);   break;

      case Producer::Key_F1: weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED);    break;

      case Producer::Key_F2: weather->SetBasicVisibilityType(Weather::VIS_FAR);    break;

      case Producer::Key_F3: weather->SetBasicVisibilityType(Weather::VIS_MODERATE); break;

      case Producer::Key_F4: weather->SetBasicVisibilityType(Weather::VIS_LIMITED);  break;

      case Producer::Key_F5: weather->SetBasicVisibilityType(Weather::VIS_CLOSE);    break;

      default:
         break;
      }
   }
	
private:
   P51::InfiniteTerrain *terr;
   P51ABC::Weather *weather;
   P51::CloudBillboards *cloudBlbs;

};


	//Window win("TestClouds", 100 , 100 ,420, 340);
	//Scene scene;
	//System *sys = System::GetSystem();

	//Camera cam;
	//cam.SetWindow( &win );
	//cam.SetScene( &scene );
	//Transform position;
	//position.Set(-80.f, -500.f, 50.f, 0.0f, 0.f, 00.f);
	//cam.SetTransform( &position );

	//This is where we'll find our files to load

	//Object heightField("heightfield");
	//Object cessna("cessna");


	//P51::CloudBillboards *cb = new P51::CloudBillboards();
	
	/*env->AddEffect(sd);*/
	//env->AddEffect(cp);
    //env->AddEffect(cp2);
	//env->AddEffect(cb);


	//cessna.LoadFile("cessna.osg");
	//position.SetTranslation( 0.f, 0.f, 50.f );
	//cessna.SetTransform( &position, Transformable::REL_CS );
	
	//Add the Objects to the Scene to be rendered
	//env->AddDrawable(&heightField);

	//scene.AddDrawable(&cessna);
	//scene.AddDrawable(env);



//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
   CloudApp *app = new CloudApp();
   app->Config();
   app->Run();

	return 0;
}