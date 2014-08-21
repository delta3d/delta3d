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

#include <dtRender/scenebase.h>

#include <dtRender/scenegroup.h>

#include <osg/Group>

namespace dtRender
{

   SceneBase::SceneBase(const SceneType& st, const SceneEnum& defaultScene)
   : BaseClass()
   , mRenderOrder(0)
   , mSceneEnum(&defaultScene)
   , mType(&st)   
   {
      SetName("SceneBase");
   }


   SceneBase::~SceneBase()
   {
      mType = NULL;
   }

   const SceneType& SceneBase::GetType() const
   {
      return *mType;
   }


   bool SceneBase::AddChild( DeltaDrawable* child )
   {
      if (DeltaDrawable::AddChild(child))
      {
         GetSceneNode()->addChild(child->GetOSGNode());
         return true;
      }
      else
      {
         return false;
      }
    
   }

   void SceneBase::RemoveChild( DeltaDrawable* child )
   {
      GetSceneNode()->removeChild(child->GetOSGNode());
      DeltaDrawable::RemoveChild(child);
   }


   void SceneBase::RemoveAllChildren()
   {
      while(GetNumChildren() > 0)
      {
         DeltaDrawable* dd = GetChild(0);
         RemoveChild(dd);
      }
   }

   void SceneBase::SetRenderOrder( RenderOrder r)
   {
      mRenderOrder = r;
   }

   RenderOrder SceneBase::GetRenderOrder() const
   {
      return mRenderOrder;
   }

   osg::Node* SceneBase::GetOSGNode()
   {
      return GetSceneNode();
   }

   const osg::Node* SceneBase::GetOSGNode() const
   {
      return GetSceneNode();
   }

   const SceneEnum& SceneBase::GetSceneEnum() const
   {
      return *mSceneEnum;
   }

   void SceneBase::SetSceneEnum( const SceneEnum& se)
   {
      mSceneEnum = &se;
   }

   bool SceneBase::ContainsActor( dtCore::DeltaDrawable& dd ) const
   {
      unsigned int numChildren = DeltaDrawable::GetNumChildren();
      for (unsigned int i = 0; i < numChildren; i++)
      {
         if( GetChild(i) == &dd)
         {
            return true;
         }
      }

      return false;
   }

   void SceneBase::GetAllActors( std::vector<dtCore::DeltaDrawable*>& vec )
   {
      unsigned int numChildren = DeltaDrawable::GetNumChildren();
      for (unsigned int i = 0; i < numChildren; i++)
      {
         vec.push_back(DeltaDrawable::GetChild(i));
      }
   }


}//namespace dtRender
