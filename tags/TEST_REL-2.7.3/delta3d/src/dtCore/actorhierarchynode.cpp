/* 
 * Delta3D Open Source Game and Simulation Engine 
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
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/actorhierarchynode.h>

#include <dtCore/baseactorobject.h>

#include <dtUtil/log.h>

namespace dtCore
{
////////////////////////////////////////////////////////////////////////////////
ActorHierarchyNode::ActorHierarchyNode(BaseActorObject* baseActor, bool isRoot)
   : mActorNode(baseActor)
   , mParent(NULL)
   , mChildren()
{
   //Someday we may do more than Drawables, but not today.
   //Root doesn't have to ANY BaseActorObject, because it's going to be a Scene,
   //which doesn't have an Actor
   if (!isRoot && baseActor->GetDrawable() == NULL)
   {
      LOG_ERROR("Can't create a non-root ActorHierarchyNode from a non-Drawable Actor.\n");
      mActorNode = NULL;
      delete this;
   }
}

////////////////////////////////////////////////////////////////////////////////
ActorHierarchyNode::~ActorHierarchyNode()
{
}

////////////////////////////////////////////////////////////////////////////////
bool ActorHierarchyNode::AddChild(ActorHierarchyNode* child)
{
   mChildren.push_back(child);
   child->SetParent(this);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool ActorHierarchyNode::RemoveChild(BaseActorObject* actorToRemove)
{
   dtCore::UniqueId id = actorToRemove->GetId();
   unsigned int i;

   for (i = 0; i < GetNumChildren(); ++i)
   {
      if (GetChild(i)->GetBaseActorObject()->GetId() == id)
      {
         RemoveChild(i);
         return true;
      }
   }  

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool ActorHierarchyNode::RemoveChild(unsigned int index)
{
   mChildren.erase(mChildren.begin() + index);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
ActorHierarchyNode* ActorHierarchyNode::GetChild(unsigned int index) const
{   
   try 
   {
      ActorHierarchyNode* n = mChildren.at(index);
      return n;
   }
   catch (...)
   {
      return NULL;
   }
}

////////////////////////////////////////////////////////////////////////////////
unsigned int ActorHierarchyNode::GetNumChildren() const
{
   return mChildren.size();
}

////////////////////////////////////////////////////////////////////////////////
BaseActorObject* ActorHierarchyNode::GetBaseActorObject() const
{
   return mActorNode;
}

////////////////////////////////////////////////////////////////////////////////
ActorHierarchyNode* ActorHierarchyNode::GetParent() const
{
   return mParent;
}

////////////////////////////////////////////////////////////////////////////////
void ActorHierarchyNode::SetParent(ActorHierarchyNode* p, 
                                   bool reallyReallyWantItNull)
{
   if (! reallyReallyWantItNull)
   {
      if (p == NULL)
      {
         LOG_ERROR("Setting a non-root ActorHierarchy parent's node to NULL. Bad idea.\n");
         return;
      }
   }
   mParent = p;
}

}//namespace dtCore


