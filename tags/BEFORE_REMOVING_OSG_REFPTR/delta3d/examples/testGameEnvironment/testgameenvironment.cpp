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
 * @author William E. Johnson II
 */
#include <dtABC/application.h>
#include <dtCore/environment.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/camera.h>
#include <dtDAL/actorproxy.h>
#include <dtActors/basicenvironmentactorproxy.h>
#include <dtUtil/exception.h>
#include <dtGame/gamemanager.h>

class TestGameEnvironmentApp : public dtABC::Application
{
   public:

      TestGameEnvironmentApp()
      {
         mGM = new dtGame::GameManager(*GetScene());
        
         dtCore::RefPtr<dtDAL::ActorProxy> proxy = mGM->CreateActor("dtcore.Terrain", "Infinite Terrain");
         if(!proxy.valid())
         {
            LOG_ERROR("Failed to create the infinite terrain proxy. Aborting.");
            Quit();
         }
         mTerrain = dynamic_cast<dtCore::InfiniteTerrain*>(proxy->GetActor());
         if(!mTerrain.valid())
         {
            LOG_ERROR("The infinite terrain proxy was created, but has an invalid actor. Aborting.");
            Quit();
         }
         mGM->AddActor(*proxy);

         mEnvironmentProxy = dynamic_cast<dtActors::BasicEnvironmentActorProxy*>(mGM->CreateActor("dtcore.Environment", "Environment").get());
         if(!mEnvironmentProxy.valid())
         {
            LOG_ERROR("Failed to create the environment actor. Aborting.");
            Quit();
         }
         mEnvironmentActor = dynamic_cast<dtActors::BasicEnvironmentActor*>(mEnvironmentProxy->GetActor());
         if(!mEnvironmentActor.valid())
         {
            LOG_ERROR("The environment actor proxy had an invalid actor. Aborting.");
            Quit();
         }
         mGM->SetEnvironmentActor(mEnvironmentProxy.get());

         mFMM = new dtCore::FlyMotionModel(GetKeyboard(), GetMouse());
         mFMM->SetTarget(GetCamera());

         GetScene()->UseSceneLight(true);

         dtCore::Transform transform(0.0f, 0.0f, 30.0f);
         GetCamera()->SetTransform(&transform);
      }

      virtual void Config()
      {
         GetWindow()->SetWindowTitle("testGameEnvironment");
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
               static bool enable = false;
               mEnvironmentActor->EnableCloudPlane(enable);
               enable = !enable;
               break;
            }
            case Producer::Key_2:
            {
               static bool enable = false;
               mEnvironmentActor->EnableFog(enable);
               enable = !enable;
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
               mGM->SetEnvironmentActor(mGM->GetEnvironmentActor() != NULL ? NULL : mEnvironmentProxy.get());
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
      dtCore::RefPtr<dtActors::BasicEnvironmentActorProxy> mEnvironmentProxy;
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
      LOG_ERROR("Exception caught: " + e.What());
   }
   return 0;
}
