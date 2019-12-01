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
#ifndef DELTA_CUBEMAPSCENE_H
#define DELTA_CUBEMAPSCENE_H

#include <dtRender/ppuscene.h>

#include <dtCore/baseactorobject.h>

#include <osg/Vec4>

namespace osg
{
   class Camera;
   class TextureCubeMap;
}

namespace osgPPU
{
   class Unit;
}

namespace dtRender
{
   class CubemapSceneImpl;
   class DT_RENDER_EXPORT CubeMapScene : public PPUScene
   {
   public:
      typedef PPUScene BaseClass;
      static const dtCore::RefPtr<SceneType> CUBEMAP_SCENE;

   public:
      CubeMapScene();
      virtual ~CubeMapScene();
      
      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;

      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      osg::TextureCubeMap* GetCubeMap();
      const osg::TextureCubeMap* GetCubeMap() const;

      void SetTarget(dtCore::Transformable&);
      const dtCore::Transformable& SetTarget() const;

      DT_DECLARE_ACCESSOR_INLINE(int, TexWidth)
      DT_DECLARE_ACCESSOR_INLINE(int, TexHeight)
      
      DT_DECLARE_ACCESSOR_INLINE(int, NearPlane)
      DT_DECLARE_ACCESSOR_INLINE(int, FarPlane)
      DT_DECLARE_ACCESSOR_INLINE(float, LODScale)

      DT_DECLARE_ACCESSOR_INLINE(bool, ShowCubeMap)

      DT_DECLARE_ACCESSOR_INLINE(osg::Vec4, ClearColor)

      DT_DECLARE_ACCESSOR_INLINE(bool, RenderEveryFrame)
      DT_DECLARE_ACCESSOR_INLINE(bool, RenderOnLightChanged)
      DT_DECLARE_ACCESSOR_INLINE(int, TraversalMod)

      void SetTraversal(int num);

   private:
      DT_DECLARE_ACCESSOR_INLINE(int, TraversalNumber)
      DT_DECLARE_ACCESSOR_INLINE(bool, BypassTraversal)
      DT_DECLARE_ACCESSOR_INLINE(bool, LightChanged)


      CubemapSceneImpl* mImpl;
   };

   class DT_RENDER_EXPORT CubeMapSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      CubeMapSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~CubeMapSceneActor();
   };

}

#endif // DELTA_CUBEMAPSCENE_H
