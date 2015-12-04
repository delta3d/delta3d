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

#include <dtRender/lightscene.h>
#include <dtRender/uniformactcomp.h>

#include <dtUtil/nodemask.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/noiseutility.h>
#include <dtCore/observerptr.h>
#include <dtCore/transform.h>
#include <dtCore/system.h>

//needed to set the scene camera
#include <dtRender/scenemanager.h>
#include <dtGame/gamemanager.h>
#include <dtABC/application.h>
#include <dtCore/camera.h>

#include <osg/Version>

namespace dtRender
{

#if defined (__APPLE__) && OSG_VERSION_LESS_THAN(3,2,0)
   const dtUtil::RefString LightScene::UNIFORM_DYNAMIC_LIGHT_ARRAY = "d3d_DynamicLights[0]";
   const dtUtil::RefString LightScene::UNIFORM_SPOT_LIGHT_ARRAY = "d3d_SpotLights[0]";
#else
   const dtUtil::RefString LightScene::UNIFORM_DYNAMIC_LIGHT_ARRAY = "d3d_DynamicLights";
   const dtUtil::RefString LightScene::UNIFORM_SPOT_LIGHT_ARRAY = "d3d_SpotLights";
#endif

   const dtUtil::RefString LightScene::UNIFORM_NUM_DYNAMIC_LIGHTS_TO_USE = "NUM_DYNAMIC_LIGHTS_TO_USE";
   const dtUtil::RefString LightScene::UNIFORM_NUM_SPOT_LIGHTS_TO_USE = "NUM_SPOT_LIGHTS_TO_USE";


   const dtCore::RefPtr<SceneType> LightScene::LIGHT_SCENE(new SceneType("Light Scene", "Scene", "Creates a scene to hold and render dynamic lights."));

   //useful functors
   struct findLightById
   {
      findLightById(DynamicLight::LightID id): mId(id){}

      template<class T>
      bool operator()(T lightPtr)
      {
         return lightPtr->GetLightId() == mId;
      }
   private:

      DynamicLight::LightID mId;
   };

   struct removeLightsFunc
   {
      template<class T>
      bool operator()(T lightPtr)
      {
         return lightPtr->GetDeleteMe();
      }
   };


   struct funcCompareLights
   {
      funcCompareLights(const osg::Vec3& viewPos): mViewPos(viewPos){}

      //todo- cache these operations for efficiency
      template<class T>
      bool operator()(T& pElement1, T& pElement2)
      {
         osg::Vec3 vectElement1 = pElement1->GetLightPosition() - mViewPos;
         osg::Vec3 vectElement2 = pElement2->GetLightPosition() - mViewPos;

         float dist1 = dtUtil::Max(0.0f, vectElement1.length() - pElement1->GetRadius());
         float dist2 = dtUtil::Max(0.0f, vectElement2.length() - pElement2->GetRadius());

         return  dist1 < dist2;
      }

   private:
      osg::Vec3 mViewPos;

   };


   //a callback to update the lights
   class UpdateLightsCallback : public osg::NodeCallback
   {
   public:

      UpdateLightsCallback(LightScene& ls)
         : mLightScene(&ls)
      {
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         // first update subgraph to make sure objects are all moved into postion
         traverse(node,nv);

         mLightScene->UpdateDynamicLights();
         
      }

   protected:

      virtual ~UpdateLightsCallback() {}

      dtCore::ObserverPtr<LightScene>                 mLightScene;

   };



   ////////////////////////////////////////////////////////////////////////////////////////////////////////////   
   const std::string LightScene::DEFAULT_LIGHT_NAME = "DefaultLight";


   LightScene::LightScene()
   : BaseClass(*LIGHT_SCENE, SceneEnum::PRE_RENDER)
   , mMaxDynamicLights(25)
   , mMaxSpotLights(10)
   , mRootNode(new osg::Group())
   {
      SetName("LightScene");  
   }


   LightScene::~LightScene()
   {
   }


   void LightScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
      
