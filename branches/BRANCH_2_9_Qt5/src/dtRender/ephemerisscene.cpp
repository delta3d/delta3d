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

#include <dtCore/propertymacros.h>

#include <osg/StateSet>
#include <osg/Group>
#include <osg/Depth>
#include <osg/Fog>
#include <osg/Vec4>
#include <osg/CullFace>
#include <osg/LightSource>

#include <osgEphemeris/EphemerisModel.h>
#include <osgEphemeris/EphemerisData.h>

#include <dtCore/system.h>  //for setting time from system clock
#include <dtCore/shadermanager.h>

#include <dtRender/scenemanager.h> //needed to get the camera
#include <dtCore/camera.h> //needed to set the clear color to get rid of rendering artifact

#include <dtUtil/mathdefines.h>


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


   class UpdateEphemerisCameraCallback : public osg::NodeCallback
   {
   public:

      UpdateEphemerisCameraCallback(osg::Camera* trans, osg::Camera* camera)
         : mTarget(trans)
         , mCamera(camera)
      {
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         // first update subgraph to make sure objects are all moved into postion
         traverse(node,nv);

         mCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
         mCamera->setProjectionMatrix(mTarget->getProjectionMatrix());;
         mCamera->setViewMatrix(mTarget->getViewMatrix());

      }

   protected:

      virtual ~UpdateEphemerisCameraCallback() {}

      dtCore::ObserverPtr<osg::Camera>                mTarget;
      dtCore::ObserverPtr<osg::Camera>                mCamera;

   };


   osg::Node* CreateQuad(osg::Texture2D* tex, int renderBin, const osg::Vec4& color, const osg::Vec4& extents )
   {
      osg::Geometry* geo = new osg::Geometry;
      geo->setUseDisplayList( false );
      osg::Vec4Array* colors = new osg::Vec4Array;
      colors->push_back(color);
      geo->setColorArray(colors);
      geo->setColorBinding(osg::Geometry::BIND_OVERALL);
      osg::Vec3Array *vx = new osg::Vec3Array;
      vx->push_back(osg::Vec3(extents[0], extents[2], 0));
      vx->push_back(osg::Vec3(extents[1], extents[2], 0));
      vx->push_back(osg::Vec3(extents[1], extents[3], 0 ));
      vx->push_back(osg::Vec3(extents[0], extents[3], 0));
      geo->setVertexArray(vx);
      osg::Vec3Array *nx = new osg::Vec3Array;
      nx->push_back(osg::Vec3(0, 0, 1));
      geo->setNormalArray(nx);

      if(tex != NULL)
      {
         osg::Vec2Array *tx = new osg::Vec2Array;
         tx->push_back(osg::Vec2(0, 0));
         tx->push_back(osg::Vec2(1, 0));
         tx->push_back(osg::Vec2(1, 1));
         tx->push_back(osg::Vec2(0, 1));
         geo->setTexCoordArray(0, tx);

         geo->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
      }

      geo->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
      osg::Geode *geode = new osg::Geode;
      geode->addDrawable(geo);
      geode->setCullingActive(false);
      osg::StateSet* ss = geode->getOrCreateStateSet();
      
      //we cannot let reflections change the cullface side of full screen quads
      osg::CullFace* cullState = new osg::CullFace(osg::CullFace::BACK);
      ss->setAttributeAndModes(cullState, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE);
      
      ss->setMode( GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
      ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
      ss->setRenderBinDetails( renderBin, "RenderBin" );
      return geode;
   }


   class EphemerisImpl
   {
   public:
         EphemerisImpl()
            : mFogEnabled(true)
            , mFogNear(1.0f)
            , mVisibility(25000.0f)
            , mFogColor(0.84f, 0.87f, 1.0f, 1.0f)
            , mFogMode(EphemerisScene::EXP2)
            , mDateTime()
            , mFog(new osg::Fog())
            , mEphemerisModel()
         {
            //default the date time to fourth of july
            mDateTime.SetYear(2014);
            mDateTime.SetMonth(07);
            mDateTime.SetDay(04);
            mDateTime.SetHour(12);
            mDateTime.SetMinute(0);
            mDateTime.SetSecond(0);

            //set some default fog values
            mFog->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
            mFog->setMode(osg::Fog::EXP2);

            mFog->setStart(mFogNear);
            mFog->setEnd(mVisibility);
         }


         //! Create camera resulting texture
         osg::Texture2D* createRenderTexture(int tex_width, int tex_height)
         {
            // create simple 2D texture
            osg::Texture2D* texture2D = new osg::Texture2D;
            texture2D->setTextureSize(tex_width, tex_height);
            //texture2D->setInternalFormat(GL_RGBA);
            texture2D->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
            texture2D->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);

            // since we want to use HDR, setup float format
            texture2D->setInternalFormat(GL_RGBA16F_ARB);//(GL_RGBA16F_ARB);
            //texture2D->setSourceFormat(GL_RGBA);
            texture2D->setSourceType(GL_FLOAT);

            return texture2D;
         }

         //! Setup the camera to do the render to texture
         osg::Texture2D* setupCamera(osg::Camera* camera, osg::Viewport* vp)
         {
            
            // create texture to render to
            osg::Texture2D* texture = createRenderTexture((int)vp->width(), (int)vp->height());

            camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

            // set up the background color and clear mask.
            camera->setClearColor(osg::Vec4(0.0f,1.0f,0.0f,0.0f));
            camera->setClearMask(GL_NONE);
            
            // set viewport
            camera->setViewport(0, 0, (int)vp->width(), (int)vp->height());
            camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

            // tell the camera to use OpenGL frame buffer object where supported.
            camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

            camera->detach(osg::Camera::COLOR_BUFFER);

            // attach the texture and use it as the color buffer.
            camera->attach(osg::Camera::COLOR_BUFFER, texture);//, 0, 0, false, 4, 4);
            
            return texture;
         }
   
         
         void Init(dtCore::Camera& sceneCamera)
         {
            mRootNode = new osg::Group();
            mRootNode->setNodeMask(dtUtil::NodeMask::BACKGROUND);

            //create a camera to render the ephemeris
            mEphCamera = new osg::Camera();
            mEphCamera->setRenderOrder(osg::Camera::NESTED_RENDER);
            
            mRootNode->addChild(mEphCamera);
            //osg::Viewport* vp = sceneCamera.GetOSGCamera()->getViewport();
            //dtCore::RefPtr<osg::Texture2D> renderTarget = setupCamera(mEphCamera.get(), vp);

            //create the actual ephemeris
            mEphemerisModel = new osgEphemeris::EphemerisModel();
            //add in moonlight which gives a nice ambient effect at night
            mEphemerisModel->setMembers(osgEphemeris::EphemerisModel::DEFAULT_MEMBERS | osgEphemeris::EphemerisModel::MOON_LIGHT_SOURCE);//osgEphemeris::EphemerisModel::GROUND_PLANE | osgEphemeris::EphemerisModel::PLANETS | osgEphemeris::EphemerisModel::STAR_FIELD | osgEphemeris::EphemerisModel::SUN_LIGHT_SOURCE | osgEphemeris::EphemerisModel::MOON_LIGHT_SOURCE | osgEphemeris::EphemerisModel::MOON);
            mEphemerisModel->setSkyDomeRadius( 499.0f );
            mEphemerisModel->setSunLightNum(0);
            mEphemerisModel->setMoonLightNum(1);
            mEphemerisModel->setMoveWithEyePoint(true);
            mEphemerisModel->setTurbidity(2.2f);

            osg::StateSet* ss = mEphemerisModel->getOrCreateStateSet();
            dtCore::RefPtr<osg::Depth> depthState = new osg::Depth(osg::Depth::ALWAYS, 1.0f , 1.0f );
            ss->setAttributeAndModes(depthState);
            
            //this will create a large bounding volume for the ephemeris to ensure it doesn't get culled out
            BBVisitor bbv;
            mEphemerisModel->traverse(bbv);

            mEphCamera->addChild(mEphemerisModel.get());

            //give the ephemeris the same view as the scene
            //mEphCamera->setUpdateCallback(new UpdateEphemerisCameraCallback(sceneCamera.GetOSGCamera(), mEphCamera.get()));

            //create geometry to view the result of the ephemeris
            /*osg::Vec4 color(1.0, 1.0, 1.0, 1.0);
            osg::Vec4 extents(0.0, vp->width(), 0.0, vp->height());
            osg::Node* rt = CreateQuad(renderTarget.get(), -10, color, extents);
            rt->getOrCreateStateSet()->setAttributeAndModes(depthState);

            mRootNode->addChild(rt);*/
         }



      bool mFogEnabled;
      float mFogNear;
      float mVisibility;
      osg::Vec4 mFogColor;
      EphemerisScene::FogMode mFogMode;
      dtUtil::DateTime mDateTime;
      dtCore::RefPtr<osg::Fog> mFog; 
      dtCore::ObserverPtr<osg::StateSet> mFogStateSet;
      dtCore::RefPtr<osgEphemeris::EphemerisModel> mEphemerisModel;
      dtCore::RefPtr<osg::Group> mRootNode;
      dtCore::RefPtr<osg::Camera> mEphCamera;

   };

   EphemerisScene::EphemerisScene()
   : BaseClass(*EPHEMERIS_SCENE, SceneEnum::BACKGROUND)   
   , mSetToLocalTime(false)
   , mSetTimeFromSystem(false)
   , mAutoComputeFogColor(true)
   , mLatLong(19.3333f, 81.2167f)
   , mImpl(new EphemerisImpl())
   {
      SetName("EphemerisScene");
      SetFogDensity(mImpl->mVisibility);
   }

   EphemerisScene::~EphemerisScene()
   {
      delete mImpl;
   }


   void EphemerisScene::CreateScene(SceneManager& sm, const GraphicsQuality& g)
   {
      //sets up the osg ephemeris
      dtCore::Camera* cam = sm.GetSceneCamera();
      if(cam == NULL)
      {
         LOG_ERROR("Unable to create ephemeris scene without main scene camera");
         return;
      }

      mImpl->Init(*cam);

      //set default lat long, grand cayman
      SetLatitudeLongitude(mLatLong.x(), mLatLong.y());
      
      
      if(mSetToLocalTime)
      {
         SetTimeToLocalTime();
      }
      else if(mSetTimeFromSystem)
      {
         SetTimeFromSystem();
      }
      else
      {
         SetDateTime(mImpl->mDateTime);
      }

      //setup default fog state
      mImpl->mFogStateSet = sm.GetOSGNode()->getOrCreateStateSet();            
      SetVisibility(mImpl->mVisibility);
      SetFogEnable(mImpl->mFogEnabled);

      //////////////////////////////////////////////////////////
      //these are camera settings required by ephemeris   
      dtCore::Camera* camera = sm.GetSceneCamera();
      if (camera != NULL)
      {
         camera->SetClearColor(osg::Vec4(0, 0, 0, 0));
         camera->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);
      }

      BindShader();
      
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

   osg::Vec3d EphemerisScene::GetMoonPosition() const
   {
      return mImpl->mEphemerisModel->getMoonTransform()->getMatrix().getTrans();
   }

   std::string EphemerisScene::GetDateTimeAsString() const
   {
      return mImpl->mDateTime.ToString();
   }

   void EphemerisScene::SetDateTimeAsString(const std::string& timeAndDate)
   {
      bool result = false;

      if(!timeAndDate.empty())
      {
         std::istringstream iss( timeAndDate );
         // The time is stored in the universal format of:
         // yyyy-mm-ddThh:min:ss-some number
         // So we need to use a delimeter to ensure that we don't choke on the separators
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

   float EphemerisScene::GetFogDensity() const
   {
      return mImpl->mFog->getDensity();
   }

   bool EphemerisScene::GetFogEnable() const
   {
      return mImpl->mFogEnabled;
   }

   void EphemerisScene::SetFogEnable(bool enable)
   {
      mImpl->mFogEnabled = enable;
      
      if(mImpl->mFogStateSet.valid())
      {
         osg::StateSet* state = mImpl->mFogStateSet.get();

         if(mImpl->mFogEnabled)
         {
            state->setMode(GL_FOG, osg::StateAttribute::ON);
            mImpl->mFogStateSet->setAttributeAndModes(mImpl->mFog.get());
         }
         else
         {
            state->setMode(GL_FOG, osg::StateAttribute::OFF);
         }
      }
      
   }

   const osg::Vec4& EphemerisScene::GetFogColor() const
   {
      return mImpl->mFogColor;
   }

   void EphemerisScene::SetFogColor(const osg::Vec4& color)
   {
      mImpl->mFogColor = color;

      mImpl->mFog->setColor(color);

      if (mAutoComputeFogColor)
      {
         UpdateFogColor();
      }
   }

   void EphemerisScene::UpdateFogColor()
   {
      // Calculate the fog color in the direction of the sun for
      // sunrise/sunset effects.
      
      osgEphemeris::EphemerisData* ephemData = mImpl->mEphemerisModel->getEphemerisData();
      if (ephemData != nullptr)
      {
         double sunAlt = osg::RadiansToDegrees(ephemData->data[osgEphemeris::CelestialBodyNames::Sun].alt);
         
         osg::Vec4 fogColor = mImpl->mFogColor * ApproximateSkyBrightness(sunAlt);
         osg::Vec4 sunColor = mImpl->mEphemerisModel->getSunLightDiffuse();

         float red = (fogColor[0] + 2.f * sunColor[0] * sunColor[0]) / 3.f;
         float green = (fogColor[1] + 2.f * sunColor[1] * sunColor[1]) / 3.f;
         float blue = (fogColor[2] + 2.f * sunColor[2]) / 3.f;

         // interpolate between the sunrise/sunset color and the color
         // at the opposite direction of this effect. Take in account
         // the current visibility.
         float vis = GetVisibility();

         const float MAX_VISIBILITY = 20000;

         // Clamp visibility
         if (vis > MAX_VISIBILITY)
         {
            vis = MAX_VISIBILITY;
         }

         double sunRotation = osg::DegreesToRadians(-95.0);
         double heading = osg::DegreesToRadians(-95.0);

         double rotation = -(sunRotation + osg::PI) - heading;

         float inverseVis = 1.f - (MAX_VISIBILITY - vis) / MAX_VISIBILITY;
         float sif = 0.5f - cosf(osg::DegreesToRadians(sunAlt) * 2.0f) / 2.f + 0.000001f;

         float rf1 = std::abs((rotation - osg::PI) / osg::PI); // difference between eyepoint heading and sun heading (rad)
         float rf2 = inverseVis * pow(rf1 * rf1, 1.0f / sif);

         float rf3 = 1.f - rf2;

         fogColor[0] = rf3 * fogColor[0] + rf2 * red;
         fogColor[1] = rf3 * fogColor[1] + rf2 * green;
         fogColor[2] = rf3 * fogColor[2] + rf2 * blue;
         fogColor[3] = 1.0;

         // now apply the fog's color         
         mImpl->mFog->setColor(fogColor);
      }

   }

   double EphemerisScene::ApproximateSkyBrightness(double sunAlt)
   {
      if (sunAlt > 50.0)
      {
         return 1.0f;
      }
      else if (sunAlt > 40.0)
      {
         return dtUtil::MapRangeValue(sunAlt, 40.0, 50.0, 0.997, 1.0);
      }
      else if (sunAlt > 20.0)
      {
         return dtUtil::MapRangeValue(sunAlt, 20.0, 40.0, 0.962, 0.997);
      }
      else if (sunAlt > 10.0)
      {
         return dtUtil::MapRangeValue(sunAlt, 10.0, 20.0, 0.895, 0.962);
      }
      else if (sunAlt > 5.0)
      {
         return dtUtil::MapRangeValue(sunAlt, 5.0, 10.0, 0.806, 0.895);
      }
      else if (sunAlt > 0.0)
      {
         return dtUtil::MapRangeValue(sunAlt, 0.0, 5.0, 0.616, 0.806);
      }
      else if (sunAlt > -5.0)
      {
         return dtUtil::MapRangeValue(sunAlt, -5.0, 0.0, 0.350, 0.616);
      }
      else if (sunAlt > -10.0)
      {
         return dtUtil::MapRangeValue(sunAlt, -10.0, -5.0, 0.2, 0.350);
      }
      else if (sunAlt > -20.0)
      {
         return dtUtil::MapRangeValue(sunAlt, -20.0, -10.0, 0.11, 0.2);
      }
      else if (sunAlt > -90.0)
      {
         return dtUtil::MapRangeValue(sunAlt, -90.0, -20.0, 0.08, 0.11);
      }
      else
      {
         return 0.0;
      }

   }

   void EphemerisScene::SetFogMode(FogMode mode)
   {
      if (mImpl->mFogMode == mode) { return; }

      mImpl->mFogMode = mode;
      osg::Fog::Mode fm;
      //short attr = osg::StateAttribute::OFF;

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

      if (mImpl->mFogNear < 0.0f)
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
      if (dtUtil::Equivalent(distance, 0.0f))
      {
         SetFogEnable(false);
         return;
      }
      
      mImpl->mVisibility = distance;

      double sqrt_m_log01 = sqrt(-log(0.01));
      float density = sqrt_m_log01 / mImpl->mVisibility;

      mImpl->mFog->setDensity(density);
      mImpl->mFog->setEnd(mImpl->mVisibility);
   }

   float EphemerisScene::GetVisibility() const
   {
      return mImpl->mVisibility;
   }

   void EphemerisScene::SetDateTime(const dtUtil::DateTime& dt)
   {
      mImpl->mDateTime = dt;
      
      OnTimeChanged();
   }

   void EphemerisScene::OnTimeChanged()
   {
      if(mImpl->mEphemerisModel.valid())
      {
         mImpl->mEphemerisModel->setAutoDateTime( false );

         osgEphemeris::EphemerisData* ephem = mImpl->mEphemerisModel->getEphemerisData();

         dtUtil::DateTime dt = GetDateTime().GetGMTTime();

         if (ephem != NULL)
         {
            mImpl->mEphemerisModel->setDateTime(osgEphemeris::DateTime(dt.GetYear(),
                 dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), int(dt.GetSecond())));

            if (mAutoComputeFogColor)
            {
               UpdateFogColor();
            }
         }

      }
   }

   void EphemerisScene::SetTimeToLocalTime()
   {
      dtUtil::DateTime dt = GetDateTime();
      dt.SetToLocalTime();
      SetDateTime(dt);
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

   void EphemerisScene::BindShader()
   {
      dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();
      const dtCore::ShaderGroup* shaderGroup = sm.FindShaderGroupPrototype("EphemerisGroup");

      if (shaderGroup == NULL)
      {
         //try to load the default shaders
         //sm.LoadShaderDefinitions("shaders/WaterGroup.dtshader");
         sm.LoadShaderDefinitions("shaders/ShaderDefinitions.xml");
         shaderGroup = sm.FindShaderGroupPrototype("EphemerisGroup");

         if (shaderGroup == NULL)
         {
            LOG_INFO("Could not find shader group: EphemerisGroup");
            return;
         }
      }

      const dtCore::ShaderProgram* defaultShader = shaderGroup->FindShader("Ephemeris");

      try
      {
         if (defaultShader != NULL)
         {
            dtCore::ShaderManager::GetInstance().AssignShaderFromPrototype(*defaultShader, *mImpl->mEphemerisModel);
         }
         else
         {
            LOG_WARNING("Could not find ephemeris shader.'");
            return;
         }
      }
      catch (const dtUtil::Exception& e)
      {
         LOG_WARNING("Caught Exception while assigning shader: " + e.ToString());
         return;
      }
   }

   osg::LightSource* EphemerisScene::GetLightSource()
   {
      osg::LightSource* result = NULL;

      if(mImpl->mEphemerisModel.valid())
      {
         result = mImpl->mEphemerisModel->getSunLightSource();
      }

      return result;
   }

   const osg::LightSource* EphemerisScene::GetLightSource() const
   {
      const osg::LightSource* result = NULL;

      if(mImpl->mEphemerisModel.valid())
      {
         result = mImpl->mEphemerisModel->getSunLightSource();
      }

      return result;
   }

   EphemerisSceneActor::EphemerisSceneActor()
   {
   }

   EphemerisSceneActor::~EphemerisSceneActor()
   {
   }

   void EphemerisSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      EphemerisScene* es = NULL;
      GetDrawable(es);


      std::string group("EphemerisScene");
      typedef dtCore::PropertyRegHelper<EphemerisSceneActor, EphemerisScene> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, es, group);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(LatLong, "Lattitude and Longitude", "Lattitude and Longitude", 
         "This property sets the ephemeris lattitude and longitude which effects the sun, moon and star positions.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(SetTimeFromSystem, "SetTimeFromSystem", "Set Time From System Clock", 
         "Set this property to have the time set to the system clock on startup, this ignores the date time string, and assumes the system clock will be set elsewhere.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(SetToLocalTime, "SetToLocalTime", "Set To Local Time", 
         "Set this property to have the time set to the real pc clock time on startup, this ignores the date time string.",
         PropRegHelperType, propRegHelper);

      
      AddProperty(new dtCore::StringActorProperty("Date and Time", "Date and Time",
         dtCore::StringActorProperty::SetFuncType(es, &EphemerisScene::SetDateTimeAsString),
         dtCore::StringActorProperty::GetFuncType(es, &EphemerisScene::GetDateTimeAsString),
         "Sets the system clock at startup. This string must be in the following UTC format: yyyy-mm-ddThh:mm:ss.", group));


      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(FogEnable, "Enable Fog", "Enable Fog", 
         "Sets the fog state on the scene root.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(FogColor, "Fog Color", "Fog Color", 
         "The color of the fog.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(AutoComputeFogColor, "AutoComputeFogColor", "AutoComputeFogColor",
          "Setting this property auto changes the fog color based on the time of day.",
          PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(Visibility, "Visibility", "Visibility", 
         "How far things are visible before they become the full fog color.",
         PropRegHelperType, propRegHelper);

   }

   void EphemerisSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<EphemerisScene> es = new EphemerisScene();
      SetDrawable(*es);

   }


   bool EphemerisSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender
