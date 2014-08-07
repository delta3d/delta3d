/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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
* Bradley Anderegg
*/

#include <dtRender/ephemerisscene.h>

#include <dtUtil/log.h>
#include <dtUtil/nodemask.h>

#include <osg/StateSet>
#include <osg/Group>
#include <osg/Depth>
#include <osg/Fog>
#include <osg/Vec4>

#include <osgEphemeris/EphemerisModel.h>
#include <osgEphemeris/EphemerisData.h>

#include <dtCore/system.h>  //for setting time from system clock

#include <dtRender/scenemanager.h> //needed to get the game manager
#include <dtGame/gamemanager.h> //used to get the camera
#include <dtABC/application.h> //used to get the camera
#include <dtCore/camera.h> //needed to set the clear color to get rid of rendering artifact

namespace dtRender
{
   const dtCore::RefPtr<SceneType> EphemerisScene::EPHEMERIS_SCENE(new SceneType("Ephemeris Scene", "Scene", "Uses osgEphemeris to render a sky dome."));


   struct EphemerisBoundingBoxCallback: public osg::Drawable::ComputeBoundingBoxCallback
   {
      virtual osg::BoundingBox computeBound(const osg::Drawable& d) const
      {
         return osg::BoundingBox();
      }
   };


   class BBVisitor : public osg::NodeVisitor
   {
   public:

      BBVisitor(): osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
      {

      }

      virtual void apply(osg::Geode& geode)
      {
         unsigned pNumDrawables = geode.getNumDrawables();
         for(unsigned i = 0; i < pNumDrawables; ++i)
         {
            osg::Drawable* draw = geode.getDrawable(i);
            draw->setComputeBoundingBoxCallback(new EphemerisBoundingBoxCallback());
         }

      }
   };


   class EphemerisImpl
   {
   public:
         EphemerisImpl()
            : mFogEnabled(false)
            , mFogNear(1.0f)
            , mVisibility(0.0f)
            , mFogMode(EphemerisScene::LINEAR)
            , mDateTime()
            , mFog(new osg::Fog())
            , mEphemerisModel()
         {
         }


         void Init()
         {
            mRootNode = new osg::Camera();
            
            mRootNode->setRenderOrder(osg::Camera::NESTED_RENDER);
            mRootNode->setClearMask(GL_NONE);
            mRootNode->setReferenceFrame(osg::Transform::RELATIVE_RF);

            mEphemerisModel = new osgEphemeris::EphemerisModel();
            //mEphemerisModel->setMembers(osgEphemeris::EphemerisModel::SKY_DOME);//osgEphemeris::EphemerisModel::GROUND_PLANE | osgEphemeris::EphemerisModel::PLANETS | osgEphemeris::EphemerisModel::STAR_FIELD | osgEphemeris::EphemerisModel::SUN_LIGHT_SOURCE | osgEphemeris::EphemerisModel::MOON_LIGHT_SOURCE | osgEphemeris::EphemerisModel::MOON);
            mEphemerisModel->setSkyDomeRadius( 499.0f );
            mEphemerisModel->setSunLightNum(0);
            mEphemerisModel->setMoveWithEyePoint(true);
            
            mEphemerisModel->setNodeMask(dtUtil::NodeMask::BACKGROUND);

            osgEphemeris::DateTime dt;
            dt.now();
            dt.setHour(10);
            mEphemerisModel->setAutoDateTime(false);
            mEphemerisModel->setDateTime(dt);

            // Change render order and depth writing.
            osg::StateSet* ss = mEphemerisModel->getOrCreateStateSet();
            dtCore::RefPtr<osg::Depth> depthState = new osg::Depth(osg::Depth::ALWAYS, 1.0f , 1.0f );
            ss->setAttributeAndModes(depthState);
            ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

            //this will create a large bounding volume for the ephemeris to ensure it doesn't get culled out
            BBVisitor bbv;
            mEphemerisModel->traverse(bbv);

            mRootNode->addChild(mEphemerisModel.get());
         }


      bool mFogEnabled;
      float mFogNear;
      float mVisibility;
      EphemerisScene::FogMode mFogMode;
      dtUtil::DateTime mDateTime;
      dtCore::RefPtr<osg::Fog> mFog; 
      dtCore::ObserverPtr<osg::StateSet> mFogStateSet;
      dtCore::RefPtr<osgEphemeris::EphemerisModel> mEphemerisModel;
      dtCore::RefPtr<osg::Camera> mRootNode;

   };

