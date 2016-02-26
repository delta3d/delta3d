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
#ifndef DELTA_LIGHTSCENE_H
#define DELTA_LIGHTSCENE_H

#include <dtRender/scenebase.h>
#include <dtRender/dynamiclight.h>
#include <dtCore/baseactorobject.h>

namespace dtCore
{
   class Camera;
}

namespace dtRender
{
   
   class DT_RENDER_EXPORT LightScene : public SceneBase
   {
   public:
      typedef SceneBase BaseClass;
      static const dtCore::RefPtr<SceneType> LIGHT_SCENE;

      static const dtUtil::RefString UNIFORM_DYNAMIC_LIGHT_ARRAY;
      static const dtUtil::RefString UNIFORM_NUM_DYNAMIC_LIGHTS_TO_USE;
      
      static const dtUtil::RefString UNIFORM_SPOT_LIGHT_ARRAY;
      static const dtUtil::RefString UNIFORM_NUM_SPOT_LIGHTS_TO_USE;


      typedef std::vector<dtCore::ObserverPtr<DynamicLight> > LightArray;      

      static const std::string DEFAULT_LIGHT_NAME;

   public:
      LightScene();
      virtual ~LightScene();
      
      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;

      virtual void CreateScene(SceneManager&, const GraphicsQuality&);
      
      //Use this method for adding dynamic lights or spot lights to the rendering support component
      void AddDynamicLight(DynamicLight*);
      void RemoveDynamicLight(DynamicLight::LightID id);

      /**
      * Simple check to make sure a light is valid
      */
      bool HasLight(DynamicLight::LightID id) const;

      //if your light is a spot light use this function and cast it
      //if you aren't sure what type of light it is you can check the light type enumeration
      DynamicLight* GetDynamicLight(DynamicLight::LightID id);

      void SetMaxDynamicLights(unsigned lights);
      unsigned GetMaxDynamicLights() const;

      void SetMaxSpotLights(unsigned lights);
      unsigned GetMaxSpotLights() const;

      void TimeoutAndDeleteLights(float dt);
      void TransformAndSortLights();

      void UpdateDynamicLightUniforms(osg::Uniform* lightArray, osg::Uniform* numDynLights, osg::Uniform* spotLightArray, osg::Uniform* numSpotLights);
      void UpdateDynamicLightUniforms(const LightArray& lights, osg::Uniform* lightArray, osg::Uniform* numDynLights, osg::Uniform* spotLightArray, osg::Uniform* numSpotLights);

      void FindBestLights(dtCore::Transformable& actor);
      unsigned GetNumLights() const;

      //called from update callback
      void UpdateDynamicLights();

   private:

      void RemoveLight(LightArray::iterator);
      DynamicLight* FindLight(DynamicLight::LightID id);

      void SetPosition(DynamicLight* dl);
      void SetDirection(SpotLight* light);


      unsigned mMaxDynamicLights;
      unsigned mMaxSpotLights;

      LightArray mLights;

      dtCore::RefPtr<dtCore::Transformable> mTargetCamera;

      dtCore::RefPtr<osg::Uniform> mDynamicLightUniform;
      dtCore::RefPtr<osg::Uniform> mNumDynamicLightsUniform;

      dtCore::RefPtr<osg::Uniform> mSpotLightUniform;
      dtCore::RefPtr<osg::Uniform> mNumSpotLightsUniform;

      dtCore::RefPtr<osg::Group> mRootNode;
   };

   class DT_RENDER_EXPORT LightSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      LightSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~LightSceneActor();
   };

}

#endif // DELTA_LIGHTSCENE_H
