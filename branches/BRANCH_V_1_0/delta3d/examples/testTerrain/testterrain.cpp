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
         double lon = -98.0;
         double lat = 30.0;
         mTerrain->SetOrigin(osg::Vec3d(lat,lon,0.0));
         
         // Add some sample LCC Data
         std::vector<dtTerrain::LCCType> LCCData;         
         
         dtTerrain::LCCType water(11,"water");
         water.SetRGB(179,161,95);
         
         dtTerrain::LCCType snow(12,"snow");
         snow.SetRGB(171,140,74);

         dtTerrain::LCCType rockSand(31, "rock/sand");
         rockSand.SetRGB(210,205,192);

         dtTerrain::LCCType quarry(32, "quarries");
         quarry.SetRGB(175,175,177);
         
         dtTerrain::LCCType grassland(71,"grasslands");
         grassland.SetRGB(253,233,170);

         dtTerrain::LCCType pasture(81,"pasture");
         pasture.SetRGB(252,246,93);

         dtTerrain::LCCType lowResidential(21,"low residential");
         lowResidential.SetRGB(179,161,95);
         lowResidential.SetSlope(0,20,1);
         lowResidential.SetElevation(5,2000,1);
         lowResidential.SetRelativeElevation(1,80,1);
         lowResidential.SetAspect(255);
         lowResidential.AddModel("models/house2.ive");
         
         dtTerrain::LCCType highResidential(22,"high residential");
         highResidential.SetRGB(247,178,159);
         highResidential.SetSlope(0,2000,1);
         highResidential.SetElevation(5,2000,1);
         highResidential.SetRelativeElevation(0,80,1);
         highResidential.SetAspect(255);
         highResidential.AddModel("models/house0.ive");
         highResidential.AddModel("models/house1.ive");

         dtTerrain::LCCType industrial(23,"industrial");
         industrial.SetRGB(179,168,102);
         industrial.SetSlope(0,10,1);
         industrial.SetElevation(5,500,1);
         industrial.SetRelativeElevation(0,100,1);
         industrial.SetAspect(225);
         industrial.AddModel("models/industry0.ive");
         industrial.AddModel("models/industry1.ive");
         industrial.AddModel("models/industry2.ive");

         dtTerrain::LCCType deciduous(41,"deciduous");
         deciduous.SetRGB(175,150,84);
         deciduous.SetSlope(0,500,1);
         deciduous.SetElevation(10,1280,1);
         deciduous.SetRelativeElevation(0,73,1);
         deciduous.SetAspect(225);
         deciduous.AddModel("models/cypress0.ive",1.0);
         deciduous.AddModel("models/cypress1.ive",1.0);
         deciduous.AddModel("models/cypress2.ive",1.0);

         dtTerrain::LCCType evergreen(42,"evergreen");
         evergreen.SetRGB(175,148,81);
         evergreen.SetSlope(0,50,1);
         evergreen.SetElevation(10,1400,1);
         evergreen.SetRelativeElevation(15,73,1);
         evergreen.SetAspect(255);
         evergreen.AddModel("models/grape_oregon0.ive",1.0);
         evergreen.AddModel("models/grape_oregon1.ive",1.0);
         evergreen.AddModel("models/grape_oregon2.ive",1.0);

         dtTerrain::LCCType forest(43,"mixed forest");
         forest.SetRGB(181,166,100);
         forest.SetSlope(0,50,1);
         forest.SetElevation(10,1280,1);
         forest.SetRelativeElevation(15,73,1);
         forest.SetAspect(255);
         forest.AddModel("models/maple_sugar0.ive",1.0);        
         forest.AddModel("models/maple_sugar1.ive",1.5);

         LCCData.push_back(water);
         LCCData.push_back(lowResidential);
         LCCData.push_back(industrial);
         LCCData.push_back(deciduous);
         LCCData.push_back(evergreen);
         LCCData.push_back(forest);

         // Add a decoration layer
         dtTerrain::TerrainDecorationLayer* veg = new dtTerrain::VegetationDecorator;
         
         // send over the LCCData to the vegetation decorator
         veg->SetLCCData(LCCData);

         mTerrain->AddDecorationLayer(veg);
         
         mTerrain->LoadResource("dted");
         
         dtCore::Transform tx;
         tx.SetTranslation(300.0f,300.0f,5000.0f);                          
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
               ShowStatistics();
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
      
      void ShowStatistics()
      {
         static osgUtil::Statistics::statsType currStats = osgUtil::Statistics::STAT_NONE;
         
         switch (currStats)
         {
            case osgUtil::Statistics::STAT_NONE:
               currStats = osgUtil::Statistics::STAT_FRAMERATE;
               break;
               
            case osgUtil::Statistics::STAT_FRAMERATE:
               currStats = osgUtil::Statistics::STAT_PRIMS;
               break;
               
            case osgUtil::Statistics::STAT_PRIMS:
               currStats = osgUtil::Statistics::STAT_NONE;
               break;
               
            default:
               break;
         }
         
         GetCamera()->SetStatisticsType(currStats);
      }
      
   private:
   
      dtCore::RefPtr<dtCore::FlyMotionModel> mMotionModel;
      dtCore::RefPtr<dtTerrain::Terrain> mTerrain;
      bool mTerrainClamp,mFlyFast;
};
        

int main(int argc, char *argv[])
{
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());   
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