      if(sm.GetSceneCamera() != NULL)
      {
         dtCore::RefPtr<UpdateLightsCallback> udlc = new UpdateLightsCallback(*this);

         mTargetCamera = sm.GetSceneCamera();

         //now setup the lighting uniforms necessary for rendering the dynamic lights
         osg::StateSet* ss = NULL;

         UniformActComp* uniformActComp = sm.GetOwner()->GetComponent<UniformActComp>();
         if(uniformActComp != NULL)
         {
            //currently no array uniform property
            ss = uniformActComp->GetStateSet();
         }

         if(ss == NULL)
         {
            ss = sm.GetOSGNode()->getOrCreateStateSet();
         }


         mDynamicLightUniform = ss->getOrCreateUniform(UNIFORM_DYNAMIC_LIGHT_ARRAY, osg::Uniform::FLOAT_VEC4, mMaxDynamicLights * 3);
         mDynamicLightUniform->setDataVariance(osg::Object::DYNAMIC);

         mSpotLightUniform = ss->getOrCreateUniform(UNIFORM_SPOT_LIGHT_ARRAY, osg::Uniform::FLOAT_VEC4, mMaxSpotLights * 4);
         mSpotLightUniform->setDataVariance(osg::Object::DYNAMIC);

         mNumDynamicLightsUniform = ss->getOrCreateUniform(UNIFORM_NUM_DYNAMIC_LIGHTS_TO_USE, osg::Uniform::INT);
         mNumDynamicLightsUniform->setDataVariance(osg::Object::DYNAMIC);

         mNumSpotLightsUniform = ss->getOrCreateUniform(UNIFORM_NUM_SPOT_LIGHTS_TO_USE, osg::Uniform::INT);
         mNumSpotLightsUniform->setDataVariance(osg::Object::DYNAMIC);


         mRootNode->setUpdateCallback(udlc);
      }
      else
      {
         LOG_ERROR("Unable to create light scene without a main camera.");
      }
   }

   osg::Group* LightScene::GetSceneNode()
   {
      return mRootNode;
   }

   const osg::Group* LightScene::GetSceneNode() const
   {
      return mRootNode;
   }


   ///////////////////////////////////////////////////////////////////////////////////////////////////
   void LightScene::AddDynamicLight(DynamicLight* dl)
   {
      if (dl != NULL)
      {
         mLights.push_back(dl);
      }
      else
      {
         LOG_ERROR("Attempting to add a light that is NULL to the Rendering Support Component");         
      }
   }

   void LightScene::RemoveDynamicLight(DynamicLight::LightID id)
   {
      mLights.erase(std::remove_if(mLights.begin(), mLights.end(), findLightById(id)), mLights.end());
   }

   void LightScene::RemoveLight(LightArray::iterator iter)
   {
      mLights.erase(iter);
   }

   bool LightScene::HasLight(DynamicLight::LightID id) const
   {
      LightArray::const_iterator iter = std::find_if(mLights.begin(), mLights.end(), findLightById(id));
      return iter != mLights.end();
   }

   DynamicLight* LightScene::GetDynamicLight(DynamicLight::LightID id)
   {
      return FindLight(id);
   }

   DynamicLight* LightScene::FindLight(DynamicLight::LightID id)
   {
      LightArray::iterator iter = std::find_if(mLights.begin(), mLights.end(), findLightById(id));
      if(iter != mLights.end())
      {
         return (*iter).get();
      }

      return NULL;
   }


   ///////////////////////////////////////////////////////////////////////////////////////////////////
   void LightScene::TimeoutAndDeleteLights(float dt)
   {
      LightArray::iterator iter = mLights.begin();
      LightArray::iterator endIter = mLights.end();

      for(;iter != endIter; ++iter)
      {
         DynamicLight* dl = (*iter).get();

         //this if check looks a little iffy but the control flow is used to allow the auto delete, max time, and fade out to all work together
         //while the first component of the if is straight forward, the second component "(!dl->mDeleteAfterMaxTime && dl->mAutoDeleteLightOnTargetNull)"
         //ensures that we do not fade out if are target is still valid but we DONT have a max time
         if((dl->GetDeleteOnTargetIsNull() && dl->GetTarget() == NULL ) || (!dl->GetDeleteAfterMaxTime() && dl->GetDeleteOnTargetIsNull()))
         {
            if(dl->GetTarget() == NULL )
            {
               if(dl->GetFadeOut())
               {
                  //by setting this to false we will continue into a fade out
                  dl->SetDeleteOnTargetIsNull(false);
                  //by setting this one false we ensure we will begin fading out next frame
                  dl->SetDeleteAfterMaxTime(false);
               }
               else
               {
                  dl->SetDeleteMe(true);
                  //std::cout << "Auto delete on NULL Ptr" << std::endl;
                  continue;
               }
            }
         }
         else if(dl->GetDeleteAfterMaxTime())
         {
            dl->SetMaxTime(dl->GetMaxTime() - dt);

            if(dl->GetMaxTime() <= 0.0f)
            {
               if(dl->GetFadeOut())
               {
                  //by setting this to false we will continue into a fade out
                  dl->SetDeleteAfterMaxTime(false);
                  //by setting this one false we ensure we will begin fading out next frame
                  dl->SetDeleteOnTargetIsNull(false);
               }
               else
               {
                  dl->SetDeleteMe(true);
                  //std::cout << "Auto delete on Max Time" << std::endl;
                  continue;
               }
            }
         }
         else if(dl->GetFadeOut())
         {
            dl->SetIntensityMod(dl->GetIntensityMod() - (dt / dl->GetFadeOutTime()));
            if(dl->GetIntensityMod() <= 0.0f)
            {
               dl->SetDeleteMe(true);
               //std::cout << "Auto delete on fade out" << std::endl;
               continue;
            }
         }

         //apply different update effects
         if(dl->GetFlicker())
         {
            //lets flicker the lights a little
            static dtUtil::Noise1f perlinNoise;
            float noiseValue = dl->GetFlickerScale() * perlinNoise.GetNoise(dt + dtUtil::RandFloat(0.0f, 10.0f));

            //keep the intensity within range of the noise flicker
            //TODO- don't assume an intensity of 1.0
            if(dtUtil::Abs(1.0f - (dl->GetIntensityMod() + noiseValue)) > dl->GetFlickerScale()) noiseValue *= -1.0f;
            dl->SetIntensityMod(dl->GetIntensityMod() + noiseValue);

            //std::cout << "Intensity " << dl->mIntensity << std::endl;
         }

      }

      //now remove all flagged lights, note this is actually faster because we only have a single deallocation for N lights
      LightArray::iterator lightIter = std::remove_if(mLights.begin(), mLights.end(), removeLightsFunc());
      if (lightIter != mLights.end())
      {
         //std::cout << "Deleting lights" << std::endl;
         mLights.erase(lightIter, mLights.end());
      }

   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////
   void LightScene::TransformAndSortLights()
   {
      LightArray::iterator iter = mLights.begin();
      LightArray::iterator endIter = mLights.end();

      for(;iter != endIter; ++iter)
      {
         DynamicLight* dl = (*iter).get();
         //update the light's position
         SetPosition(dl);
      
         if (dl->GetLightType() == DynamicLight::LightType::SPOT_LIGHT)
         {
            SpotLight* sLight = dynamic_cast<SpotLight*>(dl);
            if(sLight != NULL)
            {
               SetDirection(sLight);
            }
         }
      }

      //update uniforms by finding the closest lights to the camera
      dtCore::Transform trans;
      mTargetCamera->GetTransform(trans);
      osg::Vec3 pos;
      trans.GetTranslation(pos);
      //sort the lights, though a heap may be more efficient here, we will sort so that we can combine lights later
      std::sort(mLights.begin(), mLights.end(), funcCompareLights(pos));
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void LightScene::FindBestLights(dtCore::Transformable& actor)
   {
      LightArray tempLightArray(mLights);

      dtCore::Transform trans;
      actor.GetTransform(trans);
      osg::Vec3 pos;
      trans.GetTranslation(pos);
      //sort the lights, though a heap may be more efficient here, we will sort so that we can combine lights later
      std::sort(tempLightArray.begin(), tempLightArray.end(), funcCompareLights(pos));

      //now setup the lighting uniforms necessary for rendering the dynamic lights
      osg::StateSet* ss = actor.GetOSGNode()->getOrCreateStateSet();
      //temporary hack
      osg::Uniform* lightArrayUniform = ss->getOrCreateUniform(UNIFORM_DYNAMIC_LIGHT_ARRAY, osg::Uniform::FLOAT_VEC4, mMaxDynamicLights * 3);
      lightArrayUniform->setDataVariance(osg::Object::DYNAMIC);

      osg::Uniform* numlightArrayUniform = ss->getOrCreateUniform(UNIFORM_NUM_DYNAMIC_LIGHTS_TO_USE, osg::Uniform::INT);
      numlightArrayUniform->setDataVariance(osg::Object::DYNAMIC);

      osg::Uniform* spotLightArrayUniform = ss->getOrCreateUniform(UNIFORM_SPOT_LIGHT_ARRAY, osg::Uniform::FLOAT_VEC4, mMaxSpotLights * 4);
      spotLightArrayUniform->setDataVariance(osg::Object::DYNAMIC);

      osg::Uniform* numspotLightArrayUniform = ss->getOrCreateUniform(UNIFORM_NUM_SPOT_LIGHTS_TO_USE, osg::Uniform::INT);
      numspotLightArrayUniform->setDataVariance(osg::Object::DYNAMIC);


      UpdateDynamicLightUniforms(tempLightArray, lightArrayUniform, numlightArrayUniform, spotLightArrayUniform, numspotLightArrayUniform);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////
   void LightScene::UpdateDynamicLightUniforms(osg::Uniform* lightArray, osg::Uniform* numDynLights, osg::Uniform* spotLightArray, osg::Uniform* numSpotLights)
   {
      UpdateDynamicLightUniforms(mLights, lightArray, numDynLights, spotLightArray, numSpotLights);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////
   void LightScene::UpdateDynamicLightUniforms(const LightArray& lights, osg::Uniform* lightArray, osg::Uniform* numDynLightsUniform, osg::Uniform* spotLightArray, osg::Uniform* numSpotLightsUniform)
   {
      int numDynamicLights = 0;
      int numSpotLights = 0;

      int numDynamicLightAttributes = 3;
      int numSpotLightAttributes = 4;

      int maxDynamicLightUniforms = numDynamicLightAttributes * mMaxDynamicLights;
      int maxSpotLightUniforms = numSpotLightAttributes * mMaxSpotLights;

      int numLights = 0;
      int totalLightSlots = mMaxSpotLights + mMaxDynamicLights;

      LightArray::const_iterator iter = lights.begin();
      LightArray::const_iterator endIter = lights.end();

      // Go over our lights and add them if they have actual intensity.
      for(; iter != endIter && numLights < totalLightSlots; ++iter)
      {
         DynamicLight* dl = (*iter).get();
         SpotLight* sl = NULL;

         bool useSpotLight = false;
         float spotExp = 0.0f;
         osg::Vec4 spotParams;

         //if we have an open slot for spot lights and we have a spot light
         //else we are out of dynamic light spots and we have a dynamic light make a spot light and bind it here
         if( (numSpotLights < maxSpotLightUniforms) && (dl->GetLightType() == DynamicLight::LightType::SPOT_LIGHT))
         {
            sl = static_cast<SpotLight*>(dl); // if it's not a SpotLight, crash anyway to let the dev know they messed up
            spotExp = sl->GetSpotExponent();
            osg::Vec3 curDir = sl->GetCurrentDirection();
            spotParams.set(curDir[0], curDir[1], curDir[2], sl->GetSpotCosCutoff());
            useSpotLight = true;
         }
         else if( !(numDynamicLights < maxDynamicLightUniforms) )
         {
            spotExp = 0.0f;
            spotParams.set(0.0f, 1.0f, 0.0f, -1.0f);
            useSpotLight = true;
         }

         //don't bind lights of zero intensity
         float intensity = dl->GetIntensity() * dl->GetIntensityMod();

         if(intensity > 0.0001f)
         {

            if(useSpotLight)
            {
               spotLightArray->setElement(numSpotLights, osg::Vec4(dl->GetLightPosition(), intensity));
               spotLightArray->setElement(numSpotLights + 1, osg::Vec4(dl->GetLightColor(), 1.0f));
               spotLightArray->setElement(numSpotLights + 2, osg::Vec4(dl->GetAttenuation(), spotExp));
               spotLightArray->setElement(numSpotLights + 3, spotParams);
               numSpotLights += numSpotLightAttributes;
            }
            else
            {
               lightArray->setElement(numDynamicLights, osg::Vec4(dl->GetLightPosition(), intensity));
               lightArray->setElement(numDynamicLights + 1, osg::Vec4(dl->GetLightColor(), 1.0f));
               lightArray->setElement(numDynamicLights + 2, osg::Vec4(dl->GetAttenuation(), 1.0f));
               numDynamicLights += numDynamicLightAttributes;
            }

            ++numLights;
         }
      }

      numDynLightsUniform->set(numDynamicLights / numDynamicLightAttributes);
      numSpotLightsUniform->set(numSpotLights / numSpotLightAttributes);

      //std::cout << "Num Lights " << numLights << std::endl;

   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////
   void LightScene::UpdateDynamicLights()
   {
      float dt = dtCore::System::GetInstance().GetSimulationTime();

      TimeoutAndDeleteLights(dt);
      TransformAndSortLights();

      UpdateDynamicLightUniforms(mLights, mDynamicLightUniform, mNumDynamicLightsUniform, mSpotLightUniform, mNumSpotLightsUniform);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////
   void LightScene::SetPosition(DynamicLight* dl)
   {
      if(dl != NULL)
      {
         osg::Vec3 newPos;
         dtCore::Transform xform;

         //if the light has a target set use the target position
         if(dl->GetTarget() != NULL)
         {
            dl->GetTarget()->GetTransform(xform);
            xform.GetTranslation(newPos);
            dl->SetLightPosition(newPos);
         }
         else //use the actor position
         {
            newPos = dl->GetTranslation();
            dl->SetLightPosition(newPos);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void LightScene::SetDirection(SpotLight* light)
   {
      if(light->GetTarget() != NULL && !light->GetUseAbsoluteDirection())
      {
         //this transforms our direction relative to our target
         //to create a world space direction
         dtCore::Transform xform;
         light->GetTarget()->GetTransform(xform);
         osg::Matrix rot;
         xform.GetRotation(rot);

         osg::Vec3 lightDir = light->GetDirection();       
         lightDir.normalize();
         light->SetDirection(lightDir);
         light->SetCurrentDirection(rot.preMult(light->GetDirection()));
         lightDir = light->GetCurrentDirection();
         lightDir.normalize();
         light->SetCurrentDirection(lightDir);

      }
      else
      {
         //if our direction is absolute or we do not have a target
         //then our world space direction is our current local direction
         light->SetCurrentDirection(light->GetDirection());
      }
   }


   ///////////////////////////////////////////////////////////////////////////////////
   void LightScene::SetMaxDynamicLights( unsigned lights )
   {
      mMaxDynamicLights = lights;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   unsigned LightScene::GetMaxDynamicLights() const
   {
      return mMaxDynamicLights;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void LightScene::SetMaxSpotLights( unsigned lights )
   {
      mMaxSpotLights = lights;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   unsigned LightScene::GetMaxSpotLights() const
   {
      return mMaxSpotLights;
   }


   ///proxy
   LightSceneActor::LightSceneActor()
   {
   }

   LightSceneActor::~LightSceneActor()
   {
   }

   void LightSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

   }

   void LightSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<LightScene> es = new LightScene();

      SetDrawable(*es);
   }

   bool LightSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender

