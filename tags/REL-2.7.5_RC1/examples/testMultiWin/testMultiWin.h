#ifndef DELTA_testMultiWin
#define DELTA_testMultiWin

// TestMultiWin.cpp : declares the interface of the application

#include <dtABC/application.h>
#include <dtCore/orbitmotionmodel.h>

class TestMultiWin : public dtABC::Application
{
public:
   TestMultiWin(const std::string& configFilename = "");

protected:
   virtual ~TestMultiWin();

public:
   virtual void Config();
   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int kc);

private:
   dtCore::RefPtr<dtCore::DeltaWin> mWin2;
   dtCore::RefPtr<dtCore::Camera>   mCam2;
   dtCore::RefPtr<dtCore::Camera>   mCam3;
   
   dtCore::RefPtr<dtCore::View> mView2;
   dtCore::RefPtr<dtCore::View> mView3;
   dtCore::RefPtr<dtCore::OrbitMotionModel> mMotion1;
   dtCore::RefPtr<dtCore::OrbitMotionModel> mMotion2;
   dtCore::RefPtr<dtCore::OrbitMotionModel> mMotion3;
};

#endif // DELTA_testMultiWin
