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
#ifndef DELTA_SHADOWSCENE_H
#define DELTA_SHADOWSCENE_H

#include <dtRender/scenebase.h>
#include <dtCore/propertymacros.h>
#include <dtCore/baseactorobject.h>
#include <dtUtil/enumeration.h>
namespace osg
{
   class LightSource;
}

namespace osgShadow
{
   class ShadowTechnique;
   class ShadowedScene;
}

namespace dtRender
{
   class ShadowSceneImpl;

   class DT_RENDER_EXPORT ShadowScene : public SceneBase
   {
   public:
      typedef SceneBase BaseClass;
      static const dtCore::RefPtr<SceneType> SHADOW_SCENE;
      
      class DT_RENDER_EXPORT ShadowMapType : public dtUtil::Enumeration 
      {
         DECLARE_ENUM(ShadowScene::ShadowMapType);
      public:

         static ShadowMapType BASIC_SHADOW_MAP; 
         static ShadowMapType LSPSM;
         static ShadowMapType PSSM; 
         static ShadowMapType SOFT_SHADOW_MAP;

      protected:         
         ShadowMapType(const std::string &name);
      };


      class DT_RENDER_EXPORT ShadowResolution : public dtUtil::Enumeration 
      {
         DECLARE_ENUM(ShadowScene::ShadowResolution);
      public:
         
         static ShadowResolution SR_ULTRAHIGH;
         static ShadowResolution SR_HIGH;
         static ShadowResolution SR_MEDIUM;
         static ShadowResolution SR_LOW;

      protected:
         ShadowResolution(const std::string &name);
      };
      
      static const dtUtil::RefString UNIFORM_RENDER_SHADOWS;
      static const dtUtil::RefString UNIFORM_SHADOW_EFFECT_SCALAR;
      static const dtUtil::RefString UNIFORM_SHADOW_TEXTURE_UNIT;


   public:
      ShadowScene();
      virtual ~ShadowScene();
      
      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;

      osg::LightSource* GetLightSource();
      void SetLightSource(osg::LightSource* ls);

      void SetShadowsEnabled(bool enabled);
      bool GetShadowsEnabled() const;

      virtual bool AddChild(DeltaDrawable* child);
      virtual void RemoveChild(DeltaDrawable* child);


      DT_DECLARE_ACCESSOR_INLINE(dtUtil::EnumerationPointer<ShadowMapType>, ShadowMapType);
      DT_DECLARE_ACCESSOR_INLINE(dtUtil::EnumerationPointer<ShadowResolution>, ShadowResolution);

      DT_DECLARE_ACCESSOR_INLINE(osg::Vec2, AmbientBias)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec2, PolygonOffset)


      DT_DECLARE_ACCESSOR_INLINE(int, TextureUnitOffset)
      DT_DECLARE_ACCESSOR_INLINE(int, NumPSSMSplits)
      
      DT_DECLARE_ACCESSOR_INLINE(float, MinNearDistance)
      DT_DECLARE_ACCESSOR_INLINE(float, MaxFarDistance)
      
      DT_DECLARE_ACCESSOR_INLINE(bool, ComputeBestLightFromEphemeris)
      DT_DECLARE_ACCESSOR_INLINE(bool, UseShadowEffectScalar)
      
      void SetShadowEffectsScalar(float f);
      float GetShadowEffectsScalar() const;

      DT_DECLARE_ACCESSOR_INLINE(bool, RenderEveryFrame)
      DT_DECLARE_ACCESSOR_INLINE(bool, RenderOnLightChanged)
      DT_DECLARE_ACCESSOR_INLINE(int, TraversalMod)

      void SetTraversal(int num);
      
   private:
      DT_DECLARE_ACCESSOR_INLINE(int, TraversalNumber)
      DT_DECLARE_ACCESSOR_INLINE(bool, BypassTraversal)
      DT_DECLARE_ACCESSOR_INLINE(bool, LightChanged)

      void SetShadowMap();
      osgShadow::ShadowTechnique* GetBasicShadowMap(int shadowRes);
      osgShadow::ShadowTechnique* GetLightSpacePerspectiveShadowMap(int shadowRes);
      osgShadow::ShadowTechnique* GetSoftShadowMap(int shadowRes);
      osgShadow::ShadowTechnique* GetParallelSplitShadowMap(int shadowRes);


     ShadowSceneImpl* mImpl; 
   };

   class DT_RENDER_EXPORT ShadowSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      ShadowSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~ShadowSceneActor();
   };

}

#endif // DELTA_SHADOWSCENE_H
