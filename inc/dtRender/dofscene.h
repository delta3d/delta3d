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
#ifndef DELTA_DOFSCENE_H
#define DELTA_DOFSCENE_H

#include <dtRender/ppuscene.h>
#include <dtUtil/getsetmacros.h>
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
   
   class DT_RENDER_EXPORT DOFScene : public PPUScene
   {
   public:
      typedef PPUScene BaseClass;
      static const dtCore::RefPtr<SceneType> DOF_SCENE;

   public:
      DOFScene();
      virtual ~DOFScene();

      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      
      DT_DECLARE_ACCESSOR_INLINE(float, FocalLength)
      DT_DECLARE_ACCESSOR_INLINE(float, FocalRange)
      DT_DECLARE_ACCESSOR_INLINE(float, ZNear)
      DT_DECLARE_ACCESSOR_INLINE(float, ZFar)
      DT_DECLARE_ACCESSOR_INLINE(float, GaussSigma)
      DT_DECLARE_ACCESSOR_INLINE(float, GaussRadius)

      void OnAddedToPPUScene( MultipassScene& mps );

   protected:
      void CreateDOFPipeline(osgPPU::UnitBypass* bypass, osgPPU::UnitBypass* depthbypass, osgPPU::Unit* resampleLight);

   private:
      
   };

   class DT_RENDER_EXPORT DOFSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      DOFSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~DOFSceneActor();
   };

}

#endif // DELTA_DOFSCENE_H
