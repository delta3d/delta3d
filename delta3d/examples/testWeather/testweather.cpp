/********************************************************************
	created:	2004/02/29
	
	Purpose:	This application tests out the dtABC::Weather class by
            deriving from the dtABC::Application class and renders the 
            dtCore::InfiniteTerrain.

            Keyboard controls:
            F1-F5: change the Visibility
            1-5  : change the cloud layers
            Esc: Quit the application
            H: display the debug GUI 
*********************************************************************/
#include "dt.h"
#include "dtabc.h"

/*
#include "application.h"
#include "infiniteterrain.h"
#include "weather.h"
#include "orbitmotionmodel.h"
#include "logicalinputdevice.h"*/


using namespace dtABC;
using namespace dtCore;

class WeatherApp : public dtABC::Application
{
public:
	WeatherApp()
   {
      terr = new dtCore::InfiniteTerrain();
      terr->SetHorizontalScale(0.005);
      terr->SetVerticalScale(35.f);
      terr->SetSegmentDivisions(64);

      weather = new dtABC::Weather();   
      weather->AddDrawable(terr);

      this->AddDrawable(weather->GetEnvironment());
      GetWindow()->SetWindowTitle("F1-F5 for Visibility, 1-5 for Clouds");

      mLogicalInputDevice = new LogicalInputDevice();

      Axis* leftButtonLeftAndRight = mLogicalInputDevice->AddAxis(
         "left mouse button left/right",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(LeftButton),
         GetMouse()->GetAxis(0)
         )
         );

      Axis* leftButtonUpAndDown = mLogicalInputDevice->AddAxis(
         "left mouse button up/down",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(LeftButton),
         GetMouse()->GetAxis(1)
         )
         );

      Axis* middleButtonUpAndDown = mLogicalInputDevice->AddAxis(
         "middle mouse button up/down",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(MiddleButton),
         GetMouse()->GetAxis(1)
         )
         );

      Axis* rightButtonLeftAndRight = mLogicalInputDevice->AddAxis(
         "right mouse button left/right",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(RightButton),
         GetMouse()->GetAxis(0)
         )
         );

      Axis* rightButtonUpAndDown = mLogicalInputDevice->AddAxis(
         "right mouse button up/down",
         new ButtonAxisToAxis(
         GetMouse()->GetButton(RightButton),
         GetMouse()->GetAxis(1)
         )
         );

      orbit = new dtCore::OrbitMotionModel();
      orbit->SetAzimuthAxis( leftButtonLeftAndRight );
      orbit->SetElevationAxis(leftButtonUpAndDown );
      orbit->SetDistanceAxis( middleButtonUpAndDown );
      orbit->SetLeftRightTranslationAxis(rightButtonLeftAndRight);
      orbit->SetUpDownTranslationAxis(rightButtonUpAndDown);
      orbit->SetTarget(GetCamera());

 }
	~WeatherApp()
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
   dtCore::InfiniteTerrain *terr;
   dtABC::Weather *weather;
   dtCore::OrbitMotionModel *orbit;
   LogicalInputDevice *mLogicalInputDevice;
};


//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
   WeatherApp *app = new WeatherApp();
   app->Config();
   app->Run();

	return 0;
}
