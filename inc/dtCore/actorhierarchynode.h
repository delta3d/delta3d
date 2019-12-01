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

#ifndef DELTA_ACTOR_HIERARCHY_NODE
#define DELTA_ACTOR_HIERARCHY_NODE

#include <dtCore/export.h>

#include <dtCore/refptr.h>

#include <osg/Referenced>

#include <vector>

/**
 * @class ActorHierarchyNode
 * @brief Allows a map to create a tree to structure all Drawable Actors in a Map.
 * This gives a user more control over a scene graph's layout in STAGE;
 * maps set up their Drawables according a hierarchy when loading.
 */
namespace dtCore
{
   class BaseActorObject;

   class DT_CORE_EXPORT ActorHierarchyNode : public osg::Referenced
   {
   public:
      /** Though the first parameter is a BaseActorObject, this class
       *  only accepts actors with Drawables in them at the moment.
       */
      ActorHierarchyNode(BaseActorObject* baseActor, bool isRoot = false);
      ~ActorHierarchyNode();

      /*
       * In addition to adding child to the list of this's children,
       * Automatically sets child's parent to this.
       * Returns false on failure
       */
      bool AddChild(ActorHierarchyNode* child);

      ///Returns false on failure
      bool RemoveChild(BaseActorObject* actorToRemove);

      ///Returns false on failure
      bool RemoveChild(unsigned int index);

      /** Returns null if there is no child at the given index.
       */
      ActorHierarchyNode* GetChild(unsigned int index) const;
      unsigned int GetNumChildren() const;

      BaseActorObject* GetBaseActorObject() const;

      ActorHierarchyNode* GetParent() const;
      void SetParent(ActorHierarchyNode* p, bool reallyReallyWantItNull = false);

   private:
      BaseActorObject*                                           mActorNode;
      dtCore::RefPtr<ActorHierarchyNode>                         mParent;
      std::vector<dtCore::RefPtr<ActorHierarchyNode> >           mChildren;
   };
}//namespace dtCore

#endif //DELTA_ACTOR_HIERARCHY_NODE
