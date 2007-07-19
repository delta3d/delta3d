/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * William E. Johnson II
 */
#include <dtABC/application.h>
#include <dtCore/environment.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtDAL/actorproxy.h>
#include <dtActors/basicenvironmentactorproxy.h>
#include <dtUtil/exception.h>
#include <dtGame/gamemanager.h>

class TestGameEnvironmentApp : public dtABC::Application
{
   public:

      TestGameEnvironmentApp()
      {
         Config();

         mGM = new dtGame::GameManager(*GetScene());
        
         dtCore::RefPtr<dtDAL::ActorProxy> proxy = mGM->CreateActor("dtcore.Terrain", "Infinite Terrain");
         if(!proxy.valid())
         {
            LOG_ERROR("Failed to create the infinite terrain proxy. Aborting.");
            Quit();
         }
         mTerrain = static_cast<dtCore::InfiniteTerrain*>(proxy->GetActor());
         
         mGM->AddActor(*proxy);

         proxy = mGM->CreateActor("dtcore.Environment", "Environment");
         if(!proxy.valid())
         {
            LOG_ERROR("Failed to create the environment proxy. Aborting.");
            Quit();
         }
            
         mEnvironmentActor = static_cast<dtActors::BasicEnvironmentActor*>(proxy->GetActor());
         
         mGM->SetEnvironmentActor(static_cast<dtActors::BasicEnvironmentActorProxy*>(&mEnvironmentActor->GetGameActorProxy()));

         mFMM = new dtCore::FlyMotionModel(GetKeyboard(), GetMouse());
         mFMM->SetTarget(GetCamera());

         GetScene()->UseSceneLight(true);

         dtCore::Transform transform(0.0f, 0.0f, 30.0f);
         GetCamera()->SetTransform(transform);
      }

      virtual void Config()
      {
         GetWindow()->SetWindowTitle("TestGameEnvironment");
         dtABC::Application::Config();
      }

      virtual bool KeyPressed(const dtCore::Keyboard* keyboard, 
                              Producer::KeyboardKey key, 
                              Producer::KeyCharacter character)
      {
         bool handled = true;
         switch(key)
         {
            case Producer::Key_1:
            {
               mEnvironmentActor->EnableCloudPlane(!mEnvironmentActor->IsCloudPlaneEnabled());
               break;
            }
            case Producer::Key_2:
            {
               mEnvironmentActor->EnableFog(!mEnvironmentActor->IsFogEnabled());
               break;
            }
            case Producer::Key_3:
            {
               mEnvironmentActor->SetWeatherVisibility(dtActors::BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_CLOSE);
               break;
            }
            case Producer::Key_4:
            {
               mEnvironmentActor->SetWeatherVisibility(dtActors::BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_MODERATE);
               break;
            }
            case Producer::Key_5:
            {
               mEnvironmentActor->SetWeatherVisibility(dtActors::BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_UNLIMITED);
               break;
            }
            case Producer::Key_6:
            {
               static bool enable = true;
               if(enable)
                  mEnvironmentActor->SetWeatherTheme(dtActors::BasicEnvironmentActor::WeatherThemeEnum::THEME_RAINY);
               else
                  mEnvironmentActor->SetWeatherTheme(dtActors::BasicEnvironmentActor::WeatherThemeEnum::THEME_FAIR);
               
               enable = !enable;
               break;
            }
            case Producer::Key_7:
            {
               static bool enable = true;
               if(enable)
                  mEnvironmentActor->SetTimePeriodAndSeason(dtActors::BasicEnvironmentActor::TimePeriodEnum::TIME_NIGHT, dtActors::BasicEnvironmentActor::SeasonEnum::SEASON_WINTER);
               else
                  mEnvironmentActor->SetTimePeriodAndSeason(dtActors::BasicEnvironmentActor::TimePeriodEnum::TIME_DAY, dtActors::BasicEnvironmentActor::SeasonEnum::SEASON_SUMMER);
               
               enable = !enable;
               break;
            }
            case Producer::Key_8:
            {
               static bool enable = true;
               if(enable)
                  mEnvironmentActor->SetWindType(dtActors::BasicEnvironmentActor::WindTypeEnum::WIND_SEVERE);
               else
                  mEnvironmentActor->SetWindType(dtActors::BasicEnvironmentActor::WindTypeEnum::WIND_NONE);
               break;
            }
            case Producer::Key_space:
            {
               dtActors::BasicEnvironmentActorProxy *proxy = 
                  mGM->GetEnvironmentActor() == NULL ?
                  static_cast<dtActors::BasicEnvironmentActorProxy*>(&mEnvironmentActor->GetGameActorProxy()) :
                  NULL;

               mGM->SetEnvironmentActor(proxy);
               GetScene()->UseSceneLight(true);
               break;
            }
            case Producer::Key_Escape:
            {
               Quit();
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

      dtCore::RefPtr<dtGame::GameManager> mGM;
      dtCore::RefPtr<dtActors::BasicEnvironmentActor> mEnvironmentActor;
      dtCore::RefPtr<dtCore::InfiniteTerrain> mTerrain;
      dtCore::RefPtr<dtCore::FlyMotionModel> mFMM;
};


int main(int argc, char **argv)
{
   try
   {
      dtCore::RefPtr<TestGameEnvironmentApp> app = new TestGameEnvironmentApp;
      app->Config();
      app->Run();
   }
   catch(const dtUtil::Exception &e) 
   {
      e.LogException(dtUtil::Log::LOG_ERROR);
      return -1;
   }
   return 0;
}
