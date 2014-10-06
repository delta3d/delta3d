#include "testtrigger.h"

#include <dtCore/keyboard.h>
#include <dtCore/camera.h>
#include <dtCore/transform.h>

using namespace dtCore;
using namespace dtABC;

////////////////////////////////////////////////////////////////////////////////
TestTrigger::LightAction::LightAction(dtCore::PositionalLight* light)
   : mPositionalLight(light)
{
}

////////////////////////////////////////////////////////////////////////////////
void TestTrigger::LightAction::OnStart()
{
   // When this action is started, we want it to invert the light's LightingMode.
   if (mPositionalLight->GetLightingMode() == Light::GLOBAL)
   {
      mPositionalLight->SetLightingMode(Light::LOCAL);
   }
   else
   {
      mPositionalLight->SetLightingMode(Light::GLOBAL);
   }
}

////////////////////////////////////////////////////////////////////////////////
TestTrigger::TestTrigger(const std::string& configFilename)
   : Application(configFilename)
   , mWarehouse(0)
   , mHappySphere(0)
   , mPositionalLight(0)
   , mWalkMotionModel(0)
   , mProximityTrigger(0)
{
}

////////////////////////////////////////////////////////////////////////////////
void TestTrigger::Config()
{
   // Load up a dark 'n scary warehouse...
   mWarehouse = new Object("Warehouse");
   mWarehouse->LoadFile("StaticMeshes/warehouse.ive");
   AddDrawable(mWarehouse.get());

   // Throw in a happy sphere to boot...
   mHappySphere = new Object("HappySphere ");
   mHappySphere->LoadFile("StaticMeshes/physics_happy_sphere.ive");

   // Make a yellow light that is local to only the sphere.
   mPositionalLight = new PositionalLight(0, "PositionalLight");
   mPositionalLight->SetDiffuse(1.0f, 1.0f, 0.0f, 1.0f);

   // Somehow, this needs to be called before the change to LightingMode
   // for it to take effect. Logged as a bug. -osb
   AddDrawable(mPositionalLight.get());
   mPositionalLight->SetLightingMode(dtCore::Light::LOCAL);
   mPositionalLight->AddChild(mHappySphere.get());

   // Motion model to walk around the scene.
   mWalkMotionModel = new WalkMotionModel(GetKeyboard(), GetMouse());
   mWalkMotionModel->SetTarget(GetCamera());
   mWalkMotionModel->SetScene(GetScene());
   mWalkMotionModel->SetEnabled(true);

   // Create our trigger. We want this to fire when the Camera hits it.
   mProximityTrigger = new ProximityTrigger("ProximityTrigger");

   // Let's make the collision shape a tad smaller (default is a sphere of 5.0f).
   // This makes it slightly bigger than the happy sphere.
   mProximityTrigger->SetCollisionSphere(1.0f);

   // Set the collide bits of this ProximityTrigger to match the category bits of Camera.
   // If you want this ProximityTrigger to hit more than once class, just do the OR of all
   // the relavent category bits. See the docs for Transformable::SetCollisionCollideBits
   // for more info.
   mProximityTrigger->SetCollisionCollideBits(GetCamera()->GetCollisionCategoryBits());

   // Assign our custom action to the internal trigger.
   mProximityTrigger->GetTrigger()->SetAction(new LightAction(mPositionalLight.get()));

   // Make the trigger activate an unlimited number of times.
   mProximityTrigger->GetTrigger()->SetTimesActive(-1);

   // Let's attach the trigger to the sphere, just in case we want to move the sphere.
   mHappySphere->AddChild(mProximityTrigger.get());

   // Position the default camera location inside the warehouse.
   Transform xform(4.0f, 17.5f, 2.5f, -180.0f, 0.0f, 0.0f);
   GetCamera()->SetTransform(xform);

   // Important! The Camera must be assigned a collision shape for this to work.
   GetCamera()->SetCollisionSphere(1.0f);

   // Also, note that collision detection will not occur unless the Camera is added to the
   // scene.
   AddDrawable(GetCamera());

   // Put the light/trigger/sphere slightly ahead of the camera.
   xform.SetTranslation(3.5f, 4.5f, 3.0f);
   xform.SetRotation(0.0f, 0.0f, 0.0f);
   mPositionalLight->SetTransform(xform);

   // Make a offset between the happy sphere and the light, so we can see the
   // sphere get lit easier.
   xform.SetTranslation(0.0f, -1.0f, -1.0f);
   mHappySphere->SetTransform(xform, dtCore::Transformable::REL_CS);

   Application::Config();
}
