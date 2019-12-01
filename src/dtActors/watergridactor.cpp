/* -*-c++-*-
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * @author Bradley Anderegg
 * @author Curtiss Murphy
 */

#include <dtActors/watergridactor.h>

#include <dtABC/application.h>
#include <dtABC/applicationconfigdata.h>

#include <dtActors/watergridbuilder.h>

#include <dtCore/cloudplane.h>
#include <dtCore/scene.h>
#include <dtCore/shadermanager.h>
#include <dtCore/transform.h>

#include <dtCore/actorproxyicon.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/enumactorproperty.h>
#include <dtCore/functor.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/project.h>

#include <dtGame/gamemanager.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtGame/gameactor.h>
#include <dtGame/invokable.h>
#include <dtGame/actorupdatemessage.h>

#include <dtUtil/mathdefines.h>
#include <dtUtil/noisetexture.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/nodemask.h>
#include <dtUtil/cullmask.h>

#include <osg/BlendFunc>
#include <osg/Camera>
#include <osg/CullFace>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Math>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/Projection>
#include <osg/StateSet>
#include <osg/TexGen>
#include <osg/TexGenNode>
#include <osg/TexMat>
#include <osg/Texture>
#include <osg/ClipNode>
#include <osg/ClipPlane>
#include <osg/Version>

#include <osgDB/FileUtils>
#include <osgDB/ReadFile>

#include <osgGA/GUIEventAdapter>

#include <osgViewer/GraphicsWindow>

#include <iostream>
#include <cmath>

namespace dtActors
{
   class UpdateReflectionCameraCallback : public osg::NodeCallback
   {
   public:

