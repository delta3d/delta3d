/* -*-c++-*-
 * testTerrain - testterrain (.h & .cpp) - Using 'The MIT License'
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
 * Matthew W. Campbell
 */
#include <osg/ApplicationUsage>
#include <osg/ArgumentParser>

#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtABC/weather.h>

#include <dtCore/refptr.h>
#include <dtCore/environment.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/transform.h>

#include <dtTerrain/terrain.h>
#include <dtTerrain/dtedterrainreader.h>
#include <dtTerrain/soarxterrainrenderer.h>
#include <dtTerrain/geocoordinates.h>
#include <dtTerrain/terraindecorationlayer.h>
#include <dtTerrain/vegetationdecorator.h>
#include <dtTerrain/lccanalyzer.h>
#include <dtTerrain/lcctype.h>
#include <dtTerrain/geotiffdecorator.h>
#include <dtTerrain/colormapdecorator.h>

#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/keyboard.h>
#include <dtCore/generickeyboardlistener.h>

#include <dtUtil/exception.h>
#include <dtUtil/datapathutils.h>

#include <osg/Version>
#include <sstream>
#include <iostream>

// helper func that should be somewhere
std::string vec3dToString(const osg::Vec3d& pt)
{
   std::ostringstream ost;
   ost << "Vec3d("<< pt.x()<< ", " << pt.y()<< ", " << pt.z()<< " )";
   return ost.str();
}

// helper func that should be somewhere
std::string vec3ToString(const osg::Vec3& pt)
{
   std::ostringstream ost;
   ost << "Vec3 ("<< pt.x()<< ", " << pt.y()<< ", " << pt.z()<< " )";
   return ost.str();
}

class TestTerrainApp : public dtABC::Application
{
public:

   //////////////////////////////////////////////////////////////////////////
   TestTerrainApp()
      : dtABC::Application("config.xml")
      , mTerrainClamp(false)
      , mFlyFast(false)
   {
      GetKeyboardListener()->SetReleasedCallback(dtCore::GenericKeyboardListener::CallbackType(this, &TestTerrainApp::KeyReleased));
   }

protected:

   virtual ~TestTerrainApp() {}

public:

   //////////////////////////////////////////////////////////////////////////
   void CreateTerrain()
   {
      // Before we load any resources, lets set our origin to correlate to the data we
      // are loading.
      dtCore::Transform tx;
      dtTerrain::GeoCoordinates coords;
      osg::Vec3d pos;
      osg::Vec3 posV3;
      pos = coords.GetCartesianPoint();
      coords.GetCartesianPoint(posV3);
      LOG_DEBUG("Raw Geo: "+coords.ToStringAll());
      LOG_DEBUG("  gotPt: "+vec3dToString(pos));
      LOG_DEBUG("  altPt: "+vec3ToString(posV3));

      dtTerrain::GeoCoordinates origin;
      dtTerrain::GeoCoordinates::GetOrigin(origin);
      pos = coords.GetCartesianPoint();
      coords.GetCartesianPoint(posV3);
      LOG_DEBUG("Origin : "+coords.ToStringAll());
      LOG_DEBUG("  gotPt: "+vec3dToString(pos));
      LOG_DEBUG("  altPt: "+vec3ToString(posV3));

      coords.SetLatitude(mLatitude);
      coords.SetLongitude(mLongitude);
      coords.SetAltitude(0.0);
      pos = coords.GetCartesianPoint();
      coords.GetCartesianPoint(posV3);

      LOG_DEBUG("Set Geo: "+coords.ToStringAll());
      LOG_DEBUG("  gotPt: "+vec3dToString(pos));
      LOG_DEBUG("  atlPt: "+vec3ToString(posV3));

      // Set Origin to avoid floating point round off errors
      // geo <-> cartesian is done in dbl precision
      // while rest of calculations are single precision
      dtTerrain::GeoCoordinates::SetOrigin(coords);

      coords.SetAltitude(1000.0);
      coords.GetCartesianPoint(posV3);
      tx.SetTranslation(posV3);
      LOG_DEBUG("SetCam: "+vec3ToString(posV3));

      //std::cout << "Camera Pos = " <<  pos.x() << ", " << pos.y() << ", " << pos.z() << std::endl;
      GetCamera()->SetTransform(tx);

      // Set up a motion model so we can move the camera
      mMotionModel = new dtCore::FlyMotionModel(GetKeyboard(), GetMouse());
      mMotionModel->SetMaximumFlySpeed(1200.0f);
      mMotionModel->SetTarget(GetCamera());

      GetCamera()->GetOSGCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
      GetCamera()->SetPerspectiveParams(60.0f, 1.33, 1, 120000.0f);
      GetCamera()->SetProjectionResizePolicy(osg::Camera::HORIZONTAL);

      // Create our terrain and add a reader and renderer to it.
      mTerrain = new dtTerrain::Terrain();
      mTerrain->SetCachePath(mCachePath);
      mTerrain->AddResourcePath(mResourcePath);

      // Create the terrain reader...
      dtTerrain::DTEDTerrainReader* reader = new dtTerrain::DTEDTerrainReader();
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
         dtTerrain::GeoTiffDecorator* geoTiffDecorator = new dtTerrain::GeoTiffDecorator();
         geoTiffDecorator->AddGeoSpecificImage(mGeoDrapePath);
         mTerrain->AddDecorationLayer(geoTiffDecorator);
      }

