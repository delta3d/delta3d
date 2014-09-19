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
#ifndef DELTA_SCENEBASE_H
#define DELTA_SCENEBASE_H

#include <dtRender/dtrenderexport.h>
#include <dtRender/scenetype.h>
#include <dtRender/sceneenum.h>
#include <dtRender/renderorder.h>
#include <dtRender/graphicsquality.h>

#include <dtCore/deltadrawable.h>
#include <dtCore/refptr.h>

namespace osg
{
   class Group;
   class Node;
}

namespace dtRender
{
   class SceneGroup;
   class SceneManager;

   class DT_RENDER_EXPORT SceneBase : public dtCore::DeltaDrawable
   {
   public:
      typedef dtCore::DeltaDrawable BaseClass;

   public:
      SceneBase(const SceneType& sceneId, const SceneEnum& defaultScene);
      virtual ~SceneBase();

      const SceneType& GetType() const;

      void SetChildOrder(RenderOrder);
      RenderOrder GetChildOrder() const;
      
      const SceneEnum& GetSceneEnum() const;
      void SetSceneEnum(const SceneEnum&);

      virtual void CreateScene(SceneManager&, const GraphicsQuality&) = 0;
         
      virtual bool AddChild(DeltaDrawable* child);
      virtual void RemoveChild(DeltaDrawable* child);
      
      ///use to clean up the scene hierarchy
      virtual void RemoveAllChildren();

      virtual bool ContainsActor(dtCore::DeltaDrawable& dd) const;
      virtual void GetAllActors(std::vector<dtCore::DeltaDrawable*>& vec);
      
      virtual osg::Group* GetSceneNode() = 0;
      virtual const osg::Group* GetSceneNode() const = 0;

      virtual osg::Node* GetOSGNode();
      virtual const osg::Node* GetOSGNode() const;

      virtual SceneGroup* GetAsSceneGroup(){return NULL;}
      virtual const SceneGroup* GetAsSceneGroup() const{return NULL;}

   protected:

      
   private:
      SceneBase(); //not implemented
      SceneBase(const SceneBase&);//not implemented
      SceneBase& operator=(const SceneBase&);//not implemented

      RenderOrder mChildOrder;
      const SceneEnum* mSceneEnum;
      dtCore::RefPtr<const SceneType> mType;

   };

}

#endif // DELTA_SCENEBASE_H