      UpdateReflectionCameraCallback(WaterGridActor& actor, osg::Camera* trans, osg::Camera* camera)
         : mTarget(trans)
         , mCamera(camera)
         , mWaterActor(&actor)
      {
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         // first update subgraph to make sure objects are all moved into postion
         traverse(node,nv);

         mCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
         mCamera->setProjectionMatrix(mTarget->getProjectionMatrix());;
         
         osg::Matrix viewMat = mTarget->getViewMatrix();
         osg::Matrix inverseView = mTarget->getInverseViewMatrix();
         osg::Vec3 pos = dtUtil::MatrixUtil::GetRow3(inverseView, 3);

         float waterHeight;
         osg::Vec3 waterNormal;

         mWaterActor->GetHeightAndNormalAtPoint(pos, waterHeight, waterNormal);
         //adds 1 to avoid light coming through
         if(pos.z() > waterHeight)
         {
            viewMat.preMult( osg::Matrix::scale(1.0f, 1.0f, -1.0f) );

            osg::CullFace* cullState = new osg::CullFace(osg::CullFace::FRONT);
            mCamera->getOrCreateStateSet()->setAttributeAndModes(cullState,
               osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

            osg::Uniform* reflectMode = mCamera->getOrCreateStateSet()->getOrCreateUniform("ReflectMode", osg::Uniform::FLOAT);
            reflectMode->set(-1.0f);

         }
         else
         {

            osg::Uniform* reflectMode = mCamera->getOrCreateStateSet()->getOrCreateUniform("ReflectMode", osg::Uniform::FLOAT);
            reflectMode->set(1.0f);

            osg::CullFace* cullState = new osg::CullFace(osg::CullFace::BACK);
            mCamera->getOrCreateStateSet()->setAttributeAndModes(cullState,
               osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
         }

         mCamera->setViewMatrix(viewMat);
      }

   protected:

      virtual ~UpdateReflectionCameraCallback() {}

      dtCore::ObserverPtr<osg::Camera> mTarget;
      dtCore::ObserverPtr<osg::Camera>      mCamera;
      dtCore::ObserverPtr<WaterGridActor>      mWaterActor;
   };

   ////////////////////////////////////////////////////////////////////////////////
   const int WaterGridActor::MAX_TEXTURE_WAVES(32);
   const dtUtil::RefString WaterGridActor::UNIFORM_ELAPSED_TIME("elapsedTime");
   const dtUtil::RefString WaterGridActor::UNIFORM_MAX_COMPUTED_DISTANCE("maxComputedDistance");

#if defined (__APPLE__) && OSG_VERSION_LESS_THAN(3,2,0)
   const dtUtil::RefString WaterGridActor::UNIFORM_WAVE_ARRAY("waveArray[0]");
#else
   const dtUtil::RefString WaterGridActor::UNIFORM_WAVE_ARRAY("waveArray");
#endif

   const dtUtil::RefString WaterGridActor::UNIFORM_TEXTURE_WAVE_ARRAY("TextureWaveArray");
   const dtUtil::RefString WaterGridActor::UNIFORM_REFLECTION_MAP("reflectionMap");
   const dtUtil::RefString WaterGridActor::UNIFORM_NOISE_TEXTURE("noiseTexture");
   const dtUtil::RefString WaterGridActor::UNIFORM_WAVE_TEXTURE("waveTexture");
   const dtUtil::RefString WaterGridActor::UNIFORM_SCREEN_WIDTH("ScreenWidth");
   const dtUtil::RefString WaterGridActor::UNIFORM_SCREEN_HEIGHT("ScreenHeight");
   const dtUtil::RefString WaterGridActor::UNIFORM_WATER_HEIGHT("WaterHeight");
   const dtUtil::RefString WaterGridActor::UNIFORM_CENTER_OFFSET("cameraRecenter");
   const dtUtil::RefString WaterGridActor::UNIFORM_WAVE_DIRECTION("waveDirection");
   const dtUtil::RefString WaterGridActor::UNIFORM_WATER_COLOR("WaterColor");
   const dtUtil::RefString WaterGridActor::UNIFORM_UNDERWATER_VIEW_DISTANCE("UnderWaterViewDistance");
   const dtUtil::RefString WaterGridActor::UNIFORM_TEXWAVE_RESOLUTION_SCALAR("textureWaveResolutionScalar");
   const dtUtil::RefString WaterGridActor::UNIFORM_TEXWAVE_AMPLITUDE_SCALAR("textureWaveAmpScalar");
   const dtUtil::RefString WaterGridActor::UNIFORM_TEXWAVE_SPREAD_SCALAR("textureWaveSpreadScalar");
   const dtUtil::RefString WaterGridActor::UNIFORM_TEXWAVE_STEEPNESS("textureWaveSteepness");


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(WaterGridActor::ChopSettings);
   WaterGridActor::ChopSettings WaterGridActor::ChopSettings::
      CHOP_FLAT("CHOP_FLAT", 0.0f, 20.0f);
   WaterGridActor::ChopSettings WaterGridActor::ChopSettings::
      CHOP_MILD("CHOP_MILD", 0.51f, 35.0f);
   WaterGridActor::ChopSettings WaterGridActor::ChopSettings::
      CHOP_MED("CHOP_MED", 1.0f, 65.0f);
   WaterGridActor::ChopSettings WaterGridActor::ChopSettings::
      CHOP_ROUGH("CHOP_ROUGH", 2.5f, 130.0f);


   WaterGridActor::ChopSettings::ChopSettings(const std::string &name, float rotationSpread, float texMod)
      : dtUtil::Enumeration(name), mRotationSpread(rotationSpread), mTextureWaveModifier(texMod)
   {  
      AddInstance(this);
   }


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(WaterGridActor::SeaState);//These can be used to customize sea state behavior- AMP  WaveLen  Speed
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_0("SeaState_0", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_1("SeaState_1", 1.0, 1.0, 0.4);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_2("SeaState_2", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_3("SeaState_3", 1.0, 1.0, 0.8);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_4("SeaState_4", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_5("SeaState_5", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_6("SeaState_6", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_7("SeaState_7", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_8("SeaState_8", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_9("SeaState_9", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_10("SeaState_10", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_11("SeaState_11", 1.0, 1.0, 1.0);
   WaterGridActor::SeaState WaterGridActor::SeaState::SeaState_12("SeaState_12", 1.0, 1.0, 1.0);



   WaterGridActor::SeaState::SeaState(const std::string& name, float ampMod, float waveLenMod, float speedMod)
      : dtUtil::Enumeration(name), mAmplitudeModifier(ampMod) , mWaveLengthModifier(waveLenMod), mSpeedModifier(speedMod)
   {  
      AddInstance(this);
   }


   
   ////////////////////////////////////////////////////////////////////////////////
   //This should temporarily keep us from being culled out
   //TODO: calculate bounding box
   ////////////////////////////////////////////////////////////////////////////////
   class WaterGridComputeBound : public osg::Drawable::ComputeBoundingBoxCallback
   {
   public:
      WaterGridComputeBound()
      {
      }

      /*virtual*/ osg::BoundingBox computeBound(const osg::Drawable& drawable) const
      {
         //by default its min float to max float.. so that should do the trick ;)
         return osg::BoundingBox(-FLT_MAX,-FLT_MAX,-FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
      }
   };

   class WaterCullCallback: public osg::Drawable::CullCallback
   {
   public:
      WaterCullCallback()
      {}

      virtual bool cull(osg::NodeVisitor* nv, osg::Drawable* drawable, osg::RenderInfo* renderInfo) const
      {
         return false;
      }

   private:

   };

   ////////////////////////////////////////////////////////////////////////////////
   //WATER GRID ACTOR
   ////////////////////////////////////////////////////////////////////////////////
   WaterGridActor::WaterGridActor(WaterGridActorProxy& owner)
      : BaseClass(owner)
      , mModForWaveLength(1.0f)
      , mModForSpeed(1.0f)
      , mModForSteepness(1.0f)
      , mModForAmplitude(1.0f)
      , mModForDirectionInDegrees(0.0f)
      , mUnderWaterViewDistance(15.0f)
      , mNumRows(180)
      , mNumColumns(180)
      , mUseDebugKeys(false)
      , mReflectionMapResolution(512, 512)
      , mTexWaveTextureResolution(512, 512)
      , mTexWaveResolutionScalar(2.0)
      , mTexWaveAmpScalar(1.0)
      , mTexWaveAmpInternal(1.0)
      , mTexWaveSpreadScalar(1.35)
      , mTexWaveSteepness(2.0)
      , mElapsedTime(0.0f)
      , mDeltaTime(0.0f)
      , mRenderWaveTexture(false)
      , mWireframe(false)
      , mDeveloperMode(false)
      , mComputedRadialDistance(0.0)
      , mFarDistanceBetweenVerts(1.0)
      , mNearDistanceBetweenVerts(0.0)
      , mTextureWaveAmpOverLength(1.0 / 64.0)
      , mModForFOV(1.0f)
      , mCameraFoVScalar(1.0f)
      , mWaterColor(0.117187, 0.3125, 0.58593, 1.0)
      , mLastCameraOffsetPos()
      , mCurrentCameraPos()
      , mChopEnum(&ChopSettings::CHOP_ROUGH)
      , mSeaStateEnum(&SeaState::SeaState_4)
   {
      SetName("WaterGridActor"); // Set a default name

      // Add a callback to the camera this can set uniforms on each camera.
      dtCore::Camera::AddCameraSyncCallback(*this,
         dtCore::Camera::CameraSyncCallback(this, &WaterGridActor::UpdateViewMatrix));
   }

   ////////////////////////////////////////////////////////////////////////////////
   WaterGridActor::~WaterGridActor()
   {
      dtABC::Application* app = dtABC::Application::GetInstance("Application");
      if (app != NULL)
      {
         app->GetScene()->GetSceneNode()->removeChild(mWaveCamera.get());
         app->GetScene()->GetSceneNode()->removeChild(mWaveCameraScreen.get());
         app->GetScene()->GetSceneNode()->removeChild(mReflectionCamera.get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      Update(tickMessage.GetDeltaSimTime());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::OnEnteredWorld()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::SetChop(WaterGridActor::ChopSettings &choppiness)
   {
      //std::cout << "Setting Chop to ["<< choppiness.GetName() << "]." << std::endl;
      mChopEnum = &choppiness;
   }

   ////////////////////////////////////////////////////////////////////////////////////
   WaterGridActor::ChopSettings& WaterGridActor::GetChop() const
   {
      return *mChopEnum;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::SetWaterColor(const osg::Vec4& color)
   {
      mWaterColor = color;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4 WaterGridActor::GetWaterColor() const
   {
      return mWaterColor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::Init(const dtGame::Message&)
   {
      Initialize();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::Initialize()
   {
      CreateGeometry();

      ///Added a callback to the camera this can set uniforms on each camera.
      dtCore::Camera::AddCameraSyncCallback(*this,
         dtCore::Camera::CameraSyncCallback(this, &WaterGridActor::UpdateWaveUniforms));

      if (!GetGameActorProxy().IsInSTAGE())
      {
         std::string developerMode = "false";
         developerMode = GetGameActorProxy().GetGameManager()->GetConfiguration().GetConfigPropertyValue("DeveloperMode");
         mDeveloperMode = (developerMode == "true" || developerMode == "1");
      }

      // Set the water height to the height of the water actor in the world.
      dtCore::Transform transform;
      GetTransform(transform);
      osg::Vec3 pos = transform.GetTranslation();
      transform.MakeIdentity();
      transform.SetTranslation(0.0f, 0.0f, 0.0f);
      SetTransform(transform);
      SetWaterHeight(pos.z());

      WaterGridBuilder::BuildWavesFromSeaState(mSeaStateEnum , mWaves);
      WaterGridBuilder::BuildTextureWaves(mTextureWaves);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaterGridActor::GetHeightAndNormalAtPoint(const osg::Vec3& detectionPoint,
                                                  float& outHeight, osg::Vec3& outNormal) const
   {
      osg::Vec2 point2d(detectionPoint.x(), detectionPoint.y());
      osg::Vec2 cameraPos2d(mCurrentCameraPos.x(), mCurrentCameraPos.y());

      float distanceToCamera = (point2d - cameraPos2d).length();

      float distBetweenVerts = dtUtil::MapRangeValue(distanceToCamera, 0.0f, mComputedRadialDistance, 2.0f * mNearDistanceBetweenVerts, mFarDistanceBetweenVerts);

      float cameraHeight = 10.0;//std::max(0.1f, std::abs(mCurrentCameraPos.z() - float(GetWaterHeight())));

      float scalar = std::min(10.0f, std::log(cameraHeight/20.0f + 1.0f)) + std::min(10.0f, std::max(0.0f, (cameraHeight-10.0f))/50.0f);
      scalar = 1.15f * std::max(1.1f, scalar);

      float distBetweenVertsScalar = 10.0 + (distBetweenVerts * scalar);
      
      outHeight = GetWaterHeight();

      {
         
         float xPos = detectionPoint[0];
         float yPos = detectionPoint[1];

         // There are 2 vec4's of data per wave, so the loop is MAX_WAVES * 2 but increments by 2's
         for(int i = 0; i < 16/*MAX_WAVES*/; i++)
         {
            // Order is: waveLength, speed, amp, freq, UNUSED, UNUSED, dirX, dirY
            float waveLen = mProcessedWaveData[i][0]; //waveArray[i].x;
            float speed = mProcessedWaveData[i][1]; //waveArray[i].y;
            float freq = mProcessedWaveData[i][3]; //waveArray[i].w;
            float amp = mProcessedWaveData[i][2]; //waveArray[i].z;
            float waveDirX = mProcessedWaveData[i][6]; //waveArray[i + 1].zw;
            float waveDirY = mProcessedWaveData[i][7];            
            float k = std::max(1.5f * mProcessedWaveData[i][4], 4.00001f);

            float scaledDownAmp = distBetweenVertsScalar / waveLen;
            dtUtil::Clamp(scaledDownAmp, 0.0f, 0.999f);
            amp *= 1.0f - scaledDownAmp;
            
            // This math MUST match the calculations done in water_functions.vert AND water.vert
            float mPlusPhi = (freq * (speed * mElapsedTime +
               xPos * waveDirX + waveDirY * yPos));
            float sinDir = pow((std::sin(mPlusPhi) + 1.0f) / 2.0f, k);

            outHeight += (amp * sinDir);
         }
      }


      outNormal.set(0.0f, 0.0f, 1.0f);

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::Update(float dt)
   {
      mDeltaTime = dt;
      mElapsedTime += dt;

      dtCore::Keyboard* kb = dtABC::Application::GetInstance("Application")->GetKeyboard();

      static float keyTimeOut = 0.0f;
      keyTimeOut -= dt;

      if(kb != NULL && mUseDebugKeys && keyTimeOut <= 0.0f)
      {
         if(kb->GetKeyState('9'))
         {
            mModForWaveLength *= 0.96f; // 10% less
            std::cout << "WaveLength mod changed to [" << mModForWaveLength << "]." << std::endl;
         }
         else if(kb->GetKeyState('0'))
         {
            mModForWaveLength *= 1.04f; // 10% more
            std::cout << "WaveLength mod changed to [" << mModForWaveLength << "]." << std::endl;
         }

         if (kb->GetKeyState('7'))
         {
            mModForSpeed *= 0.96f; // 10% less
            std::cout << "Speed mod changed to [" << mModForSpeed << "]." << std::endl;
         }
         else if (kb->GetKeyState('8'))
         {
            mModForSpeed *= 1.04f; // 10% more
            std::cout << "Speed mod changed to [" << mModForSpeed << "]." << std::endl;
         }

         if (kb->GetKeyState('5'))
         {
            mModForAmplitude *= 0.96f; // 10% less
            std::cout << "Amp mod changed to [" << mModForAmplitude << "]." << std::endl;
         }
         else if (kb->GetKeyState('6'))
         {
            mModForAmplitude *= 1.04f; // 10% more
            std::cout << "Amp mod changed to [" << mModForAmplitude << "]." << std::endl;
         }

         if (kb->GetKeyState('3'))
         {
            mModForDirectionInDegrees -= 2.00; // 10% less
            mModForDirectionInDegrees = (mModForDirectionInDegrees < 0.0f) ? (mModForDirectionInDegrees + 360.0f) : mModForDirectionInDegrees;
            std::cout << "Direction mod changed to [" << mModForDirectionInDegrees << "]." << std::endl;
         }
         else if (kb->GetKeyState('4'))
         {
            mModForDirectionInDegrees += 2.00f; // 10% more
            mModForDirectionInDegrees = (mModForDirectionInDegrees > 360.0f) ? (mModForDirectionInDegrees - 360.0f) : mModForDirectionInDegrees;
            std::cout << "Direction mod changed to [" << mModForDirectionInDegrees << "]." << std::endl;
         }

         if (kb->GetKeyState('1'))
         {
            mModForFOV *= 0.96f; // 10% less
            std::cout << "Mod For FOV changed to [" << mModForFOV << "]." << std::endl;
         }
         else if (kb->GetKeyState('2'))
         {
            mModForFOV *= 1.04f; // 10% more
            std::cout << "Mod For FOV changed to [" << mModForFOV << "]." << std::endl;
         }

         if(kb->GetKeyState(osgGA::GUIEventAdapter::KEY_Return))
         {
            mModForWaveLength = 1.0f; // 10% less
            mModForFOV = 1.0f; // 10% more
            mModForDirectionInDegrees = 0.0f;
            mModForAmplitude = 1.0f;
            mModForSpeed = 1.0f;
            std::cout << "Resetting ALL Dev mods to Default." << std::endl;

            mRenderWaveTexture = !mRenderWaveTexture;
            SetRenderWaveTexture(mRenderWaveTexture);
            keyTimeOut = 0.5;
         }

         
         if(kb->GetKeyState(osgGA::GUIEventAdapter::KEY_Tab))
         {
            mWireframe = !mWireframe;

            osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;

            if(mWireframe)
            {
               polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
            }
            else
            {
               polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
            }

            mGeode->getOrCreateStateSet()->setAttributeAndModes(polymode.get(),osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
            keyTimeOut = 0.5;
         }

         if(kb->GetKeyState(osgGA::GUIEventAdapter::KEY_Home))
         {
            static int testChop = 0;
            testChop++;
            testChop %= 4;

            if (testChop == 0)
            {
               SetChop(ChopSettings::CHOP_FLAT);
            }
            else if (testChop == 1)
            {
               SetChop(ChopSettings::CHOP_MILD);
            }
            else if (testChop == 2)
            {
               SetChop(ChopSettings::CHOP_MED);
            }
            else if (testChop == 3)
            {
               SetChop(ChopSettings::CHOP_ROUGH);
            }

            keyTimeOut = 0.5;
         }


         //set to 4 because that is the default
         static int testSeaState = 4;
         if(kb->GetKeyState(osgGA::GUIEventAdapter::KEY_Page_Up))
         {
            if(testSeaState == 12) testSeaState = -1;

            testSeaState++;

            SetSeaStateByNumber(testSeaState);

            std::cout << "Setting Sea State to: " << testSeaState << std::endl;

            keyTimeOut = 0.5;
         }

         if(kb->GetKeyState(osgGA::GUIEventAdapter::KEY_Page_Down))
         {
            if(testSeaState == 0) testSeaState = 13;

            testSeaState--;

            SetSeaStateByNumber(testSeaState);

            std::cout << "Setting Sea State to: " << testSeaState << std::endl;

            keyTimeOut = 0.5;
         }

      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::CreateGeometry()
   {
      mGeode = new osg::Geode();
      mGeode->setDataVariance(osg::Object::DYNAMIC);
      mGeode->setNodeMask(dtUtil::NodeMask::WATER);

      mGeometry = WaterGridBuilder::BuildRadialGrid(mNumRows, mNumColumns, mComputedRadialDistance, mNearDistanceBetweenVerts, mFarDistanceBetweenVerts);
      mGeometry->setCullCallback(new WaterCullCallback());

      BindShader(mGeode, "WaterShader");

      mGeode->addDrawable(mGeometry.get());

      osg::StateSet* ss = mGeode->getOrCreateStateSet();
      ss->setDataVariance(osg::Object::DYNAMIC);
      ss->setMode(GL_BLEND, osg::StateAttribute::ON);
      osg::BlendFunc* bf = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      ss->setAttribute(bf);
      //ss->setRenderBinDetails(0, "RenderBin");
      GetMatrixNode()->addChild(BuildSubmergedGeometry());
      GetMatrixNode()->addChild(mGeode.get());

      CreateWaveTexture();
      CreateReflectionCamera();
      
      CreateNoiseTexture();
      CreateFoamTexture();

      //add a custom compute bounding box callback
      mGeometry->setComputeBoundingBoxCallback(new WaterGridComputeBound());
   }

   void WaterGridActor::CreateFoamTexture()
   {

      
      std::string foamTextureFile = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor("textures/ShaderBase/OceanFoam.png"));
      
      if(!foamTextureFile.empty())
      {

         osg::Image* newImage = osgDB::readImageFile(foamTextureFile);
         if (newImage == NULL)
         {
            LOG_ERROR("WaterGridActor failed to load Foam Texture file [" + foamTextureFile + "].");
         }
      

         osg::Texture2D* foamTexture2D = new osg::Texture2D();
         foamTexture2D->setImage(newImage);
         foamTexture2D->dirtyTextureObject();
         foamTexture2D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR_MIPMAP_LINEAR);
         foamTexture2D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
         foamTexture2D->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
         foamTexture2D->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
         foamTexture2D->setUnRefImageDataAfterApply(true);
      
         osg::StateSet* ss = mGeode->getOrCreateStateSet();

         osg::Uniform* foamTexUniform = new osg::Uniform(osg::Uniform::SAMPLER_2D, "foamTexture");
         foamTexUniform->set(3);
         ss->addUniform(foamTexUniform);
         ss->setTextureAttributeAndModes(3, foamTexture2D, osg::StateAttribute::ON);
      }
      else
      {
         LOG_ERROR("Unable to create foam effect, unable to find texture file [" + foamTextureFile + "].");
      }
   }


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::CreateNoiseTexture()
   {
      LOG_INFO("Creating noise texture.");
      dtUtil::NoiseTexture noise3d(6, 2, 0.7, 0.5, 128, 128, 32);
      dtCore::RefPtr<osg::Image> img = noise3d.MakeNoiseTexture(GL_ALPHA);
      LOG_INFO("Finished creating noise texture.");

      mNoiseTexture = new osg::Texture3D();
      mNoiseTexture->setImage(img.get());

      mNoiseTexture->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
      mNoiseTexture->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
      mNoiseTexture->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::REPEAT );
      mNoiseTexture->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::REPEAT );
      mNoiseTexture->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );

      osg::Uniform* tex = new osg::Uniform(osg::Uniform::SAMPLER_3D, UNIFORM_NOISE_TEXTURE);
      tex->set(4);
      mGeometry->getOrCreateStateSet()->addUniform(tex);
      mGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(4, mNoiseTexture.get(), osg::StateAttribute::ON);

   }


   ////////////////////////////////////////////////////////////////////////////////
   osg::Node* WaterGridActor::BuildSubmergedGeometry()
   {
      osg::Node* quad = WaterGridBuilder::CreateQuad(NULL, -1);
      BindShader(quad, "UnderWater");

      osg::StateSet* ss = quad->getOrCreateStateSet();
     
      ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
      ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

      osg::MatrixTransform* modelview_abs = new osg::MatrixTransform;
      modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      modelview_abs->setMatrix(osg::Matrix::identity());
      modelview_abs->addChild(quad);

      osg::Projection* projection = new osg::Projection;
      projection->setMatrix(osg::Matrix::ortho2D(-1,1,-1,1));
      projection->addChild(modelview_abs);

      return projection;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::UpdateWaveUniforms(dtCore::Camera& pCamera)
   {
      osg::Camera* sceneCam = pCamera.GetOSGCamera();
      osg::StateSet* ss = sceneCam->getOrCreateStateSet();

      // Bail if there is no viewport assigned to this camera.
      if (!sceneCam->getViewport())
      {
         return;
      }

      //if(sceneCam->getViewport() != NULL)
      //{
         osg::Matrix matView, matProj, matViewProjScreenInverse, matScreen;

         matView.set(sceneCam->getViewMatrix());
         matProj.set(sceneCam->getProjectionMatrix());
         matScreen.set(sceneCam->getViewport()->computeWindowMatrix());

         osg::Matrix mvps(matView * matProj * matScreen);
         matViewProjScreenInverse.invert(mvps);

         UpdateWaterPlaneFOV(pCamera, matViewProjScreenInverse);
         UpdateScreenSpaceWaterHeight(pCamera, matView * matProj);
      //}

      osg::Uniform* screenWidth = ss->getOrCreateUniform(UNIFORM_SCREEN_WIDTH, osg::Uniform::FLOAT);
      osg::Uniform* screenHeight = ss->getOrCreateUniform(UNIFORM_SCREEN_HEIGHT, osg::Uniform::FLOAT);

      screenWidth->set(float(sceneCam->getViewport()->width()));
      screenHeight->set(float(sceneCam->getViewport()->height()));

      //set the elapsed time
      osg::Uniform* elapsedTime = ss->getOrCreateUniform(UNIFORM_ELAPSED_TIME, osg::Uniform::FLOAT);
      elapsedTime->setDataVariance(osg::Object::DYNAMIC);
      elapsedTime->set(mElapsedTime);
      elapsedTime->setDataVariance(osg::Object::DYNAMIC);

      //set the wave direction modifier
      osg::Uniform* waveDirModifier = ss->getOrCreateUniform(UNIFORM_WAVE_DIRECTION, osg::Uniform::FLOAT);
      waveDirModifier->set(mModForDirectionInDegrees);
      waveDirModifier->setDataVariance(osg::Object::DYNAMIC);

      //set the max distance
      osg::Uniform* maxComputedDistance = ss->getOrCreateUniform(UNIFORM_MAX_COMPUTED_DISTANCE, osg::Uniform::FLOAT);
      maxComputedDistance->set(mComputedRadialDistance);

      // set the water height
      osg::Uniform* waterHeight = ss->getOrCreateUniform(UNIFORM_WATER_HEIGHT, osg::Uniform::FLOAT);
      waterHeight->set(GetWaterHeight());

      //update vertex wave uniforms
      osg::Uniform* waveArray = ss->getOrCreateUniform(UNIFORM_WAVE_ARRAY, osg::Uniform::FLOAT_VEC4, MAX_WAVES * 2);
      waveArray->setDataVariance(osg::Object::DYNAMIC);

      // Update the
      osg::Uniform* centerOffset = ss->getOrCreateUniform(UNIFORM_CENTER_OFFSET, osg::Uniform::FLOAT_VEC3);
      centerOffset->set(mLastCameraOffsetPos);

      osg::Uniform* waterColor = ss->getOrCreateUniform(UNIFORM_WATER_COLOR, osg::Uniform::FLOAT_VEC4);
      waterColor->set(mWaterColor);

      osg::Uniform* underWaterViewDistance = ss->getOrCreateUniform(UNIFORM_UNDERWATER_VIEW_DISTANCE, osg::Uniform::FLOAT);
      underWaterViewDistance->set(mUnderWaterViewDistance);

      osg::Uniform* texWaveResScalar = ss->getOrCreateUniform(UNIFORM_TEXWAVE_RESOLUTION_SCALAR, osg::Uniform::FLOAT);
      texWaveResScalar->set(mTexWaveResolutionScalar);

      osg::Uniform* texWaveAmpScalar = ss->getOrCreateUniform(UNIFORM_TEXWAVE_AMPLITUDE_SCALAR, osg::Uniform::FLOAT);
      texWaveAmpScalar->set(mTexWaveAmpInternal);

      osg::Uniform* texWaveSpreadScalar = ss->getOrCreateUniform(UNIFORM_TEXWAVE_SPREAD_SCALAR, osg::Uniform::FLOAT);
      texWaveSpreadScalar->set(mTexWaveSpreadScalar);

      osg::Uniform* texWaveSteepness = ss->getOrCreateUniform(UNIFORM_TEXWAVE_STEEPNESS, osg::Uniform::FLOAT);
      texWaveSteepness->set(mTexWaveSteepness);


      // Loop through the current list of waves and put them in the uniform
      DetermineCurrentWaveSet(pCamera);
      for(int count = 0; count < MAX_WAVES; count++)
      {
         // Order is: waveLength, speed, amp, freq, UNUSED, UNUSED, dirX, dirY
         waveArray->setElement(2 * count, osg::Vec4(mProcessedWaveData[count][0], mProcessedWaveData[count][1],
            mProcessedWaveData[count][2], mProcessedWaveData[count][3]));
         waveArray->setElement(2 * count + 1, osg::Vec4(mProcessedWaveData[count][4], mProcessedWaveData[count][5],
            mProcessedWaveData[count][6], mProcessedWaveData[count][7]));
      }

      //set the FOV Modifier - uses the value from DetermineCurrentWaveSet()
      osg::Uniform* fovModifier = ss->getOrCreateUniform("modForFOV", osg::Uniform::FLOAT);
      fovModifier->set(mCameraFoVScalar * mModForFOV);
      fovModifier->setDataVariance(osg::Object::DYNAMIC);

      //set the TextureWaveChopModifier, changes the range of angles used to compute the wave directions
      osg::Uniform* twcModifier = ss->getOrCreateUniform("textureWaveChopModifier", osg::Uniform::FLOAT);
      twcModifier->set(mChopEnum->mTextureWaveModifier);
      twcModifier->setDataVariance(osg::Object::DYNAMIC);
      
   //   osg::Uniform* textureWaveArray = ss->getOrCreateUniform(UNIFORM_TEXTURE_WAVE_ARRAY, osg::Uniform::FLOAT_VEC4, MAX_TEXTURE_WAVES);
   //   textureWaveArray->setDataVariance(osg::Object::DYNAMIC);

   //   TextureWaveArray::iterator tw_iter = mTextureWaves.begin();
   //   TextureWaveArray::iterator tw_endIter = mTextureWaves.end();

   //   for(int count = 0;count < MAX_TEXTURE_WAVES; ++count)
   //   {
   //      if(tw_iter != tw_endIter)
   //      {
   //         TextureWave& wave = (*tw_iter);

   //         float freq = (2.0f * osg::PI) / wave.mWaveLength;

   //         textureWaveArray->setElement(count, osg::Vec4(wave.mWaveLength, wave.mSpeed, wave.mSteepness, freq));

   //         ++tw_iter;
   //      }
   //      else
   //      {
   //         //else disable the wave by zero-ing it out
   //         textureWaveArray->setElement(count, osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
   //      }
   //   }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::DetermineCurrentWaveSet(dtCore::Camera& pCamera)
   {
      // Camera Height is used to skip past small waves when we are high up.
      dtCore::Camera *camera = &pCamera;
      dtCore::Transform trans;
      camera->GetTransform(trans);
      const osg::Vec3 translation = trans.GetTranslation();
      float cameraHeight = translation[2] - GetWaterHeight();
      cameraHeight = (cameraHeight > 1.0) ? cameraHeight : 1.0;

      // Reset the camera center so thatwe get a LOT less jitter due to near/far clipping
      mCurrentCameraPos = trans.GetTranslation();
      osg::Vec3 camTrans(mCurrentCameraPos);
      camTrans[2] = 0.0;
      float distance = (camTrans - mLastCameraOffsetPos).length();
      if (distance > 9999.0)
      {
         mLastCameraOffsetPos = camTrans;
      }

      // FOV calculation
      // ideal FoV is estimated to be 60 Hor & 90 Vert so ... 75 avgFoV.
      float avgFoV = 0.5f * (camera->GetHorizontalFov() + camera->GetVerticalFov());
      mCameraFoVScalar = (75.0f / avgFoV);

      int count = 0;
      //float numWaves = float(mWaves.size());
      WaveArray::iterator iter = mWaves.begin();
      WaveArray::iterator endIter = mWaves.end();

      //float cameraCutPoint = 0.5 + cameraHeight / (24.0 * mModForWaveLength * mCameraFoVScalar * mModForFOV); // Used to pick waves


      // Search for the next wave that is big enough to be worth showing.
      // Camera Cut Point is an estimated value for the cut point - scaled by all the FoV modifiers.
      //bool quitLooking = false;
      //int numIgnored = 0;
      /*while(iter != endIter && !quitLooking)
      {
         Wave &nextWave = (*iter);
         if (nextWave.mWaveLength > (cameraCutPoint) || (numWaves - numIgnored) <= MAX_WAVES)
         {
            quitLooking = true;
         }
         else
         {
            ++iter;
            numIgnored ++;
         }
      }*/

      // The choppiness rotation spreads out the 8 waves so that they come at wider angles
      // which causes then to be choppier.
      float choppinessRotationAmount = mChopEnum->mRotationSpread * mModForAmplitude;
      float choppinessSign = 1.0;
      for(;count < MAX_WAVES * 2;)
      {
         if(iter != endIter)
         {
            Wave& wave = (*iter);
            // weaken the amp as it reaches the pop point to hide some of the popping
            //float fadeRatio = sqrt((wave.mWaveLength - cameraCutPoint) / cameraCutPoint);
            float amp = wave.mAmplitude * mModForAmplitude;
            float waveLength = wave.mWaveLength * mModForWaveLength;
            float speed = wave.mSpeed * mModForSpeed;

            float freq = (2.0f * osg::PI) / waveLength;

            float steepness = 1.0 + 2.0 * wave.mSteepness;
            steepness = dtUtil::Max(steepness, 1.0f);

            //don't bind waves that have a zero amplitude
            if(amp > 0.00001f)
            {
               choppinessSign *= -1.0f;
               float choppinessRotationOffset = choppinessSign * choppinessRotationAmount * count;
               float curWaveDir = wave.mDirectionInDegrees + choppinessRotationOffset + choppinessRotationAmount * count/2.0f;
               float dirX = sin(osg::DegreesToRadians(curWaveDir + mModForDirectionInDegrees));
               float dirY = cos(osg::DegreesToRadians(curWaveDir + mModForDirectionInDegrees));

               mProcessedWaveData[count/2][0] = waveLength * mSeaStateEnum->mWaveLengthModifier;
               mProcessedWaveData[count/2][1] = speed * mSeaStateEnum->mSpeedModifier;
               mProcessedWaveData[count/2][2] = amp * mSeaStateEnum->mAmplitudeModifier;
               mProcessedWaveData[count/2][3] = freq;
               mProcessedWaveData[count/2][4] = steepness;
               mProcessedWaveData[count/2][5] = 1.0f;
               mProcessedWaveData[count/2][6] = dirX;
               mProcessedWaveData[count/2][7] = dirY;
               count += 2;
            }

            ++iter;
         }
         else
         {
            //else disable the wave by zero-ing it out
            mProcessedWaveData[count/2][0] = 0.0;
            mProcessedWaveData[count/2][1] = 0.0;
            mProcessedWaveData[count/2][2] = 0.0;
            mProcessedWaveData[count/2][3] = 0.0;
            mProcessedWaveData[count/2][4] = 1.0;
            mProcessedWaveData[count/2][5] = 1.0;
            mProcessedWaveData[count/2][6] = 0.0;
            mProcessedWaveData[count/2][7] = 0.0;
            count += 2;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::UpdateViewMatrix(dtCore::Camera& pCamera)
   {
      osg::StateSet* ss = pCamera.GetOSGCamera()->getOrCreateStateSet();
      osg::Uniform* viewInverseUniform = ss->getOrCreateUniform("inverseViewMatrix", osg::Uniform::FLOAT_MAT4);
      osg::Uniform* mvpiUniform = ss->getOrCreateUniform("modelViewProjectionInverse", osg::Uniform::FLOAT_MAT4);
      osg::Uniform* hprUniform = ss->getOrCreateUniform("cameraHPR", osg::Uniform::FLOAT_VEC3);

      osg::Matrix matWorld, matView, matViewInverse, matProj, matProjInverse, matViewProj, matViewProjInverse;
      
      matView.set(pCamera.GetOSGCamera()->getViewMatrix());
      matViewInverse.invert(matView);

      matProj.set(pCamera.GetOSGCamera()->getProjectionMatrix());
      matProjInverse.invert(matProj);

      matViewProj = matView * matProj;
      matViewProjInverse.invert(matViewProj);

      mvpiUniform->set(matViewProjInverse);
      viewInverseUniform->set(matViewInverse);

      dtCore::Transform trans;
      pCamera.GetTransform(trans);

      osg::Vec3 hpr;
      trans.GetRotation(hpr);
      hprUniform->set(hpr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::CreateWaveTexture()
   {
      if(!mWaveTexture.valid())
      {
         mWaveCamera = new osg::Camera();
         mWaveCamera->setRenderOrder(osg::Camera::PRE_RENDER, 1);
         mWaveCamera->setClearMask(GL_NONE);

         mWaveTexture = WaterGridBuilder::CreateTexture(mTexWaveTextureResolution.x(), mTexWaveTextureResolution.y(), true);
         InitAndBindToTarget(mWaveCamera.get(), mWaveTexture.get(), mTexWaveTextureResolution.x(), mTexWaveTextureResolution.y(), true);
         AddOrthoQuad(mWaveCamera.get(), NULL, "TextureWave", "");

         mWaveCameraScreen = new osg::Camera();
         mWaveCameraScreen->setRenderOrder(osg::Camera::POST_RENDER, 1);
         mWaveCameraScreen->setClearMask(GL_NONE);
         mWaveCameraScreen->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
         mWaveCameraScreen->setProjectionMatrixAsOrtho2D(-10.0, 10.0, -10.0, 10.0);
         mWaveCameraScreen->setViewport(0, 0, mTexWaveTextureResolution.x(), mTexWaveTextureResolution.y());
         mWaveCameraScreen->setGraphicsContext(new osgViewer::GraphicsWindowEmbedded());
         AddOrthoQuad(mWaveCameraScreen.get(), mWaveTexture.get(), "WaveTest", "waveTexture");
         mWaveCameraScreen->setNodeMask(dtUtil::NodeMask::NOTHING);

         dtABC::Application::GetInstance("Application")->GetScene()->GetSceneNode()->addChild(mWaveCamera.get());
         dtABC::Application::GetInstance("Application")->GetScene()->GetSceneNode()->addChild(mWaveCameraScreen.get());
      }

      osg::Uniform* tex = new osg::Uniform(osg::Uniform::SAMPLER_2D, UNIFORM_WAVE_TEXTURE);
      tex->set(2);
      mGeometry->getOrCreateStateSet()->addUniform(tex);
      mGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(2, mWaveTexture.get(), osg::StateAttribute::ON);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::AddOrthoQuad(osg::Camera* cn, osg::Texture2D* tx, const std::string& shader, const std::string& texUniform)
   {
      osg::Node* quad = WaterGridBuilder::CreateQuad(tx, 50);
      cn->addChild(quad);
      BindShader(quad, shader);

      if(tx != NULL)
      {
         BindTextureUniformToNode(quad, tx, texUniform, 0);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::InitAndBindToTarget(osg::Camera* cn, osg::Texture2D* tx, int width, int height, bool mipMap)
   {
      
      cn->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      cn->setProjectionMatrixAsOrtho2D(-10.0, 10.0, -10.0, 10.0);
      cn->setViewport(0, 0, width, height);
      cn->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
      cn->detach(osg::Camera::COLOR_BUFFER);
      if(mipMap)
      {
         cn->attach(osg::Camera::COLOR_BUFFER, tx, 0, 0, true);
      }
      else
      {
         cn->attach(osg::Camera::COLOR_BUFFER, tx);
      }
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::BindTextureUniformToNode(osg::Node* node, osg::Texture2D* tex, const std::string& name, unsigned texUnit)
   {
      osg::StateSet* ss = node->getOrCreateStateSet();
      osg::Uniform* uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D, name);
      uniform->set(int(texUnit));
      ss->addUniform(uniform);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::BindShader(osg::Node* node, const std::string& shaderName)
   {
      dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();
      const dtCore::ShaderGroup* shaderGroup = sm.FindShaderGroupPrototype("WaterGroup");

      if (shaderGroup == NULL)
      {
         //try to load the default shaders
         //sm.LoadShaderDefinitions("shaders/WaterGroup.dtshader");
         sm.LoadShaderDefinitions("Shaders/ShaderDefinitions.xml");
         shaderGroup = sm.FindShaderGroupPrototype("WaterGroup");

         if (shaderGroup == NULL)
         {
            LOG_INFO("Could not find shader group: WaterGroup");
            return;
         }
      }

      const dtCore::ShaderProgram* defaultShader = shaderGroup->FindShader(shaderName);

      try
      {
         if (defaultShader != NULL)
         {
            dtCore::ShaderManager::GetInstance().AssignShaderFromPrototype(*defaultShader, *node);
         }
         else
         {
            LOG_WARNING("Could not find shader '" + shaderName);
            return;
         }
      }
      catch (const dtUtil::Exception& e)
      {
         LOG_WARNING("Caught Exception while assigning shader: " + e.ToString());
         return;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::SetRenderWaveTexture(bool b)
   {
      if(b)
      {
         mWaveCameraScreen->setNodeMask(dtUtil::NodeMask::FOREGROUND);
      }
      else
      {
         mWaveCameraScreen->setNodeMask(dtUtil::NodeMask::NOTHING);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::CreateReflectionCamera()
   {
      mReflectionCamera = new osg::Camera();

      mReflectionTexture = WaterGridBuilder::CreateTexture(mReflectionMapResolution.x(), mReflectionMapResolution.y(), true);
      InitAndBindToTarget(mReflectionCamera.get(), mReflectionTexture.get(), mReflectionMapResolution.x(), mReflectionMapResolution.y(), true);

      mReflectionCamera->setRenderOrder(osg::Camera::PRE_RENDER);
      mReflectionCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      mReflectionCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

      //turn off water bit, reflection shouldnt have water
      mReflectionCamera->setCullMask(dtUtil::NodeMask::BACKGROUND | dtUtil::NodeMask::TERRAIN_GEOMETRY | dtUtil::NodeMask::TRANSPARENT_EFFECTS | dtUtil::NodeMask::DEFAULT_GEOMETRY | dtUtil::NodeMask::MAT_MASKS | dtUtil::NodeMask::LIGHT_MASKS );

      ResetReflectionUpdate();

      AddReflectionGroup(mReflectionCamera.get());

      dtABC::Application::GetInstance("Application")->GetScene()->GetSceneNode()->addChild(mReflectionCamera.get());

      osg::Uniform* tex = new osg::Uniform(osg::Uniform::SAMPLER_2D, UNIFORM_REFLECTION_MAP);
      tex->set(1);
      osg::StateSet* ss = mGeometry->getOrCreateStateSet();
      ss->addUniform(tex);
      ss->setTextureAttributeAndModes(1, mReflectionTexture.get(), osg::StateAttribute::ON);

      // Debug display
      /*{
         osg::Matrix orthoMatrix;
         orthoMatrix.makeOrtho2D(-40.0f, 40.0f, -40.0f, 40.0f);

         osg::Projection* proj = new osg::Projection(orthoMatrix);

         osg::MatrixTransform* ident = new osg::MatrixTransform(osg::Matrix::translate(20.0f, 20.0f, 0.0f));
         ident->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

         proj->addChild(ident);

         osg::Node* quad = WaterGridBuilder::CreateQuad(mReflectionTexture.get(), 50);
         ident->addChild(quad);

         osg::Depth* depth = new osg::Depth;
         depth->setFunction(osg::Depth::ALWAYS);
         depth->setRange(0.0f, 0.0f);

         osg::StateSet* quadState = quad->getOrCreateStateSet();
         quadState->setAttributeAndModes(depth, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
         quadState->setRenderBinDetails(50, "RenderBin");

         dtABC::Application::GetInstance("Application")->GetScene()->GetSceneNode()->addChild(proj);
      }*/

   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::SetEnableWater(bool enable)
   {
      mGeode->setNodeMask((enable) ? dtUtil::NodeMask::WATER : 0x0);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::AddReflectionGroup(osg::Camera* cam)
   {
      if (!mReflectionGroup.valid())
      {
         mReflectionGroup = new osg::MatrixTransform();

         if (mReflectionScene.valid())
         {
            mReflectionGroup->addChild(mReflectionScene.get());
         }

         cam->addChild(mReflectionGroup.get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::SetReflectionScene(osg::Node* sceneNode)
   {
      mReflectionScene = sceneNode;

      if (mReflectionGroup.valid())
      { 
         // Cut the previous scene if it exists
         mReflectionGroup->removeChildren(0, mReflectionGroup->getNumChildren());

         mReflectionGroup->addChild(mReflectionScene.get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::ResetReflectionUpdate()
   {
      if (mReflectionCamera.valid() && mSceneCamera.valid())
      {
         mReflectionCamera->setUpdateCallback(new UpdateReflectionCameraCallback(*this,
            mSceneCamera.get(), mReflectionCamera.get()));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::SetSceneCamera(dtCore::Camera* sceneCamera)
   {
      mSceneCamera = sceneCamera->GetOSGCamera();
      
      //set this global state to allow things to know when they are being rendered in a reflection
      osg::Uniform* reflectModeBase = mSceneCamera->getOrCreateStateSet()->getOrCreateUniform("ReflectMode", osg::Uniform::FLOAT);
      reflectModeBase->set(1.0f);

   }

   ///////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::UpdateWaterPlaneFOV(dtCore::Camera& pCamera, const osg::Matrix& inverseMVP)
   {
      osg::StateSet* ss = pCamera.GetOSGCamera()->getOrCreateStateSet();
      osg::Uniform* waterFOVUniform = ss->getOrCreateUniform("waterPlaneFOV", osg::Uniform::FLOAT);
      waterFOVUniform->setDataVariance(osg::Object::DYNAMIC);

      dtCore::Transform xform;
      osg::Vec3d waterCenter, screenPosOut, camPos, waterPos;
      pCamera.GetTransform(xform);
      xform.GetTranslation(camPos);

      // Use the actual water actor's transform as an offset from the camera.
      GetTransform(xform);
      xform.GetTranslation(waterPos);

      float waterHeight = GetWaterHeight();
      waterCenter.set(camPos.x(), camPos.y(), waterHeight);

      if(pCamera.ConvertWorldCoordinateToScreenCoordinate(waterCenter, screenPosOut))
      {
         waterFOVUniform->set(180.0f);
      }
      else
      {
         int width = int(pCamera.GetOSGCamera()->getViewport()->width());
         int height = int(pCamera.GetOSGCamera()->getViewport()->height());

         osg::Vec3 bottomLeft, bottomRight, topLeft, topRight;
         osg::Vec3 bottomLeftIntersect, bottomRightIntersect, topLeftIntersect, topRightIntersect;

         ComputeRay(0, 0, inverseMVP, bottomLeft);
         ComputeRay(width, 0, inverseMVP, bottomRight);
         ComputeRay(0, height, inverseMVP, topLeft);
         ComputeRay(width, height, inverseMVP, topRight);

         osg::Vec4 waterPlane(0.0, 0.0, 1.0, -waterHeight);

         bool bool_bottomLeftIntersect = IntersectRayPlane(waterPlane, camPos, bottomLeft, bottomLeftIntersect);
         bool bool_bottomRightIntersect = IntersectRayPlane(waterPlane, camPos, bottomRight, bottomRightIntersect);
         bool bool_topLeftIntersect = IntersectRayPlane(waterPlane, camPos, topLeft, topLeftIntersect);
         bool bool_topRightIntersect = IntersectRayPlane(waterPlane, camPos, topRight, topRightIntersect);

         if(bool_bottomLeftIntersect)
         {
            bottomLeft = bottomLeftIntersect;
            bottomLeft = bottomLeft - waterCenter;
            bottomLeft.normalize();
         }
         if(bool_bottomRightIntersect)
         {
            bottomRight = bottomRightIntersect;
            bottomRight = bottomRight - waterCenter;
            bottomRight.normalize();
         }
         if(bool_topLeftIntersect)
         {
            topLeft = topLeftIntersect;
            topLeft = topLeft - waterCenter;
            topLeft.normalize();
         }
         if(bool_topRightIntersect)
         {
            topRight = topRightIntersect;
            topRight = topRight - waterCenter;
            topRight.normalize();
         }

         float maxAngle1 = 0.0, maxAngle2 = 0.0, maxAngle3 = 0.0, maxAngle4 = 0.0, maxAngle5 = 0.0, maxAngle6 = 0.0;

         maxAngle1 = GetAngleBetweenVectors(bottomLeft, bottomRight);

         maxAngle2 = GetAngleBetweenVectors(bottomLeft, topLeft);

         maxAngle3 = GetAngleBetweenVectors(bottomRight, topRight);

         maxAngle4 = GetAngleBetweenVectors(topLeft, topRight);

         maxAngle5 = GetAngleBetweenVectors(bottomRight, topLeft);

         maxAngle6 = GetAngleBetweenVectors(bottomLeft, topRight);

         //take the max of the six angles
         float angle = dtUtil::Max(dtUtil::Max(maxAngle5, maxAngle6), dtUtil::Max(dtUtil::Max(maxAngle1, maxAngle2), dtUtil::Max(maxAngle3, maxAngle4)));
         angle = osg::RadiansToDegrees(angle);
         angle /= 2.0f;
         waterFOVUniform->set(angle);

         //std::cout << "Water Angle " << angle << std::endl;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::ComputeRay(int x, int y, const osg::Matrix& inverseMVPS, osg::Vec3& rayToFill)
   {
      osg::Vec3 rayFrom, rayTo;

      rayFrom = osg::Vec3(x, y, 0.0f) * inverseMVPS;
      rayTo = osg::Vec3(x, y, 1.0f) * inverseMVPS;

      rayToFill = rayTo - rayFrom;
      rayToFill.normalize();
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   bool WaterGridActor::IntersectRayPlane(const osg::Vec4& plane, const osg::Vec3& rayOrigin, const osg::Vec3& rayDirection, osg::Vec3& intersectPoint)
   {
      osg::Vec3 norm(plane.x(), plane.y(), plane.z());
      float denominator = norm * rayDirection;

      //the normal is near parallel
      if(fabs(denominator) > FLT_EPSILON)
      {
         float t = -(norm * rayOrigin + plane.w());
         t /= denominator;
         intersectPoint = rayOrigin + (rayDirection * t);
         return t > 0;
      }

      //std::cout << "No Intersect" << std::endl;
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   float WaterGridActor::GetAngleBetweenVectors(const osg::Vec3& v1, const osg::Vec3& v2)
   {
      return std::acos(v1 * v2);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::UpdateScreenSpaceWaterHeight(dtCore::Camera& pCamera, const osg::Matrix& MVP)
   {
      osg::StateSet* ss = pCamera.GetOSGCamera()->getOrCreateStateSet();
      osg::Uniform* waterHeightScreenSpace = ss->getOrCreateUniform("waterHeightScreenSpace", osg::Uniform::FLOAT_VEC3);
      waterHeightScreenSpace->setDataVariance(osg::Object::DYNAMIC);

      dtCore::Transform xform;
      osg::Vec3d waterCenter, screenPosOut, camPos;
      osg::Vec3 right, up, forward;
      pCamera.GetTransform(xform);
      xform.GetTranslation(camPos);
      xform.GetOrientation(right, up, forward);

      double vfov, aspect, nearClip, farClip;
      pCamera.GetPerspectiveParams(vfov, aspect, nearClip, farClip);

      osg::Vec3 posOnFarPlane = camPos + (forward * farClip);
      posOnFarPlane[2] = GetWaterHeight();

      posOnFarPlane = posOnFarPlane * MVP;

      waterHeightScreenSpace->set(posOnFarPlane);
   }


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::SetSeaState(WaterGridActor::SeaState& seaState)
   {
      mSeaStateEnum = &seaState;

      if(seaState == SeaState::SeaState_0)
      {
         mTexWaveAmpInternal = 0.05;
      }
      else if(seaState == SeaState::SeaState_1)
      {
         mTexWaveAmpInternal = mTexWaveAmpScalar * 0.1;
      }
      else if(seaState == SeaState::SeaState_2)
      {
         mTexWaveAmpInternal = mTexWaveAmpScalar * 0.2;
      }
      else if(seaState == SeaState::SeaState_3)
      {
         mTexWaveAmpInternal = mTexWaveAmpScalar * 0.35;
      }
      else if(seaState == SeaState::SeaState_4)
      {
         mTexWaveAmpInternal = mTexWaveAmpScalar * 0.5;
      }
      else if(seaState == SeaState::SeaState_5)
      {
         mTexWaveAmpInternal = mTexWaveAmpScalar * 0.75;
      }
      else
      {
         mTexWaveAmpInternal = mTexWaveAmpScalar * 1.0;
      }

      mWaves.clear();
      WaterGridBuilder::BuildWavesFromSeaState(&seaState, mWaves);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   WaterGridActor::SeaState& WaterGridActor::GetSeaState() const
   {
      return *mSeaStateEnum;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void WaterGridActor::SetSeaStateByNumber(unsigned force)
   {
      if(force == 0)
      {
         SetSeaState(SeaState::SeaState_0);
      }
      else if(force == 1)
      {
         SetSeaState(SeaState::SeaState_1);
      }
      else if(force == 2)
      {
         SetSeaState(SeaState::SeaState_2);
      }
      else if(force == 3)
      {
         SetSeaState(SeaState::SeaState_3);
      }
      else if(force == 4)
      {
         SetSeaState(SeaState::SeaState_4);
      }
      else if(force == 5)
      {
         SetSeaState(SeaState::SeaState_5);
      }
      else if(force == 6)
      {
         SetSeaState(SeaState::SeaState_6);
      }
      else if(force == 7)
      {
         SetSeaState(SeaState::SeaState_7);
      }
      else if(force == 8)
      {
         SetSeaState(SeaState::SeaState_8);
      }
      else if(force == 9)
      {
         SetSeaState(SeaState::SeaState_9);
      }
      else if(force == 10)
      {
         SetSeaState(SeaState::SeaState_10);
      }
      else if(force == 11)
      {
         SetSeaState(SeaState::SeaState_11);
      }
      else if(force == 12)
      {
         SetSeaState(SeaState::SeaState_12);            
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   //WATER GRID PROXY
   ////////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString WaterGridActorProxy::CLASSNAME("WaterGridActor");
   const dtUtil::RefString WaterGridActorProxy::PROPERTY_CHOPPINESS("Chop");
   const dtUtil::RefString WaterGridActorProxy::PROPERTY_WATER_COLOR("Water Color");
   const dtUtil::RefString WaterGridActorProxy::PROPERTY_SCENE_CAMERA("Scene Camera Name");
   const dtUtil::RefString WaterGridActorProxy::INVOKABLE_MAP_LOADED("Map Loaded");
   const dtUtil::RefString WaterGridActorProxy::INVOKABLE_ACTOR_CREATED("Actor Created");
   const dtUtil::RefString WaterGridActorProxy::INVOKABLE_ACTOR_UPDATE("Actor Updated");

   WaterGridActorProxy::WaterGridActorProxy()
   : mWaveDirection(0.0f)
   , mAmplitudeModifier(1.0f)
   , mWavelengthModifier(1.0f)
   , mSpeedModifier(1.0f)
   , mSceneCameraName(dtABC::Application::GetDefaultConfigData().CAMERA_NAME)
   {
      SetClassName(WaterGridActorProxy::CLASSNAME);
   }

   ////////////////////////////////////////////////////////////////////////////////
   WaterGridActorProxy::~WaterGridActorProxy()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::CreateDrawable()
   {
      WaterGridActor* actor = new WaterGridActor(*this);
     
      SetDrawable(*actor);

      //if (IsInSTAGE())
      //{
      //   actor->Initialize();
      //   ResetSceneCamera();
      //   actor->ResetReflectionUpdate();
      //}
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::BuildInvokables()
   {
      BaseClass::BuildInvokables();

      WaterGridActor* wga = GetDrawable<WaterGridActor>();

      AddInvokable(*new dtGame::Invokable(INVOKABLE_MAP_LOADED,
         dtUtil::MakeFunctor(&WaterGridActor::Init, wga)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
         dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);

      RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED,
         INVOKABLE_MAP_LOADED);
   }

   //////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::OnMapLoadEnd()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUPNAME = "WaterGrid";

      BaseClass::BuildPropertyMap();

      WaterGridActor* actor = GetDrawable<WaterGridActor>();
      
      typedef dtCore::PropertyRegHelper<WaterGridActorProxy> RegHelperType;
      RegHelperType propReg(*this, this, GROUPNAME);

      DT_REGISTER_PROPERTY(WaveDirection, "The direction the waves are moving.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AmplitudeModifier, "A percentage multiplied times the amplitude.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(WavelengthModifier, "A percentage multiplied with the wave length.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SpeedModifier, "A percentage multiplied with the speed.", RegHelperType, propReg);


      AddProperty(new dtCore::ColorRgbaActorProperty(PROPERTY_WATER_COLOR, PROPERTY_WATER_COLOR,
         dtCore::ColorRgbaActorProperty::SetFuncType(actor, &WaterGridActor::SetWaterColor),
         dtCore::ColorRgbaActorProperty::GetFuncType(actor,&WaterGridActor::GetWaterColor),
         "Sets the color of the water.", GROUPNAME));

      AddProperty(new dtCore::EnumActorProperty<WaterGridActor::ChopSettings>(PROPERTY_CHOPPINESS, PROPERTY_CHOPPINESS,
         dtCore::EnumActorProperty<WaterGridActor::ChopSettings>::SetFuncType(actor, &WaterGridActor::SetChop),
         dtCore::EnumActorProperty<WaterGridActor::ChopSettings>::GetFuncType(actor, &WaterGridActor::GetChop),
         "Sets the choppiness for the water.", GROUPNAME));

      AddProperty(new dtCore::StringActorProperty(PROPERTY_SCENE_CAMERA, PROPERTY_SCENE_CAMERA,
         dtCore::StringActorProperty::SetFuncType(this, &WaterGridActorProxy::SetSceneCamera),
         dtCore::StringActorProperty::GetFuncType(this, &WaterGridActorProxy::GetSceneCamera),
         "Sets the name of the camera used to render the scene.", GROUPNAME));

      AddProperty(new dtCore::EnumActorProperty<WaterGridActor::SeaState>("Sea State", "Sea State",
         dtCore::EnumActorProperty<WaterGridActor::SeaState>::SetFuncType(actor, &WaterGridActor::SetSeaState),
         dtCore::EnumActorProperty<WaterGridActor::SeaState>::GetFuncType(actor, &WaterGridActor::GetSeaState),
         "The Sea State number based on the Beaufort wind force scale.", GROUPNAME));
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& WaterGridActorProxy::GetRenderMode()
   {
      return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::SetSceneCamera(const std::string& name)
   {
      mSceneCameraName = name;
      ResetSceneCamera();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string WaterGridActorProxy::GetSceneCamera() const
   {
      return mSceneCameraName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* WaterGridActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         dtCore::ActorProxyIcon::ActorProxyIconConfig config;
         config.mForwardVector = false;
         config.mUpVector = false;
         config.mScale = 1.0;

         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH, config);
      }

      return mBillBoardIcon.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::ResetSceneCamera()
   {
      WaterGridActor* waterActor;
      GetDrawable(waterActor);

      dtCore::Camera* sceneCamera = dtCore::Camera::GetInstance(mSceneCameraName);

      // If the specified camera exists, use it, O.W. grab the first available
      if (sceneCamera)
      {
         waterActor->SetSceneCamera(sceneCamera);
      }
      else if (IsInSTAGE())
      {
      }
      else if (IsInGM())
      {
         waterActor->SetSceneCamera(dtABC::Application::GetInstance("Application")->GetCamera());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR_GETTER(WaterGridActorProxy, float, WaveDirection);
   DT_IMPLEMENT_ACCESSOR_GETTER(WaterGridActorProxy, float, AmplitudeModifier);
   DT_IMPLEMENT_ACCESSOR_GETTER(WaterGridActorProxy, float, WavelengthModifier);
   DT_IMPLEMENT_ACCESSOR_GETTER(WaterGridActorProxy, float, SpeedModifier);

   /////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::SetWaveDirection(float f)
   {
      WaterGridActor* wga = NULL;
      GetDrawable(wga);

      mWaveDirection = f;
      wga->SetModForDirectionInDegrees(mWaveDirection);
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::SetWavelengthModifier(float f)
   {
      WaterGridActor* wga = NULL;
      GetDrawable(wga);

      mWavelengthModifier = f;
      wga->SetModForWaveLength(mWavelengthModifier);
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::SetAmplitudeModifier(float f)
   {
      WaterGridActor* wga = NULL;
      GetDrawable(wga);

      mAmplitudeModifier = f;
      wga->SetModForAmplitude(mAmplitudeModifier);
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaterGridActorProxy::SetSpeedModifier(float f)
   {
      WaterGridActor* wga = NULL;
      GetDrawable(wga);

      mSpeedModifier = f;
      wga->SetModForSpeed(f);
   }
}
////////////////////////////////////////////////////////////////////////////////
