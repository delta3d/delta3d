/* -*-c++-*-
 * testGameEnvironment - testgameenvironment (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * William E. Johnson II
 */
#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtCore/environment.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtCore/actorproxy.h>
#include <dtActors/weatherenvironmentactor.h>
#include <dtActors/engineactorregistry.h>
#include <dtUtil/exception.h>
#include <dtGame/gamemanager.h>
#include <osgGA/GUIEventAdapter>

class TestGameEnvironmentApp : public dtABC::Application
{
   public:

      TestGameEnvironmentApp()
      {
         Config();

         mGM = new dtGame::GameManager(*GetScene());

         dtCore::RefPtr<dtCore::BaseActorObject> proxy = mGM->CreateActor("dtcore.Terrain", "Infinite Terrain");
         if (!proxy.valid())
         {
            LOG_ERROR("Failed to create the infinite terrain proxy. Aborting.");
            Quit();
         }
         mTerrain = static_cast<dtCore::InfiniteTerrain*>(proxy->GetDrawable());

         mGM->AddActor(*proxy);

         mGM->CreateActor(*dtActors::EngineActorRegistry::WEATHER_ENVIRONMENT_ACTOR_TYPE, mEnvironmentActorProxy);
         if (!mEnvironmentActorProxy.valid())
         {
            LOG_ERROR("Failed to create the environment proxy. Aborting.");
            Quit();
         }

         mEnvironmentActor = static_cast<dtActors::WeatherEnvironmentActor*>(mEnvironmentActorProxy->GetDrawable());

         mGM->SetEnvironmentActor(mEnvironmentActorProxy.get());

         mFMM = new dtCore::FlyMotionModel(GetKeyboard(), GetMouse());
         mFMM->SetTarget(GetCamera());

         GetScene()->UseSceneLight(true);

         dtCore::Transform transform(0.0f, 0.0f, 30.0f);
         GetCamera()->SetTransform(transform);

         CreateHelpLabel();
      }

      virtual void Config()
      {
         GetWindow()->SetWindowTitle("TestGameEnvironment");
         dtABC::Application::Config();
      }

      virtual bool KeyPressed(const dtCore::Keyboard* keyboard,int key)
      {
         bool handled = true;
         switch (key)
         {
            case '1':
            {
               mEnvironmentActor->EnableCloudPlane(!mEnvironmentActor->IsCloudPlaneEnabled());
               break;
            }
            case '2':
            {
               mEnvironmentActor->EnableFog(!mEnvironmentActor->IsFogEnabled());
               break;
            }
            case '3':
            {
               mEnvironmentActor->SetWeatherVisibility(dtActors::WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_CLOSE);
               break;
            }
            case '4':
            {
               mEnvironmentActor->SetWeatherVisibility(dtActors::WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_MODERATE);
               break;
            }
            case '5':
            {
               mEnvironmentActor->SetWeatherVisibility(dtActors::WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_UNLIMITED);
               break;
            }
            case '6':
            {
               static bool enable = true;
               if (enable)
               {
                  mEnvironmentActor->SetWeatherTheme(dtActors::WeatherEnvironmentActor::WeatherThemeEnum::THEME_RAINY);
               }
               else
               {
                  mEnvironmentActor->SetWeatherTheme(dtActors::WeatherEnvironmentActor::WeatherThemeEnum::THEME_FAIR);
               }

               enable = !enable;
               break;
            }
            case '7':
            {
               static bool enable = true;
               if (enable)
               {
                  mEnvironmentActor->SetTimePeriodAndSeason(dtActors::WeatherEnvironmentActor::TimePeriodEnum::TIME_NIGHT, dtActors::WeatherEnvironmentActor::SeasonEnum::SEASON_WINTER);
               }
               else
               {
                  mEnvironmentActor->SetTimePeriodAndSeason(dtActors::WeatherEnvironmentActor::TimePeriodEnum::TIME_DAY, dtActors::WeatherEnvironmentActor::SeasonEnum::SEASON_SUMMER);
               }

               enable = !enable;
               break;
            }
            case '8':
            {
               static bool enable = true;
               if (enable)
               {
                  mEnvironmentActor->SetWindType(dtActors::WeatherEnvironmentActor::WindTypeEnum::WIND_SEVERE);
               }
               else
               {
                  mEnvironmentActor->SetWindType(dtActors::WeatherEnvironmentActor::WindTypeEnum::WIND_NONE);
               }
               break;
            }
            case ' ':
            {
               dtActors::WeatherEnvironmentActorProxy* proxy =
                  mGM->GetEnvironmentActor() == NULL ?
                  mEnvironmentActorProxy.get() :
                  NULL;

               mGM->SetEnvironmentActor(proxy);
               GetScene()->UseSceneLight(true);
               break;
            }
            case osgGA::GUIEventAdapter::KEY_Escape:
            {
               Quit();
               break;
            }
            case osgGA::GUIEventAdapter::KEY_F1:
            {
               mLabel->SetActive(!mLabel->GetActive());
               break;
            }
            default:
            {
               handled = false;
               break;
            }
         }
         return handled;
      }

   protected:

      virtual ~TestGameEnvironmentApp()
      {

      }

   private:
      void CreateHelpLabel()
      {
         mLabel = new dtABC::LabelActor();
         osg::Vec2 testSize(24.0f, 9.0f);
         mLabel->SetBackSize(testSize);
         mLabel->SetFontSize(0.8f);
         mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
         mLabel->SetText(CreateHelpLabelText());
         mLabel->SetEnableDepthTesting(false);
         mLabel->SetEnableLighting(false);

         GetCamera()->AddChild(mLabel.get());
         dtCore::Transform labelOffset(-17.0f, 50.0f, 8.5f, 0.0f, 90.0f, 0.0f);
         mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
         AddDrawable(GetCamera());
      }

      std::string CreateHelpLabelText()
      {
         std::string testString("");
         testString += "F1: Toggle Help Screen\n";
         testString += "\n";
         testString += "1: Toggle Clouds\n";
         testString += "2: Toggle Fog\n";
         testString += "3: Close Visibility\n";
         testString += "4: Moderate Visibility\n";
         testString += "5: Unlimited Visibility\n";
         testString += "6: Toggle Rainy/Fair Theme\n";
         testString += "7: Toggle Night/Day\n";
         testString += "8: Toggle Severe/No Wind\n";
         testString += "Space: Toggle EnvironmentActor\n";

         return testString;
      }

      dtCore::RefPtr<dtGame::GameManager> mGM;
      dtCore::RefPtr<dtActors::WeatherEnvironmentActorProxy> mEnvironmentActorProxy;
      dtCore::RefPtr<dtActors::WeatherEnvironmentActor> mEnvironmentActor;
      dtCore::RefPtr<dtCore::InfiniteTerrain> mTerrain;
      dtCore::RefPtr<dtCore::FlyMotionModel> mFMM;
      dtCore::RefPtr<dtABC::LabelActor> mLabel;
};


int main(int argc, char** argv)
{
   try
   {
      dtCore::RefPtr<TestGameEnvironmentApp> app = new TestGameEnvironmentApp;
      app->Config();
      app->Run();
   }
   catch(const dtUtil::Exception& e)
   {
      e.LogException(dtUtil::Log::LOG_ERROR);
      return -1;
   }
   return 0;
}
