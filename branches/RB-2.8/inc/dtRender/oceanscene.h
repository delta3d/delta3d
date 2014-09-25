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
#ifndef DELTA_OCEANSCENE_H
#define DELTA_OCEANSCENE_H

#include <dtRender/scenebase.h>
#include <dtCore/baseactorobject.h>

#include <dtActors/watergridactor.h>
#include <dtUtil/getsetmacros.h>

namespace dtRender
{
   class MultipassScene;
   class OceanSceneImpl;
   class DT_RENDER_EXPORT OceanScene : public SceneBase
   {
   public:
      typedef SceneBase BaseClass;
      static const dtCore::RefPtr<SceneType> OCEAN_SCENE;
      
      static const dtUtil::RefString UNIFORM_WATER_HEIGHT;
      static const dtUtil::RefString UNIFORM_WATER_COLOR;
      static const dtUtil::RefString UNIFORM_UNDERWATER_VIEW_DISTANCE;


   public:
      OceanScene();
      virtual ~OceanScene();
      
      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;

      virtual void OnResize(MultipassScene& mps, int width, int height);


      DT_DECLARE_ACCESSOR(int, NumRows)
      DT_DECLARE_ACCESSOR(int, NumColumns)

      DT_DECLARE_ACCESSOR(float, WaveDirection);
      DT_DECLARE_ACCESSOR(float, AmplitudeModifier);
      DT_DECLARE_ACCESSOR(float, WavelengthModifier);
      DT_DECLARE_ACCESSOR(float, SpeedModifier);      
      DT_DECLARE_ACCESSOR(float, UnderWaterViewDistance);      
      DT_DECLARE_ACCESSOR(osg::Vec4, WaterColor);
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec2, ReflectionMapResolution)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec2, TexWaveTextureResolution)
      DT_DECLARE_ACCESSOR(float, TexWaveResolutionScalar)
      DT_DECLARE_ACCESSOR(float, TexWaveAmpScalar)
      DT_DECLARE_ACCESSOR(float, TexWaveSpreadScalar)
      DT_DECLARE_ACCESSOR(float, TexWaveSteepness)

      DT_DECLARE_ACCESSOR(dtUtil::EnumerationPointer<dtActors::WaterGridActor::SeaState>, SeaState);
      DT_DECLARE_ACCESSOR(dtUtil::EnumerationPointer<dtActors::WaterGridActor::ChopSettings>, Chop);

      DT_DECLARE_ACCESSOR(bool, UseDebugKeys)

   private:
      OceanSceneImpl* mImpl;
   };



   class DT_RENDER_EXPORT OceanSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      OceanSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;


   protected:
      virtual ~OceanSceneActor();
   };
}

#endif // DELTA_OceanScene_H
