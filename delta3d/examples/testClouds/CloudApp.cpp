
#include "application.h"
#include "infiniteterrain.h"
#include "weather.h"
#include "cloudDome.h"


using namespace dtABC;
using namespace dtCore;


class CloudApp : public dtABC::Application
{

public:
	CloudApp()
	{
		terr = new dtCore::InfiniteTerrain();
		terr->SetHorizontalScale(0.01f);
		terr->SetVerticalScale(25.f);
		terr->Regenerate();

		cd = new dtCore::CloudDome("../../data/NoiseVolume_ALPHA.dds");
		//cp = new dtCore::CloudPlane(6, 0.7, 16, 1, .2, 0.96, 512, 500);

		weather = new dtABC::Weather();   
		weather->AddDrawable(terr);

		weather->GetEnvironment()->AddEffect(cd);
		//weather->GetEnvironment()->AddEffect(cp);
		this->AddDrawable(weather->GetEnvironment());

		dtCore::Transform xform(0.f, 0.f, 30.f, 0.f, 0.f, 0.f);
		GetCamera()->SetTransform(&xform);

	}

	~CloudApp()
	{
	}

protected:
	virtual void KeyPressed(dtCore::Keyboard* keyboard, 
		Producer::KeyboardKey key,
		Producer::KeyCharacter character)
	{
		switch(key) {
			case Producer::Key_Escape:    this->Quit();    	 break;
			case Producer::Key_H:   DisplayDebugGUI(true);   break;
			case Producer::Key_F1: weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED); break;
			case Producer::Key_F2: weather->SetBasicVisibilityType(Weather::VIS_FAR);       break;
			case Producer::Key_F3: weather->SetBasicVisibilityType(Weather::VIS_MODERATE);  break;
			case Producer::Key_F4: weather->SetBasicVisibilityType(Weather::VIS_LIMITED);   break;
			case Producer::Key_F5: weather->SetBasicVisibilityType(Weather::VIS_CLOSE);     break;
			default:
				break;
		}
	}

private:
	dtCore::InfiniteTerrain *terr;
	dtABC::Weather *weather;

	dtCore::CloudDome *cd;
	dtCore::CloudPlane *cp;


};


//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	CloudApp *app = new CloudApp();
	app->Config();
	app->Run();

	return 0;
}



