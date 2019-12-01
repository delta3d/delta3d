/* -*-c++-*-
 * testClouds - testclouds (.h & .cpp) - Using 'The MIT License'
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <osgGA/GUIEventAdapter>

#include <dtABC/application.h>
#include <dtABC/weather.h>
#include <dtABC/labelactor.h>

#include <dtCore/infiniteterrain.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/clouddome.h>
#include <dtCore/cloudplane.h>
#include <dtCore/environment.h>
#include <dtCore/refptr.h>
#include <dtCore/transform.h>

#include <dtUtil/log.h>

using namespace dtABC;
using namespace dtCore;

class TestCloudsApp : public Application
{

DECLARE_MANAGEMENT_LAYER(TestCloudsApp)

public:
   TestCloudsApp(std::string configFilename = "config.xml")
      : Application(configFilename)
   {
      terr = new InfiniteTerrain();
      terr->SetHorizontalScale(0.01f);
      terr->SetVerticalScale(25.0f);
      terr->Regenerate();

      LOG_ALWAYS("Creating clouds...");
      cd = new CloudDome(6, 2, 0.7, 0.5, 0.7, 5, 5500.0f, 20);
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

      Transform xform(0.0f, 00.0f, 30.0f, 0.0f, 10.0f, 0.0f);
      GetCamera()->SetTransform(xform);
      GetCamera()->SetNearFarCullingMode(dtCore::Camera::BOUNDING_VOLUME_NEAR_FAR);

      orbit = new OrbitMotionModel(GetKeyboard(), GetMouse());
      orbit->SetTarget(GetCamera());

      CreateHelpLabel();
   }

   ~TestCloudsApp()
   {
   }

protected:
   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      bool verdict(false);
      switch (key)
      {
      case osgGA::GUIEventAdapter::KEY_Escape:
         {
            this->Quit();
            verdict = true;
         } break;

      case osgGA::GUIEventAdapter::KEY_F1: mLabel->SetActive(!mLabel->GetActive()); verdict=true; break;

      case osgGA::GUIEventAdapter::KEY_F2: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED); break;
      case osgGA::GUIEventAdapter::KEY_F3: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_FAR);       break;
      case osgGA::GUIEventAdapter::KEY_F4: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_MODERATE);  break;
      case osgGA::GUIEventAdapter::KEY_F5: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_LIMITED);   break;
      case osgGA::GUIEventAdapter::KEY_F6: verdict=true; weather->SetBasicVisibilityType(Weather::VIS_CLOSE);     break;

      case 'p':
         {
            if (isDomeEnabled)
            {
               for (int i = 0; i < cloudLayers; ++i)
               {
                  weather->GetEnvironment()->AddEffect(cp[i].get());
               }

               weather->GetEnvironment()->RemEffect(cd.get());
               isDomeEnabled = false;
            }
            verdict = true;
         } break;
      case 'd':
         if (!isDomeEnabled)
         {
            weather->GetEnvironment()->AddEffect(cd.get());
            isDomeEnabled = true;
            for (int i = 0; i < cloudLayers; ++i)
            {
               weather->GetEnvironment()->RemEffect(cp[i].get());
            }
            verdict = true;
         } break;
      case osgGA::GUIEventAdapter::KEY_KP_Add:
      case '=':
         if (!isDomeEnabled && cloudLayers >= 0 && cloudLayers < 3)
         {
            weather->GetEnvironment()->AddEffect(cp[cloudLayers].get());
            ++cloudLayers;
            verdict = true;
         } break;
      case osgGA::GUIEventAdapter::KEY_KP_Subtract:
      case '-':
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

   void CreateHelpLabel()
   {
      mLabel = new LabelActor();
      osg::Vec2 testSize(19.5f, 10.5f);
      mLabel->SetBackSize(testSize);
      mLabel->SetFontSize(0.8f);
      mLabel->SetTextAlignment(LabelActor::AlignmentEnum::LEFT_CENTER);
      mLabel->SetText(CreateHelpLabelText());
      mLabel->SetEnableDepthTesting(false);
      mLabel->SetBackVisible(false);
      mLabel->SetEnableLighting(false);

      GetCamera()->AddChild(mLabel.get());
      Transform labelOffset(-17.0f, 50.0f, 7.75f, 0.0f, 90.0f, 0.0f);
      mLabel->SetTransform(labelOffset, Transformable::REL_CS);
      AddDrawable(GetCamera());
   }

   std::string CreateHelpLabelText()
   {
      std::string testString("");
      testString += "F1: Toggle Help Screen\n";
      testString += "\n";
      testString += "F2-F6  -  Set visibility (F2:unlimited - F6:close)\n";
      testString += "p      -  Turn off cloud dome\n";
      testString += "d      -  Turn on cloud dome\n";
      testString += "+|-    -  Increase|decrease number of cloud layers\n";
      testString += "Esc    -  Exit\n";
      testString += "\n";
      testString += "Left Mouse Button    -  Rotate View\n";
      testString += "Right Mouse Button   -  Translate View\n";
      testString += "Middle Mouse Button  -  Zoom View\n";

      return testString;
   }

private:
   RefPtr<InfiniteTerrain> terr;
   RefPtr<Weather> weather;
   RefPtr<OrbitMotionModel> orbit;

   RefPtr<dtCore::CloudDome>  cd;
   RefPtr<dtCore::CloudPlane> cp[3];
   int cloudLayers;
   bool isDomeEnabled;
   RefPtr<LabelActor> mLabel;

};

IMPLEMENT_MANAGEMENT_LAYER( TestCloudsApp )

int main(int argc, char* argv[])
{
   RefPtr<TestCloudsApp> app = new TestCloudsApp("config.xml");
   app->Config();
   app->Run();

   return 0;
}
