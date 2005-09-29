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
* @author Matthew W. Campbell
*/
#include <osgDB/ReadFile>
#include <osg/Image>
#include <dtCore/object.h>
#include <dtABC/dtabc.h>
#include <dtCore/refptr.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/globals.h>
#include <dtTerrain/terrain.h>
#include <dtTerrain/dtedterrainreader.h>
#include <dtTerrain/soarxterrainrenderer.h>
#include <dtUtil/exception.h>
#include <dtTerrain/terraindecorationlayer.h>
#include <dtTerrain/vegetationdecorator.h>
#include <dtTerrain/lccanalyzer.h>
#include <dtTerrain/lcctype.h>

class TestTerrainApp : public dtABC::Application
{
   public:
      
      TestTerrainApp() : dtABC::Application("config.xml")
      {
         mTerrainClamp = false;
         mFlyFast = false;
         
         // Set up a motion model so we can move the camera
         mMotionModel = new dtCore::FlyMotionModel(GetKeyboard(), GetMouse());
         mMotionModel->SetMaximumFlySpeed(800.0f);
         mMotionModel->SetTarget(GetCamera());
         
         GetCamera()->GetSceneHandler()->GetSceneView()->setComputeNearFarMode(
            osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
            
         //GetScene()->GetSceneHandler()->GetSceneView()->setCullingMode(
         //   osg::CullSettings::NO_CULLING);
        
         GetCamera()->GetLens()->setPerspective(60.0f,60.0f,1,250000.0f);
         GetCamera()->SetAutoAspect(true);         
         
         //Create our terrain and add a reader and renderer to it.
         mTerrain = new dtTerrain::Terrain();
         mTerrain->SetCachePath("cache");
        
         //Create the terrain reader...
         dtTerrain::DTEDTerrainReader *reader = new dtTerrain::DTEDTerrainReader();
                 
         //Create the terrain renderer...
         dtTerrain::SoarXTerrainRenderer *renderer = new dtTerrain::SoarXTerrainRenderer();
         osg::Image *image = osgDB::readImageFile("textures/concrete.jpg");
         if (image != NULL)
         {
            std::vector<dtTerrain::TerrainDataRenderer::CustomImage> images;
            images.push_back(image);
            renderer->SetCustomImageList(images);
         }
         else
         {
            LOG_ERROR("Could not load base image texture for the terrain.");
         }
       
         //Connect our reader and renderer to the terrain.
         mTerrain->SetDataReader(reader);
         mTerrain->SetDataRenderer(renderer);
         
         //Before we load any resources, lets set our origin to correlate to the data we
         //are loading.
         double lon = -121.85;
         double lat = 36.58;
         mTerrain->SetOrigin(osg::Vec3d(lat,lon,0.0));         
         
         mTerrain->LoadResource("dted");
         
         dtCore::Transform tx;
         tx.Set(22614.9, 27194.8, 23.3, -19.5, -0.7, 0.0);                          
         GetCamera()->SetTransform(&tx);         

         //Finally add the loaded terrain to the scene.
         GetScene()->AddDrawable(mTerrain.get());    

         //Finish all system configuration..
         Config();
      }
      
      virtual void KeyPressed(dtCore::Keyboard *keyBoard, Producer::KeyboardKey key,
         Producer::KeyCharacter character)
      {
         static bool wireFrame = false;
         switch (key)
         {
            case Producer::Key_Return:
               GetCamera()->SetNextStatisticsType();
               break;
               
            case Producer::Key_space:
               wireFrame = !wireFrame;
               if (wireFrame)
                  GetScene()->SetRenderState(dtCore::Scene::FRONT_AND_BACK, dtCore::Scene::LINE);
               else
                  GetScene()->SetRenderState(dtCore::Scene::FRONT, dtCore::Scene::FILL);
               break;
            
            case Producer::Key_G:
               mTerrainClamp = !mTerrainClamp;
               if (!mTerrainClamp)
               {
                  if (mFlyFast)
                     mMotionModel->SetMaximumFlySpeed(2500);
                  else
                     mMotionModel->SetMaximumFlySpeed(800);
               }
               else
               {
                  mMotionModel->SetMaximumFlySpeed(128);
               }
                  
               break;
               
            case Producer::Key_F:
               mFlyFast = true;
               mMotionModel->SetMaximumFlySpeed(2500);
               break;
            
            default:
               dtABC::Application::KeyPressed(keyBoard,key,character);
               break;
         } 
      }
      
      virtual void KeyReleased(dtCore::Keyboard *keyBoard, Producer::KeyboardKey key,
         Producer::KeyCharacter character)
      {
         switch (key)
         {
            case Producer::Key_F:
               mFlyFast = false;
               mMotionModel->SetMaximumFlySpeed(800);
               break;
               
            default:
               dtABC::Application::KeyReleased(keyBoard,key,character);
               break;
         }         
      }
      
      void PreFrame(const double deltaFrameTime)
      {
         static const float PLAYER_HEIGHT = 8.0f;
         
         if (mTerrainClamp)
         {
            dtCore::Transform tx;
            GetCamera()->GetTransform(&tx);
         
            osg::Vec3 trans;
            tx.GetTranslation(trans);         
            trans.z() = mTerrain->GetHeight(trans.x(),trans.y()) + PLAYER_HEIGHT;         
            tx.SetTranslation(trans);
            GetCamera()->SetTransform(&tx);
         }
      }
      
   private:
   
      dtCore::RefPtr<dtCore::FlyMotionModel> mMotionModel;
      dtCore::RefPtr<dtTerrain::Terrain> mTerrain;
      bool mTerrainClamp,mFlyFast;
};
        

int main(int argc, char *argv[])
{
   dtCore::SetDataFilePathList(dtCore::GetDeltaRootPath()+"/examples/testTerrain/;"
                               +dtCore::GetDeltaDataPathList());   
   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);
   dtCore::RefPtr<TestTerrainApp> app;
   
   try
   {
      app = new TestTerrainApp();
      app->Run();
   }
   catch (dtUtil::Exception &ex)
   {
      std::cout << ex.What() << std::endl;
   }
   
   return 0;
}
