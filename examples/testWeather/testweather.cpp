/* -*-c++-*-
 * testWeather - testWeather (.h & .cpp) - Using 'The MIT License'
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
#include <dtCore/environment.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/refptr.h>
#include <dtCore/transform.h>

#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtABC/weather.h>

#include <dtUtil/datapathutils.h>

using namespace dtABC;
using namespace dtCore;

class TestWeatherApp : public dtABC::Application
{

   DECLARE_MANAGEMENT_LAYER(TestWeatherApp)

public:
   TestWeatherApp(const std::string& configFile = "config.xml") : Application(configFile)
   {
      terr = new dtCore::InfiniteTerrain();
      terr->SetHorizontalScale(0.005);
      terr->SetVerticalScale(35.0f);
      terr->SetSegmentDivisions(64);

      Transform trans = Transform(0.0f, 0.0f, terr->GetVerticalScale() + 15.0f);
      GetCamera()->SetTransform(trans);

      weather = new dtABC::Weather();
      weather->AddChild(terr.get());

      AddDrawable(weather->GetEnvironment());

      CreateHelpLabel();

      orbit = new dtCore::OrbitMotionModel(GetKeyboard(), GetMouse());
      orbit->SetTarget(GetCamera());
   }
protected:
   virtual ~TestWeatherApp() {}

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      bool verdict(false);
      switch (key)
      {
      case osgGA::GUIEventAdapter::KEY_Escape:    Quit();        verdict=true;  break;

      case osgGA::GUIEventAdapter::KEY_F1: mLabel->SetActive(!mLabel->GetActive()); verdict=true; break;

      case '1': weather->SetBasicVisibilityType(Weather::VIS_UNLIMITED); verdict=true;  break;
      case '2': weather->SetBasicVisibilityType(Weather::VIS_FAR);       verdict=true;  break;
      case '3': weather->SetBasicVisibilityType(Weather::VIS_MODERATE);  verdict=true;  break;
      case '4': weather->SetBasicVisibilityType(Weather::VIS_LIMITED);   verdict=true;  break;
      case '5': weather->SetBasicVisibilityType(Weather::VIS_CLOSE);     verdict=true;  break;

      case '!': weather->SetBasicCloudType(Weather::CLOUD_CLEAR);      verdict=true;  break;
      case '@': weather->SetBasicCloudType(Weather::CLOUD_FEW);        verdict=true;  break;
      case '#': weather->SetBasicCloudType(Weather::CLOUD_SCATTERED);  verdict=true;  break;
      case '$': weather->SetBasicCloudType(Weather::CLOUD_BROKEN);     verdict=true;  break;
      case '%': weather->SetBasicCloudType(Weather::CLOUD_OVERCAST);   verdict=true;  break;
      default:
         break;
      }

      return verdict;
   }

private:
   void CreateHelpLabel()
   {
      mLabel = new LabelActor();
      osg::Vec2 testSize(19.5f, 10.5f);
      mLabel->SetBackSize(testSize);
      mLabel->SetFontSize(0.8f);
      mLabel->SetTextAlignment(LabelActor::AlignmentEnum::LEFT_CENTER);
      mLabel->SetText(CreateHelpLabelText());
      mLabel->SetEnableDepthTesting(false);
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
      testString += "1: Unlimited Visibility\n";
      testString += "2: Far Visibility\n";
      testString += "3: Moderate Visibility\n";
      testString += "4: Limited Visibility\n";
      testString += "5: Close Visibility\n";
      testString += "\n";
      testString += "Shift + 1: No Clouds Shift\n";
      testString += "Shift + 2: Few Clouds\n";
      testString += "Shift + 3: Scattered Clouds\n";
      testString += "Shift + 4: Broken Clouds\n";
      testString += "Shift + 5: Overcast Clouds";

      return testString;
   }

   RefPtr<InfiniteTerrain> terr;
   RefPtr<Weather> weather;
   RefPtr<OrbitMotionModel> orbit;
   RefPtr<LabelActor> mLabel;
};

IMPLEMENT_MANAGEMENT_LAYER( TestWeatherApp )

int main()
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/testWeather" + ";");

   RefPtr<TestWeatherApp> app = new TestWeatherApp("config.xml");
   app->Config();
   app->Run();

   return 0;
}