      dtABC::Weather* weather = new dtABC::Weather();
      weather->SetTheme(dtABC::Weather::THEME_FAIR);

      mEnvironment = weather->GetEnvironment();
      mEnvironment->SetDateTime(2005,3,7,14,0,0);

      mEnvironment->AddChild(mTerrain.get());
      mRenderer->SetEnableFog(true);

      GetScene()->AddChild(mEnvironment.get());
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
   virtual bool KeyPressed(const dtCore::Keyboard* keyBoard, int key)
   {
      bool verdict(false);
      static bool wireFrame = false;
      switch (key)
      {
      case osgGA::GUIEventAdapter::KEY_Return:
         ShowStatistics();
         verdict = true;
         break;

      case osgGA::GUIEventAdapter::KEY_Space:
         wireFrame = !wireFrame;
         if (wireFrame)
         {
            GetScene()->SetRenderState(dtCore::Scene::FRONT_AND_BACK, dtCore::Scene::LINE);
         }
         else
         {
            GetScene()->SetRenderState(dtCore::Scene::FRONT, dtCore::Scene::FILL);
         }

         verdict = true;
         break;

      case 'g':
         mTerrainClamp = !mTerrainClamp;
         if (!mTerrainClamp)
         {
            if (mFlyFast)
            {
               mMotionModel->SetMaximumFlySpeed(4500);
            }
            else
            {
               mMotionModel->SetMaximumFlySpeed(1200);
            }
         }
         else
         {
            mMotionModel->SetMaximumFlySpeed(128);
         }

         verdict = true;
         break;

      case 'f':
         mFlyFast = true;
         mMotionModel->SetMaximumFlySpeed(2500);

         verdict = true;
         break;

      case osgGA::GUIEventAdapter::KEY_F1:
         mLabel->SetActive(!mLabel->GetActive());
         break;

      default:
         verdict = dtABC::Application::KeyPressed(keyBoard,key);
         break;
      }

      return verdict;
   }

   //////////////////////////////////////////////////////////////////////////
   virtual bool KeyReleased(const dtCore::Keyboard *keyBoard, int key)
   {
      bool handled(false);
      switch (key)
      {
      case 'f':
         mFlyFast = false;
         mMotionModel->SetMaximumFlySpeed(1200);
         handled = true;
         break;

      default:
         break;
      }

      return handled;
   }

