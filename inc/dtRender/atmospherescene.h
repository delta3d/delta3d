/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2015, Caper Holdings, LLC
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
#ifndef DELTA_ATMOSPHERESCENE_H
#define DELTA_ATMOSPHERESCENE_H

#include <dtRender/ppuscene.h>
#include <dtUtil/getsetmacros.h>
#include <dtCore/baseactorobject.h>

#include <osg/Vec4>

namespace osg
{
   class Camera;
}

namespace osgPPU
{
   class Unit;
   class UnitBypass;
}
 
namespace dtRender
{
   
   class DT_RENDER_EXPORT AtmosphereScene : public PPUScene
   {
   public:
      typedef PPUScene BaseClass;
      static const dtCore::RefPtr<SceneType> ATMOSPHERE_SCENE;

   public:
      AtmosphereScene();
      virtual ~AtmosphereScene();

      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      bool GetFogEnable() const;
      void SetFogEnable(bool enable);

      bool GetAutoComputeNearFar() const;
      void SetAutoComputeNearFar(bool enable);

      float GetFogDensity() const;
      void SetFogDensity(float density);

      float GetVisibility() const;
      void SetVisibility(float distance);

      const osg::Vec4& GetFogColor() const;
      void SetFogColor(const osg::Vec4& color);      

      void OnAddedToPPUScene( MultipassScene& mps );

   protected:
      void CreateDOFPipeline(osgPPU::UnitBypass* bypass, osgPPU::UnitBypass* depthbypass, osgPPU::Unit* resampleLight);

   private:
      
      bool mFogEnabled;
      bool mAutoComputeNearFar;
      float mFogDensity;      
      float mNearPlane;
      float mFarPlane;
      float mVisibility;
      osg::Vec4 mFogColor;
   };

   class DT_RENDER_EXPORT AtmosphereSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      AtmosphereSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~AtmosphereSceneActor();
   };

}

#endif // DELTA_ATMOSPHERESCENE_H
