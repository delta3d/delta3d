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
#ifndef DELTA_VIDEOSCENE_H
#define DELTA_VIDEOSCENE_H

#include <dtRender/ppuscene.h>
#include <dtCore/baseactorobject.h>

namespace osg
{
   class Camera;
   class Texture2D;
}

namespace osgPPU
{
   class Unit;
}

namespace dtRender
{
   class VideoSceneImpl;
   class DT_RENDER_EXPORT VideoScene : public PPUScene
   {
   public:
      typedef PPUScene BaseClass;
      static const dtCore::RefPtr<SceneType> VIDEO_SCENE;

   public:
      VideoScene();
      virtual ~VideoScene();
      
      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;

      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      osg::Texture2D* GetOutputTexture();
      const osg::Texture2D* GetOutputTexture() const;

      DT_DECLARE_ACCESSOR_INLINE(std::string, VideoFile)
      DT_DECLARE_ACCESSOR_INLINE(bool, Looping)

   private:
      VideoSceneImpl* mImpl;
   };

   class DT_RENDER_EXPORT VideoSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      VideoSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~VideoSceneActor();
   };

}

#endif // DELTA_VIDEOSCENE_H

