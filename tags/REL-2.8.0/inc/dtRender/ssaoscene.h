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
#ifndef DELTA_SSAOSCENE_H
#define DELTA_SSAOSCENE_H

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
   
   class DT_RENDER_EXPORT SSAOScene : public PPUScene
   {
   public:
      typedef PPUScene BaseClass;
      static const dtCore::RefPtr<SceneType> SSAO_SCENE;

   public:
      SSAOScene();
      virtual ~SSAOScene();
      
      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      DT_DECLARE_ACCESSOR_INLINE(float, BlurSigma)
      DT_DECLARE_ACCESSOR_INLINE(float, BlurRadius)
      DT_DECLARE_ACCESSOR_INLINE(float, Intensity)
      DT_DECLARE_ACCESSOR_INLINE(bool, ShowOnlyAOMap)


      /*virtual*/ void OnAddedToPPUScene(MultipassScene& mps);

   protected:
      void CreateSSAOPipeline(osgPPU::UnitBypass* colorbypass, osgPPU::UnitBypass* depthbypass);

   private:
   };

   class DT_RENDER_EXPORT SSAOSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      SSAOSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~SSAOSceneActor();
   };

}

#endif // DELTA_SSAOSCENE_H