   EphemerisScene::EphemerisScene()
   : BaseClass(*EPHEMERIS_SCENE, SceneEnum::BACKGROUND)   
   , mImpl(new EphemerisImpl())
   {
      SetName("EphemerisScene");
   }

   EphemerisScene::~EphemerisScene()
   {
      delete mImpl;
   }


   void EphemerisScene::CreateScene(SceneManager& sm, const GraphicsQuality& g)
   {
      //sets up the osg ephemeris
      mImpl->Init();

      //set default lat long
      SetLatitudeLongitude(36.8506f, 75.9779f);

      //setup default fog state
      mImpl->mFogStateSet = sm.GetOSGNode()->getOrCreateStateSet();
      
      osg::Vec4 fogColor(0.84f, 0.87f, 1.0f, 1.0f);
      SetFogColor(fogColor);
      SetFogMode(EXP2);
      SetVisibility(16000.0f);

      mImpl->mFogStateSet->setAttributeAndModes(mImpl->mFog.get());

      SetFogEnable(true);

      //////////////////////////////////////////////////////////
      //these are camera settings required by ephemeris   
      dtGame::GameManager* gm = sm.GetGameManager();
      if (gm != NULL)
      {
         dtCore::Camera* camera = gm->GetApplication().GetCamera();
         camera->SetClearColor(osg::Vec4(0, 0, 0, 0));
         camera->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);
      }
      
   }

   osg::Group* EphemerisScene::GetSceneNode()
   {
      return mImpl->mRootNode.get();
   }

   const osg::Group* EphemerisScene::GetSceneNode() const
   {
      return mImpl->mRootNode.get();
   }

   void EphemerisScene::SetLatitudeLongitude(float latitude, float longitude)
   {
      mImpl->mEphemerisModel->setLatitudeLongitude(latitude, longitude);
   }

   osg::Vec3d EphemerisScene::GetSunPosition() const
   {
      return mImpl->mEphemerisModel->getSunPosition();
   }


   bool EphemerisScene::SetDateTimeAsString(const std::string& timeAndDate)
   {
      bool result = false;

      if(!timeAndDate.empty())
      {
         std::istringstream iss( timeAndDate );
         // The time is stored in the universal format of:
         // yyyy-mm-ddThh:min:ss-some number
         // So we need to use a delimeter to ensure that we don't choke on the seperators
         result = SetTimeAndDate( iss );
         if( !result  )
         {
            LOG_ERROR( "The input time and date string: " + timeAndDate
               + " was not formatted correctly. The correct format is: yyyy-mm-ddThh:mm:ss. Ignoring.");
         }
      }
      else
      {
         LOG_ERROR("Error setting time with empty string");
      }

      return result;
   }

   bool EphemerisScene::SetTimeAndDate(std::istringstream& iss)
   {
      unsigned year, month, day, hour, min, sec;
      char delimeter;

      iss >> year;
      if( iss.fail() ) { return false; }

      iss >> delimeter;
      if( iss.fail() ) { return false; }

      iss >> month;
      if( iss.fail() ) { return false; }

      iss >> delimeter;
      if( iss.fail() ) { return false; }

      iss >> day;
      if( iss.fail() ) { return false; }

      iss >> delimeter;
      if( iss.fail() ) { return false; }

      iss >> hour;
      if( iss.fail() ) { return false; }

      iss >> delimeter;
      if( iss.fail() ) { return false; }

      iss >> min;
      if( iss.fail() ) { return false; }

      iss >> delimeter;
      if( iss.fail() ) { return false; }

      iss >> sec;
      if( iss.fail() ) { return false; }

      dtUtil::DateTime dt;
      dt.SetTime(year, month, day, hour, min, sec);

      SetDateTime(dt);
      return true;
   }

   void EphemerisScene::SetFogDensity(float density)
   {
      mImpl->mFog->setDensity(density);
   }

   float EphemerisScene::GetFogDensity()
   {
      return mImpl->mFog->getDensity();
   }

   bool EphemerisScene::GetFogEnable() const
   {
      return mImpl->mFogEnabled;
   }

   void EphemerisScene::SetFogEnable(bool enable)
   {
      if (mImpl->mFogEnabled == enable)
      {
         return;
      }

      if(mImpl->mFogStateSet.valid())
      {
         osg::StateSet* state = mImpl->mFogStateSet.get();

         short attr = osg::StateAttribute::ON;

         if (enable)
         {
            attr = osg::StateAttribute::ON;
         }
         else
         {
            attr = osg::StateAttribute::OFF;
         }

         mImpl->mFogEnabled = enable;
         state->setMode(GL_FOG, attr);
      }
      
   }

   const osg::Vec4& EphemerisScene::GetFogColor() const
   {
      return mImpl->mFog->getColor();
   }

   void EphemerisScene::SetFogColor(const osg::Vec4& color)
   {
      mImpl->mFog->setColor(color);
   }

   void EphemerisScene::SetFogMode(FogMode mode)
   {
      if (mImpl->mFogMode == mode) { return; }

      mImpl->mFogMode = mode;
      osg::Fog::Mode fm;
      short attr = osg::StateAttribute::OFF;

      switch (mode)
      {
      case EphemerisScene::LINEAR:  fm = osg::Fog::LINEAR; break;
      case EphemerisScene::EXP:     fm = osg::Fog::EXP;    break;
      case EphemerisScene::EXP2:    fm = osg::Fog::EXP2;   break;
      default: fm = osg::Fog::LINEAR; break;
      }

      mImpl->mFog->setMode(fm);
   }


   void EphemerisScene::SetFogNear(float val )
   {

      mImpl->mFogNear = val;

      if (mImpl->mFogNear < 0.f)
      {
         mImpl->mFogNear = 0.f;
      }
      if (mImpl->mFogNear > mImpl->mVisibility)
      {
         mImpl->mFogNear = mImpl->mVisibility;
      }

      mImpl->mFog->setStart(mImpl->mFogNear);
      mImpl->mFog->setEnd(mImpl->mVisibility);

   }

   void EphemerisScene::SetVisibility(float distance)
   {
      if (mImpl->mVisibility == distance)
      {
         return;
      }

      mImpl->mVisibility = distance;

      double sqrt_m_log01 = sqrt(-log(0.01));
      float density = sqrt_m_log01 / mImpl->mVisibility;

      mImpl->mFog->setDensity(density);
      mImpl->mFog->setEnd(mImpl->mVisibility);
   }

   float EphemerisScene::GetVisibility()
   {
      return mImpl->mVisibility;
   }

   void EphemerisScene::SetDateTime( dtUtil::DateTime& dt)
   {
      mImpl->mDateTime = dt;
      
      OnTimeChanged();
   }

   void EphemerisScene::OnTimeChanged()
   {
      mImpl->mEphemerisModel->setAutoDateTime( false );

      osgEphemeris::EphemerisData* ephem = mImpl->mEphemerisModel->getEphemerisData();

      dtUtil::DateTime dt(GetDateTime().GetGMTTime());

      if (ephem != NULL)
      {
         ephem->dateTime.setYear(dt.GetYear()); // DateTime uses _actual_ year (not since 1900)
         ephem->dateTime.setMonth(dt.GetMonth());    // DateTime numbers months from 1 to 12, not 0 to 11
         ephem->dateTime.setDayOfMonth(dt.GetDay()); // DateTime numbers days from 1 to 31, not 0 to 30
         ephem->dateTime.setHour(dt.GetHour());
         ephem->dateTime.setMinute(dt.GetMinute());
         ephem->dateTime.setSecond(unsigned(dt.GetSecond()));
      }
      else
      {
         LOG_ERROR("Ephemeris Data is NULL");
      }
   }


   void EphemerisScene::SetTimeFromSystem()
   {
      dtCore::Timer_t t = dtCore::System::GetInstance().GetSimulationClockTime();
      dtUtil::DateTime dt = GetDateTime();
      dt.SetTime(time_t(t / dtCore::Timer_t(1000000)));
      SetDateTime(dt);
   }


   dtUtil::DateTime EphemerisScene::GetDateTime() const
   {
      return mImpl->mDateTime;
   }


   EphemerisSceneProxy::EphemerisSceneProxy()
   {
   }

   EphemerisSceneProxy::~EphemerisSceneProxy()
   {
   }

   void EphemerisSceneProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

   }

   void EphemerisSceneProxy::CreateDrawable()
   {
      dtCore::RefPtr<EphemerisScene> es = new EphemerisScene();
      SetDrawable(*es);
   }

   bool EphemerisSceneProxy::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender