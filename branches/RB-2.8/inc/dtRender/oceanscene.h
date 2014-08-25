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

namespace dtRender
{
   class OceanSceneImpl;
   class DT_RENDER_EXPORT OceanScene : public SceneBase
   {
   public:
      typedef SceneBase BaseClass;
      static const dtCore::RefPtr<SceneType> OCEAN_SCENE;

   public:
      OceanScene();
      virtual ~OceanScene();
      
      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;


   private:
      OceanSceneImpl* mImpl;
   };



   class DT_RENDER_EXPORT OceanSceneProxy : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      OceanSceneProxy();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~OceanSceneProxy();
   };
}

#endif // DELTA_OceanScene_H