   //////////////////////////////////////////////////////////////////////////
   void PreFrame(const double deltaFrameTime)
   {
      static const float PLAYER_HEIGHT = 32.0f;
      std::ostringstream ss;

      //Check for some keys and adjust the terrain rendering parameters as
      //neccessary.
      if (GetKeyboard()->GetKeyState('-'))
      {
         mRenderer->SetThreshold(float(mRenderer->GetThreshold()) - deltaFrameTime*5.0f);
         ss.str("");
         ss << "Threshold decreased to: " << mRenderer->GetThreshold();
         LOG_INFO(ss.str());
      }

      if (GetKeyboard()->GetKeyState('='))
      {
         mRenderer->SetThreshold(float(mRenderer->GetThreshold()) + deltaFrameTime*5.0f);
         ss.str("");
         ss << "Threshold increased to: " << mRenderer->GetThreshold();
         LOG_INFO(ss.str());
      }

      if (GetKeyboard()->GetKeyState(']'))
      {
         mRenderer->SetDetailMultiplier(float(mRenderer->GetDetailMultiplier()) - deltaFrameTime*5.0f);
         ss.str("");
         ss << "Detail Multiplier decreased to: " << mRenderer->GetDetailMultiplier();
         LOG_INFO(ss.str());
      }

      if (GetKeyboard()->GetKeyState('['))
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
         GetCamera()->GetTransform(tx);

         osg::Vec3 trans;
         tx.GetTranslation(trans);
         trans.z() = mTerrain->GetHeight(trans.x(),trans.y()) + PLAYER_HEIGHT;
         tx.SetTranslation(trans);
//         GetCamera()->SetTransform(tx);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ShowStatistics()
   {
      SetNextStatisticsType();
   }

   //////////////////////////////////////////////////////////////////////////
   void SetResourcePath(const std::string& path)
   {
      mResourcePath = path;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetGeospecificPath(const std::string& path)
   {
      mGeospecificPath = path;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetGeospecificDrapePath(const std::string& path)
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

   //////////////////////////////////////////////////////////////////////////
   void CreateHelpLabel()
   {
      mLabel = new dtABC::LabelActor();
      osg::Vec2 testSize(19.0f, 5.0f);
      mLabel->SetBackSize(testSize);
      mLabel->SetFontSize(0.8f);
      mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
      mLabel->SetText(CreateHelpLabelText());
      mLabel->SetEnableDepthTesting(false);
      mLabel->SetEnableLighting(false);

      GetCamera()->AddChild(mLabel.get());
      dtCore::Transform labelOffset(-17.0f, 22.5f, 10.5f, 0.0f, 90.0f, 0.0f);
      mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
      AddDrawable(GetCamera());
   }

   //////////////////////////////////////////////////////////////////////////
   std::string CreateHelpLabelText()
   {
      std::string testString("");
      testString += "F1: Toggle Help Screen\n";
      testString += "\n";
      testString += "f: Hold for faster fly speed\n";
      testString += "g: Toggle slow fly speed\n";
      testString += "Enter: Toggle statistics\n";
      testString += "Space: Toggle wireframe\n";

      return testString;
   }

   //////////////////////////////////////////////////////////////////////////
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

   dtCore::RefPtr<dtABC::LabelActor> mLabel;

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

   bool  mTerrainClamp;
   bool  mFlyFast;
   bool  mEnableVegetation;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/data" + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/testTerrain" + ";");

   dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_DEBUG);

   dtCore::RefPtr<TestTerrainApp> app;

   // use an ArgumentParser object to manage the program arguments.
   osg::ArgumentParser arguments(&argc, argv);

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
   bool vegetation(true);
   std::string geospecific;
   std::string drapeImagePath;

   if (arguments.argc()<=1)
   {
      level        = 0;
      latitude     = 36.96;
      longitude    = -121.96;
      cachePath    = "cache";
      resourcePath = "dted/level0";
      vegetation   = false;
      geospecific  = "";
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
      app->CreateHelpLabel();
      app->Config();
      app->Run();
   }
   catch (dtUtil::Exception& ex)
   {
      std::cout << ex.What() << std::endl;
   }

   return 0;
}
