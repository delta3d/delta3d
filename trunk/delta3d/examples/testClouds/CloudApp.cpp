#include "dt.h"
#include "dtabc.h"


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

		cd = new dtCore::CloudDome(6, 2, 0.7, 0.5, 0.7, 5, 5500.f, 20);
		//cd = new dtCore::CloudDome(5500.0f, 20, "../../data/NoiseVolume_ALPHA.dds");
		cp[0] = new dtCore::CloudPlane(6, 0.5, 6, 1, .3, 0.96, 256, 1800);
		cp[1] = new dtCore::CloudPlane(6, 0.7, 12, 1, .4, 0.95, 512, 1000);
		cp[2] = new dtCore::CloudPlane(6, 0.8, 20, 1, .2, 0.96, 512, 600);

		weather = new dtABC::Weather();   
		weather->AddDrawable(terr);
		
		cloudLayers = 1;
		isDomeEnabled = false;
		weather->GetEnvironment()->AddEffect(cp[0].get());
		this->AddDrawable(weather->GetEnvironment());

		GetWindow()->SetWindowTitle("Cloud Simulation");

		dtCore::Transform xform(0.f, 00.f, 30.f, 0.f, 10.f, 0.f);
		GetCamera()->SetTransform(&xform);

		mLogicalInputDevice = new LogicalInputDevice();

      Axis* leftButtonLeftAndRight = mLogicalInputDevice->AddAxis(
         "left mouse button left/right",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(LeftButton),
         GetMouse()->GetAxis(0)
         ));

      Axis* leftButtonUpAndDown = mLogicalInputDevice->AddAxis(
         "left mouse button up/down",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(LeftButton),
         GetMouse()->GetAxis(1)
         ));

      Axis* middleButtonUpAndDown = mLogicalInputDevice->AddAxis(
         "middle mouse button up/down",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(MiddleButton),
         GetMouse()->GetAxis(1)
		 ));

      Axis* rightButtonLeftAndRight = mLogicalInputDevice->AddAxis(
         "right mouse button left/right",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(RightButton),
         GetMouse()->GetAxis(0)
         ));

      Axis* rightButtonUpAndDown = mLogicalInputDevice->AddAxis(
         "right mouse button up/down",
         new ButtonAxisToAxis(
		 GetMouse()->GetButton(RightButton),
         GetMouse()->GetAxis(1)
         ));

      orbit = new dtCore::OrbitMotionModel();
      orbit->SetAzimuthAxis( leftButtonLeftAndRight );
      orbit->SetElevationAxis(leftButtonUpAndDown );
      orbit->SetDistanceAxis( middleButtonUpAndDown );
      orbit->SetLeftRightTranslationAxis(rightButtonLeftAndRight);
      orbit->SetUpDownTranslationAxis(rightButtonUpAndDown);
      orbit->SetTarget(GetCamera());
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
			case Producer::Key_P:
				if(isDomeEnabled)
				{
					for(int i = 0; i < cloudLayers; ++i)
						weather->GetEnvironment()->AddEffect(cp[i].get());
					
					weather->GetEnvironment()->RemEffect(cd.get());
					isDomeEnabled = false;
				}
				break;
			case Producer::Key_D:
				if(!isDomeEnabled)
				{
					weather->GetEnvironment()->AddEffect(cd.get());
					isDomeEnabled = true;
					for(int i = 0; i < cloudLayers; ++i)
						weather->GetEnvironment()->RemEffect(cp[i].get());
				}
				break;
			case Producer::Key_KP_Add:
				if (!isDomeEnabled && cloudLayers >= 0 && cloudLayers < 3)
				{	
					weather->GetEnvironment()->AddEffect(cp[cloudLayers].get());
					++cloudLayers;
				}
				break;
			case Producer::Key_KP_Subtract:
				if (!isDomeEnabled && cloudLayers > 0)
				{
					--cloudLayers;
					weather->GetEnvironment()->RemEffect(cp[cloudLayers].get());
				}	
				break;
			default:
				break;
		}
	}

private:
	dtCore::InfiniteTerrain *terr;
	dtABC::Weather *weather;
	dtCore::OrbitMotionModel *orbit;
    LogicalInputDevice *mLogicalInputDevice;

	osg::ref_ptr<dtCore::CloudDome>  cd;
	osg::ref_ptr<dtCore::CloudPlane> cp[3];
	int cloudLayers;
	bool isDomeEnabled;

};


//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	CloudApp *app = new CloudApp();
	app->Config();
	app->Run();

	return 0;
}
