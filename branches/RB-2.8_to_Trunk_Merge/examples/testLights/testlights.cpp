#include "testlights.h"
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtUtil/datapathutils.h>

#include <osgGA/GUIEventAdapter>

using namespace dtABC;
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(TestLightsApp)

float TestLightsApp::countOne   = 0.0f;
float TestLightsApp::countTwo   = 0.0f;
float TestLightsApp::countThree = 0.0f;

TestLightsApp::TestLightsApp(const std::string& configFilename)
   : Application(configFilename)
{}

void TestLightsApp::Config()
{
   // turn off scene light so we only see the lights we create
   GetScene()->UseSceneLight(false);

   // load up a warehouse
   mWarehouse = new Object("Warehouse");
   mWarehouse->LoadFile("StaticMeshes/warehouse.ive");
   AddDrawable(mWarehouse.get());

   Transform trans;

   // create a global spot light.
   mGlobalSpot = new SpotLight(1, "GlobalSpotlight");
   trans.Set(5.0f, 8.0f, 2.0f, 0.0f, 0.0f, 0.0f);
   mGlobalSpot->SetTransform(trans);
   mGlobalSpot->SetSpotCutoff(20.0f);
   mGlobalSpot->SetSpotExponent(50.0f);
   GetScene()->AddChild(mGlobalSpot.get());

   // add a child to the local light
   mSphere = new Object("HappySphere");
   mSphere->LoadFile("StaticMeshes/physics_happy_sphere.ive");

   // we want the sphere 1 unit below light so we can see effect of local light and
   trans.Set(0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
   mSphere->SetTransform(trans);

   // create a global positional light.
   mPositional = new PositionalLight(3, "PositionalLight");
   mPositional->SetDiffuse(1.0f, 1.0f, 0.0f, 1.0f); // yellow light
   mPositional->AddChild(mSphere.get()); //move sphere along with light
   GetScene()->AddChild(mPositional.get());
   mPositional->SetEnabled(false);

   // create an infinite light
   mGlobalInfinite = new InfiniteLight(4, "GlobalInfiniteLight");
   GetScene()->AddChild(mGlobalInfinite.get());
   mGlobalInfinite->SetEnabled(true);

   // set camera stuff
   trans.Set(30.0f, -20.0f, 25.0f, 40.0f, -33.0f, 0.0f);
   GetCamera()->SetTransform(trans);

   osg::Vec3 camLoc, origin;
   trans.GetTranslation(camLoc);

   mOmm = new OrbitMotionModel(GetKeyboard(), GetMouse());
   mOmm->SetTarget(GetCamera());

   float distance(0.0f);
   for (int i = 0; i < 3; i++)
   {
      distance += osg::square(camLoc[i] - origin[i]);
   }
   distance = sqrt(distance);

   mOmm->SetDistance(distance);

   CreateHelpLabel();
}

bool TestLightsApp::KeyPressed(const Keyboard* keyboard, int key)
{
   bool verdict(false);
   switch (key)
   {
   case osgGA::GUIEventAdapter::KEY_Escape:
      Quit();
      verdict = true;
      break;
   case '1':
      mGlobalSpot->SetEnabled(!mGlobalSpot->GetEnabled());
      verdict = true;
      break;
   case '2':
      mPositional->SetEnabled(!mPositional->GetEnabled());
      verdict = true;
      break;
   case '3':
   {
      if (mPositional->GetLightingMode() == Light::GLOBAL)
      {
         mPositional->SetLightingMode(Light::LOCAL);
      }
      else
      {
         mPositional->SetLightingMode(Light::GLOBAL);
      }

      verdict = true;
      break;
   }
   case '4':
      mGlobalInfinite->SetEnabled(!mGlobalInfinite->GetEnabled());
      verdict = true;
      break;
   case osgGA::GUIEventAdapter::KEY_F1:
      mLabel->SetActive(!mLabel->GetActive());
      break;
   default:
      break;
   }

   return verdict;
}

void TestLightsApp::PreFrame(const double deltaFrameTime)
{
   // increment some values at different rates
   countOne   += 50.0f * deltaFrameTime;
   countTwo   += 60.0f * deltaFrameTime;
   countThree += 70.0f * deltaFrameTime;

   // cap at 360
   if (countOne   > 360.0f) countOne   -= 360.0f;
   if (countTwo   > 360.0f) countTwo   -= 360.0f;
   if (countThree > 360.0f) countThree -= 360.0f;

   // scale values to 0.0-1.0
   float redValue   = (cos(osg::DegreesToRadians(countOne))   + 1.0f) / 2.0f;
   float greenValue = (cos(osg::DegreesToRadians(countTwo))   + 1.0f) / 2.0f;
   float blueValue  = (cos(osg::DegreesToRadians(countThree)) + 1.0f) / 2.0f;

   // modify all global lights
   mGlobalSpot->SetDiffuse(redValue, greenValue, blueValue, 1.0f); // change color

   // rotate the spotlight
   Transform trans;
   mGlobalSpot->GetTransform(trans);
   trans.SetRotation(countOne, 0.0f, 0.0f);
   mGlobalSpot->SetTransform(trans);

   mPositional->SetAttenuation(1.0f, greenValue / 2.0f, blueValue / 2.0f); // change attenutation

   // move the global positional light in a circle
   float tx = 1.5 * cos(osg::DegreesToRadians(countOne)) + 2.0f;
   float ty = 1.5 * sin(osg::DegreesToRadians(countOne)) + 5.0f;
   trans.Set( tx, ty, 2.0f, 0.0f, 0.0f, 0.0f);
   mPositional->SetTransform(trans);

   mGlobalInfinite->SetDiffuse(redValue, greenValue, blueValue, 1.0f); //change color

   float th = countOne;
   float tp = countTwo;
   
   mGlobalInfinite->SetTransform(dtCore::Transform(0.0f, 0.0f, 0.0f, th, tp));
}

void TestLightsApp::CreateHelpLabel()
{
   mLabel = new dtABC::LabelActor();
   osg::Vec2 testSize(23.5f, 5.5f);
   mLabel->SetBackSize(testSize);
   mLabel->SetFontSize(0.8f);
   mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
   mLabel->SetText(CreateHelpLabelText());
   mLabel->SetEnableDepthTesting(false);
   mLabel->SetEnableLighting(false);

   GetCamera()->AddChild(mLabel.get());
   dtCore::Transform labelOffset(-17.0f, 50.0f, 10.5f, 0.0f, 90.0f, 0.0f);
   mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
   AddDrawable(GetCamera());
}

std::string TestLightsApp::CreateHelpLabelText()
{
   std::string testString("");
   testString += "F1: Toggle Help Screen\n";
   testString += "\n";
   testString += "1: Toggle Spot Light\n";
   testString += "2: Toggle Positional Light\n";
   testString += "3: Toggle Positional Light's Mode\n";
   testString += "4: Toggle Infinite Light\n";

   return testString;
}

int main(int argc, const char* argv[])
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/data" + ";");

   RefPtr<TestLightsApp> app = new TestLightsApp("config.xml");
   app->Config();
   app->Run();

   return 0;
}
