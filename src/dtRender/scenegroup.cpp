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

#include <dtRender/scenegroup.h>

#include <dtUtil/log.h>

#include <algorithm> //for std::find on msvc9.0

namespace dtRender
{

   const dtCore::RefPtr<SceneType> SceneGroup::SCENE_GROUP(new SceneType("Scene Group", "Scene", "A scene which holds child scenes."));

   class SceneGroupImpl
   {
   public:
      SceneGroupImpl()
         : mNode(new osg::Group())
      {
         
      }
      ~SceneGroupImpl()
      {
         mChildren.clear();
         mNode = NULL;
      }

      dtCore::RefPtr<osg::Group> mNode;
      SceneGroup::SceneArray mChildren;
   };

   SceneGroup::SceneGroup()
   : BaseClass(*SCENE_GROUP, SceneEnum::DEFAULT_SCENE)   
   , mImpl(new SceneGroupImpl())
   {
      SetName("SceneGroup");
   }

   SceneGroup::SceneGroup(const SceneType& sceneId, const SceneEnum& defaultScene)
      : BaseClass(sceneId, defaultScene)   
      , mImpl(new SceneGroupImpl())
   {
      SetName("SceneGroup");
   }


   SceneGroup::~SceneGroup()
   {
      delete mImpl;
   }


   void SceneGroup::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
     
   }


   bool SceneGroup::AddScene(SceneBase& sb)
   {      
      //this inserts sorted in order of the scene children's render order
      SceneGroup::SceneArray::iterator iter = mImpl->mChildren.begin();
      SceneGroup::SceneArray::iterator iterEnd = mImpl->mChildren.end();
      for (;iter != iterEnd; ++iter)
      {
         SceneBase* childScene = (*iter).get();
         if(sb.GetChildOrder() < childScene->GetChildOrder())
         {
            unsigned int childNum = mImpl->mNode->getChildIndex(childScene->GetOSGNode());

            mImpl->mChildren.insert(iter, &sb);
            GetSceneNode()->insertChild(childNum, sb.GetOSGNode());
               
            //return early if we successfully insert
            return DeltaDrawable::AddChild(&sb);
         }
      }

      //if we get here, either we have no children, or this child
      //belongs at the end of the list
      mImpl->mChildren.push_back(&sb);
      GetSceneNode()->addChild(sb.GetOSGNode());
      return DeltaDrawable::AddChild(&sb);      
   }
   

   bool SceneGroup::AddChild( DeltaDrawable* child )
   {
      bool isScene = false;
      SceneBase* scene = dynamic_cast<SceneBase*>(child);

      if(scene != NULL)
      {
         isScene = true;
      }

      //first try to add it to our children
      SceneArray::iterator iter = mImpl->mChildren.begin();
      SceneArray::iterator iterEnd = mImpl->mChildren.end();
      for (; iter != iterEnd; ++iter)
      {
         //if the child is a scene we can only add it to scene groups
         if(!isScene || (*iter)->GetAsSceneGroup() != NULL)
         {
            //returns on the first scene that accepts the child
            if((*iter)->AddChild(child))
            {
               return true;
            }
         }
      }
   
      //else see if it is a scene      
      if(isScene)
      {
         return AddScene(*scene);
      }

      //else we have no children that can accept a regular drawable
      return false;
   }

   void SceneGroup::RemoveChild( DeltaDrawable* child )
   {
      SceneBase* sb = dynamic_cast<SceneBase*>(child);
      //we should only have scene children
      if(sb != NULL)
      {
         //the remove must first traverse all scene children for cleanup
         sb->RemoveAllChildren();

         SceneArray::iterator iter = std::find(mImpl->mChildren.begin(), mImpl->mChildren.end(), sb);

         //remove from internal child array
         if (iter != mImpl->mChildren.end())
         {
            mImpl->mChildren.erase(iter);
         }
         else
         {
            LOG_ERROR("Error removing child from scene.");
         }

         //SceneBase handles removing the OSG node and calling the DeltaDrawable interface
         BaseClass::RemoveChild(sb);
      }
      else
      {
         if(child->GetParent() == this)
         {
            LOG_ERROR("SceneGroup has a non scene child");
         }
         else
         {
            LOG_ERROR("Called RemoveChild() on non child actor.");
         }
      }
   }

   osg::Group* SceneGroup::GetSceneNode()
   {
      return mImpl->mNode.get();
   }

   const osg::Group* SceneGroup::GetSceneNode() const
   {
      return mImpl->mNode.get();
   }

   void SceneGroup::GetSceneChildren( SceneArray& toFill )
   {
      SceneArray::iterator iter = mImpl->mChildren.begin();
      SceneArray::iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         toFill.push_back(*iter);
      }
   }

   SceneBase* SceneGroup::GetSceneChild( unsigned index )
   {
      SceneBase* se = NULL;
      if(index < mImpl->mChildren.size())
      {
         se = mImpl->mChildren[index];
      }
      return se;
   }

   const SceneBase* SceneGroup::GetSceneChild( unsigned index ) const
   {
      const SceneBase* se = NULL;
      if(index < mImpl->mChildren.size())
      {
         se = mImpl->mChildren[index];
      }
      return se;
   }

   unsigned int SceneGroup::GetNumSceneChildren() const
   {
      return mImpl->mChildren.size();
   }

   bool SceneGroup::ContainsActor( dtCore::DeltaDrawable& dd ) const
   {
      SceneArray::iterator iter = mImpl->mChildren.begin();
      SceneArray::iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         SceneBase* g = (*iter).get();
         if(g == &dd || g->ContainsActor(dd))
         {
            return true;
         }
      }

      return false;
   }

   SceneBase* SceneGroup::FindSceneByType( SceneType& st)
   {

      SceneArray::iterator iter = mImpl->mChildren.begin();
      SceneArray::iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         SceneBase* s = (*iter).get();
         if(s->GetType() == st)
         {
            return s;
         }
         else if (s->GetAsSceneGroup() != NULL)
         {
            SceneBase* sb = s->GetAsSceneGroup()->FindSceneByType(st);
            if(sb != NULL)
            {
               return sb;
            }
         }
      }

      return NULL;
   }

   const SceneBase* SceneGroup::FindSceneByType( SceneType& st) const
   {

      SceneArray::const_iterator iter = mImpl->mChildren.begin();
      SceneArray::const_iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         const SceneBase* s = (*iter).get();
         if(s->GetType() == st)
         {
            return s;
         }
         else if (s->GetAsSceneGroup() != NULL)
         {
            const SceneBase* sb = s->GetAsSceneGroup()->FindSceneByType(st);
            if(sb != NULL)
            {
               return sb;
            }
         }
      }

      return NULL;
   }

   void SceneGroup::GetAllScenesByType( SceneType& st, std::vector<SceneBase*>& toFill)
   {
      SceneArray::iterator iter = mImpl->mChildren.begin();
      SceneArray::iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         SceneBase* s = (*iter).get();
         if(s->GetType() == st)
         {
            toFill.push_back(s);
         }

         if(s->GetAsSceneGroup() != NULL)
         {
            s->GetAsSceneGroup()->GetAllScenesByType(st, toFill);
         }
      }
   }

   SceneGroup::SceneArray& SceneGroup::GetChildArray()
   {
      return mImpl->mChildren;
   }

   const SceneGroup::SceneArray& SceneGroup::GetChildArray() const
   {
      return mImpl->mChildren;
   }

}//namespace dtRender
