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
#include <osg/ApplicationUsage>
#include <dtABC/application.h>
#include <dtABC/weather.h>
#include <dtCore/refptr.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/globals.h>
#include <dtTerrain/terrain.h>
#include <dtTerrain/dtedterrainreader.h>
#include <dtTerrain/soarxterrainrenderer.h>
#include <dtTerrain/geocoordinates.h>
#include <dtUtil/exception.h>
#include <dtTerrain/terraindecorationlayer.h>
#include <dtTerrain/vegetationdecorator.h>
#include <dtTerrain/lccanalyzer.h>
#include <dtTerrain/lcctype.h>
#include <dtTerrain/geotiffdecorator.h>
#include <dtTerrain/colormapdecorator.h>
#include <sstream>

class TestTerrainApp : public dtABC::Application
{
public:

   //////////////////////////////////////////////////////////////////////////
   TestTerrainApp() : dtABC::Application("config.xml"),
      mTerrainClamp(false),
      mFlyFast(false)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void CreateTerrain()
   {
      // Set up a motion model so we can move the camera
      mMotionModel = new dtCore::FlyMotionModel(GetKeyboard(), GetMouse());
      mMotionModel->SetMaximumFlySpeed(1200.0f);
      mMotionModel->SetTarget(GetCamera());         

      GetCamera()->GetSceneHandler()->GetSceneView()->setComputeNearFarMode(
         osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
         
      GetCamera()->GetLens()->setPerspective(60.0f,60.0f,1,120000.0f);
      GetCamera()->SetAutoAspect(true);         

      //Create our terrain and add a reader and renderer to it.
      mTerrain = new dtTerrain::Terrain();
      mTerrain->SetCachePath(mCachePath);
      mTerrain->AddResourcePath(mResourcePath);      

      //Create the terrain reader...
      dtTerrain::DTEDTerrainReader *reader = new dtTerrain::DTEDTerrainReader();
      switch (mDTEDLevel)
      {
         case 0:
            reader->SetDTEDLevel(dtTerrain::DTEDTerrainReader::DTEDLevelEnum::ZERO);
            break;
            
         case 1:
            reader->SetDTEDLevel(dtTerrain::DTEDTerrainReader::DTEDLevelEnum::ONE);
            break;
            
         case 2:
            reader->SetDTEDLevel(dtTerrain::DTEDTerrainReader::DTEDLevelEnum::TWO);
            break;
            
         default:
            std::cout << "Unsupported DTED level." << std::endl;
            exit(1);
      };

      //Create the terrain renderer...
      mRenderer = new dtTerrain::SoarXTerrainRenderer();
            
      //Connect our reader and renderer to the terrain.
      mTerrain->SetDataReader(reader);         
      mTerrain->SetDataRenderer(mRenderer.get());        
         
      //Before we load any resources, lets set our origin to correlate to the data we
      //are loading.
      dtCore::Transform tx;
      dtTerrain::GeoCoordinates coords;
      coords.SetLatitude(mLatitude);
      coords.SetLongitude(mLongitude);
      coords.SetAltitude(1000.0);
      tx.SetTranslation(coords.GetCartesianPoint());                          
      GetCamera()->SetTransform(&tx);         

      //Add a decorator to generate a base texture for the terrain.
      mColorMapDecorator = new dtTerrain::ColorMapDecorator();
      mTerrain->AddDecorationLayer(mColorMapDecorator.get());

      // Add the vegetation decorator layer if enabled.
      if (mEnableVegetation)
      {        
         mLCCType = CreateLCCType();  
         mVeg = new dtTerrain::VegetationDecorator;

         // Add the LCCTypes
         mVeg->SetLCCTypes(mLCCType);
         mVeg->GetLCCAnalyzer().SetOutputDebugImages(true);

         // Configure the vegetation
         mVeg->SetRandomSeed(27);
         mVeg->SetVegetationDistance(14000.0f);
         mVeg->SetMaxVegetationPerCell(2200);
         //mVeg->SetLoadDistance(15000.0f);

         // Add Geospecific Dataset
         mVeg->SetGeospecificImage(mGeospecificPath);

         // Add the vegetation decoration layer to our terrain
         mTerrain->AddDecorationLayer(mVeg.get());
      }
      
      if (!mGeoDrapePath.empty())
      {
         //Create a decorator used to drape a geotiff over the terrain.
         dtTerrain::GeoTiffDecorator *geoTiffDecorator = new dtTerrain::GeoTiffDecorator();
         geoTiffDecorator->AddGeoSpecificImage(mGeoDrapePath);
         mTerrain->AddDecorationLayer(geoTiffDecorator);
      }

      dtABC::Weather *weather = new dtABC::Weather();
      weather->SetTheme(dtABC::Weather::THEME_FAIR);
      weather->SetBasicVisibilityType(dtABC::Weather::VIS_MODERATE);
    
      mEnvironment = weather->GetEnvironment();
      mEnvironment->SetVisibility(6000.0f);
      mEnvironment->SetDateTime(2005,3,7,14,0,0);
     
      mEnvironment->AddChild(mTerrain.get());
      GetScene()->AddDrawable(mEnvironment.get());
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<dtTerrain::LCCType> CreateLCCType()
   {
      std::vector<dtTerrain::LCCType> LCCType;

      dtTerrain::LCCType water(11,"water");
      water.SetRGB(110,130,177);

      // The models referenced here are not distributed with the Delta3D
      // example-data package due to size constraints. The code remains
      // here as an example of how to load your own models for LCCTypes.

      dtTerrain::LCCType lowResidential(21,"low residential");
      lowResidential.SetRGB(253,229,228);
      lowResidential.SetSlope(0,20,1);
      lowResidential.SetElevation(5,2000,1);
      lowResidential.SetRelativeElevation(0,80,1);
      lowResidential.SetAspect(255);
      lowResidential.AddModel("models/house1.ive");
      lowResidential.AddModel("models/house3.ive");

      dtTerrain::LCCType highResidential(22,"high residential");
      highResidential.SetRGB(247,178,159);
      highResidential.SetSlope(0,20,1);
      highResidential.SetElevation(5,2000,1);
      highResidential.SetRelativeElevation(0,80,1);
      highResidential.AddModel("models/house2.ive");

      dtTerrain::LCCType industrial(23,"industrial");
      industrial.SetRGB(157,186,0);
      industrial.SetSlope(0,20,1);
      industrial.SetElevation(5,2000,1);
      industrial.SetRelativeElevation(0,80,1);
      industrial.SetAspect(225);
      industrial.AddModel("models/industry0.ive");
      industrial.AddModel("models/industry1.ive");
      industrial.AddModel("models/industry2.ive");
      
      dtTerrain::LCCType deciduous(41,"deciduous");
      deciduous.SetRGB(134,200,127);
      deciduous.SetSlope(0,50,1);
      deciduous.SetElevation(10,1280,1);
      deciduous.SetRelativeElevation(15,73,1);
      deciduous.SetAspect(225);
      deciduous.AddModel("models/Maple_silver_alone_16_1.flt/Maple_silver_alone_16_1.flt",4.0);
      deciduous.AddModel("models/Maple_silver_alone_21_1.flt/Maple_silver_alone_21_1.flt",4.0);
      deciduous.AddModel("models/Maple_silver_alone_32_1.flt/Maple_silver_alone_32_1.flt",4.0);

      dtTerrain::LCCType evergreen(42,"evergreen");
      evergreen.SetRGB(56,129,78);
      evergreen.SetSlope(0,50,1);
      evergreen.SetElevation(10,1400,1);
      evergreen.SetRelativeElevation(15,73,1);
      evergreen.SetAspect(255);
      evergreen.AddModel("models/Bull_bay_25_1.flt/Bull_bay_25_1.flt",3.5);
      evergreen.AddModel("models/Bull_bay_32_1.flt/Bull_bay_32_1.flt",3.5);
      evergreen.AddModel("models/Bull_bay_37_1.flt/Bull_bay_37_1.flt",3.5);

      dtTerrain::LCCType forest(43,"mixed forest");
      forest.SetRGB(212,231,177);
      forest.SetSlope(0,50,1);
      forest.SetElevation(10,1280,1);
      forest.SetRelativeElevation(15,73,1);
      forest.SetAspect(255);
      forest.AddModel("models/Grape_oregon_4_1.flt/Grape_oregon_4_1.flt",2.5);        
      forest.AddModel("models/Maple_silver_alone_21_1.flt/Maple_silver_alone_21_1.flt",2.5);

      dtTerrain::LCCType shrubland(51,"shrubland");
      shrubland.SetRGB(220,202,143);
      shrubland.SetSlope(0,70,1);
      shrubland.SetElevation(15,2000,1);
      shrubland.SetRelativeElevation(15,73,1);
      shrubland.SetAspect(255);
      shrubland.AddModel("models/Grape_oregon_4_1.flt/Grape_oregon_4_1.flt",3.0);
      shrubland.AddModel("models/Grape_oregon_6_1.flt/Grape_oregon_6_1.flt",3.0);
      shrubland.AddModel("models/Grape_oregon_8_1.flt/Grape_oregon_8_1.flt",3.0);

      LCCType.push_back(water);
      //LCCType.push_back(lowResidential);
      //LCCType.push_back(highResidential);
      //LCCType.push_back(industrial);
      LCCType.push_back(deciduous);
      LCCType.push_back(evergreen);
      LCCType.push_back(forest);
      LCCType.push_back(shrubland);

      return LCCType;
   }

   ////////////////////////////////////////////////////////////////////////// 
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
               mMotionModel->SetMaximumFlySpeed(4500);
            else
               mMotionModel->SetMaximumFlySpeed(1200);
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

   //////////////////////////////////////////////////////////////////////////
   virtual void KeyReleased(dtCore::Keyboard *keyBoard, Producer::KeyboardKey key,
      Producer::KeyCharacter character)
   {
      switch (key)
      {
      case Producer::Key_F:
         mFlyFast = false;
         mMotionModel->SetMaximumFlySpeed(1200);
         break;

      default:
         dtABC::Application::KeyReleased(keyBoard,key,character);
         break;
      }         
   }

   //////////////////////////////////////////////////////////////////////////
   void PreFrame(const double deltaFrameTime)
   {
      static const float PLAYER_HEIGHT = 32.0f;
      std::ostringstream ss;
      
      //Check for some keys and adjust the terrain rendering parameters as 
      //neccessary.
      if(GetKeyboard()->GetKeyState(Producer::Key_minus))
      {
         mRenderer->SetThreshold(float(mRenderer->GetThreshold()) - deltaFrameTime*5.0f);
         ss.str("");
         ss << "Threshold decreased to: " << mRenderer->GetThreshold();
         LOG_INFO(ss.str());
      }
      
      if(GetKeyboard()->GetKeyState(Producer::Key_equal))
      {
         mRenderer->SetThreshold(float(mRenderer->GetThreshold()) + deltaFrameTime*5.0f);
         ss.str("");
         ss << "Threshold increased to: " << mRenderer->GetThreshold();
         LOG_INFO(ss.str());
      }
      
      if(GetKeyboard()->GetKeyState(Producer::Key_bracketright))
      {
         mRenderer->SetDetailMultiplier(float(mRenderer->GetDetailMultiplier()) - deltaFrameTime*5.0f);
         ss.str("");
         ss << "Detail Multiplier decreased to: " << mRenderer->GetDetailMultiplier();
         LOG_INFO(ss.str());
      }
      
      if(GetKeyboard()->GetKeyState(Producer::Key_bracketleft))
      {
         mRenderer->SetDetailMultiplier(float(mRenderer->GetDetailMultiplier()) + deltaFrameTime*5.0f);
         ss.str("");
         ss << "Detail Multiplier increased to: " << mRenderer->GetDetailMultiplier();
         LOG_INFO(ss.str());
      }
      
      //Ground clamp our camera...
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

   //////////////////////////////////////////////////////////////////////////
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

   //////////////////////////////////////////////////////////////////////////
   void SetResourcePath(const std::string path)
   {
      mResourcePath = path;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetGeospecificPath(const std::string path)
   {
      mGeospecificPath = path;
   }
   
   //////////////////////////////////////////////////////////////////////////
   void SetGeospecificDrapePath(const std::string &path)
   {
      mGeoDrapePath = path;
   }
   
   //////////////////////////////////////////////////////////////////////////
   void SetEnableVegetation(bool flag) 
   {
      mEnableVegetation = flag;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetCachePath(const std::string path)
   {
      mCachePath = path;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetDTEDLevel(unsigned int level)
   {
      mDTEDLevel = level;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetLatitude(double latitude)
   {
      mLatitude = latitude;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetLongitude(double longitude)
   {
      mLongitude = longitude;
   }

private:

   // Motion Model
   dtCore::RefPtr<dtCore::FlyMotionModel> mMotionModel;

   // Terrain
   dtCore::RefPtr<dtTerrain::Terrain> mTerrain;

   // Environment
   dtCore::RefPtr<dtCore::Environment> mEnvironment;

   // Vegetation Decorator
   dtCore::RefPtr<dtTerrain::VegetationDecorator> mVeg;
   
   // Texture decorator mapping height values to color values.
   dtCore::RefPtr<dtTerrain::ColorMapDecorator> mColorMapDecorator;
   
   dtCore::RefPtr<dtTerrain::SoarXTerrainRenderer> mRenderer;

   // LCC Types
   std::vector<dtTerrain::LCCType> mLCCType;  

   // Resource Path
   std::string mResourcePath;

   // Geospecific Data Path
   std::string mGeospecificPath;
   
   std::string mGeoDrapePath;

   // Cache Path
   std::string mCachePath;

   // DTED level
   unsigned int mDTEDLevel;

   double mLatitude;
   double mLongitude;

   bool mTerrainClamp;
   bool mFlyFast;
   bool mSkyBoxEnable;
   float mFogNear;
   bool mEnableVegetation;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
   dtCore::SetDataFilePathList( dtCore::GetDeltaDataPathList() + ";" +
                                dtCore::GetDeltaRootPath() + "/examples/testTerrain" );

   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);

   dtCore::RefPtr<TestTerrainApp> app;
   
   // use an ArgumentParser object to manage the program arguments.
   osg::ArgumentParser arguments(&argc,argv);

   // set up the usage document, in case we need to print out how to use this program.
   arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
   arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" is a SOARX and GENETICS terrain demonstration application.");
   arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] value ...");
   arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line options");
   arguments.getApplicationUsage()->addCommandLineOption("-c <cachepath>","Set the cache path");
   arguments.getApplicationUsage()->addCommandLineOption("-r <resourcepath>","Set the resource path");
   arguments.getApplicationUsage()->addCommandLineOption("--dted <level> <latitude> <longitude>","Set the dted level, latitude and longitude");
   arguments.getApplicationUsage()->addCommandLineOption("--geo <geopath>","Set file for geospecific data");
   arguments.getApplicationUsage()->addCommandLineOption("--geodrape <path>","Set file for geospecific drape texture across the terrain.");
   arguments.getApplicationUsage()->addCommandLineOption("--enable-vegetation","Enables the placement of LCC vegetation.");

   // Handle our command line arguments
   // if user request help write it out to cout.
   if (arguments.read("-h") || arguments.read("--help"))
   {
      arguments.getApplicationUsage()->write(std::cout);
      return 1;
   }

   int level;
   double latitude;
   double longitude;
   std::string cachePath;
   std::string resourcePath;
   bool vegetation(false);
   std::string geospecific;
   std::string drapeImagePath;
   
   if (arguments.argc()<=1)
   {
      level = 0;
      latitude = 36.96;
      longitude = -121.96;
      cachePath = "cache";
      resourcePath = "dted/level0";
      vegetation = false;
      geospecific = "";
   }
   else
   {
      arguments.read("-c",cachePath);
      arguments.read("-r",resourcePath);
      arguments.read("--dted",level,latitude,longitude);
      arguments.read("--geo",geospecific);
      arguments.read("--geodrape",drapeImagePath);
      vegetation = arguments.read("--enable-vegetation");

      // any option left unread are converted into errors to write out later.
      arguments.reportRemainingOptionsAsUnrecognized();

      // report any errors if they have occured when parsing the program aguments.
      if (arguments.errors())
      {
         arguments.writeErrorMessages(std::cout);
         return 1;
      }
   }

   try
   {
      app = new TestTerrainApp();
      app->SetCachePath(cachePath);
      app->SetGeospecificPath(geospecific);
      app->SetResourcePath(resourcePath);
      app->SetDTEDLevel(level);
      app->SetLatitude(latitude);
      app->SetLongitude(longitude);
      app->SetEnableVegetation(vegetation);
      app->SetGeospecificDrapePath(drapeImagePath);
      app->CreateTerrain();
      app->Config();
      app->Run();
   }
   catch (dtUtil::Exception &ex)
   {
      std::cout << ex.What() << std::endl;
   }

   return 0;
}
