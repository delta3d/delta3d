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
#ifndef DELTA_VOLUMESCENE_H
#define DELTA_VOLUMESCENE_H

#include <dtVoxel/export.h>
#include <dtRender/scenebase.h>

#include <dtCore/baseactorobject.h>

namespace dtVoxel
{
   class DT_VOXEL_EXPORT VolumeScene : public dtRender::SceneBase
   {
   public:
      typedef dtRender::SceneBase BaseClass;
      static const dtCore::RefPtr<dtRender::SceneType> VOLUME_SCENE;

   public:
      VolumeScene();
      virtual ~VolumeScene();
      
      virtual void CreateScene(dtRender::SceneManager&, const dtRender::GraphicsQuality&);

      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;


   private:
      dtCore::RefPtr<osg::Group> mNode;
   };


   class DT_VOXEL_EXPORT VolumeSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      VolumeSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~VolumeSceneActor();
   };
}

#endif // DELTA_VOLUMESCENE_H
