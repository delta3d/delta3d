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
#ifndef DELTA_HDRSCENE_H
#define DELTA_HDRSCENE_H

#include <dtRender/ppuscene.h>

#include <dtCore/baseactorobject.h>

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
   
   class DT_RENDER_EXPORT HDRScene : public PPUScene
   {
   public:
      typedef PPUScene BaseClass;
      static const dtCore::RefPtr<SceneType> HDR_SCENE;

   public:
      HDRScene();
      virtual ~HDRScene();
      
      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      DT_DECLARE_ACCESSOR_INLINE(float, MidGrey)
      DT_DECLARE_ACCESSOR_INLINE(float, HDRBlurSigma)
      DT_DECLARE_ACCESSOR_INLINE(float, HDRBlurRadius)
      DT_DECLARE_ACCESSOR_INLINE(float, GlareFactor)
      DT_DECLARE_ACCESSOR_INLINE(float, AdaptFactor)
      DT_DECLARE_ACCESSOR_INLINE(float, MinLuminance)
      DT_DECLARE_ACCESSOR_INLINE(float, MaxLuminance)
      
      void OnAddedToPPUScene( MultipassScene& mps );

   protected:
      void CreateHDRPipeline(osgPPU::UnitBypass* bypass, osgPPU::Unit* resample);

   private:
   };

   class DT_RENDER_EXPORT HDRSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      HDRSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~HDRSceneActor();
   };

}

#endif // DELTA_HDRSCENE_H
