/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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

#ifndef DELTA_WAYPOINTCOLLECTION
#define DELTA_WAYPOINTCOLLECTION

#include <dtAI/export.h>
#include <dtAI/tree.h>
#include <dtAI/waypointinterface.h>

#include <dtCore/refptr.h>

#include <vector>
#include <map>
#include <osg/Vec3>

namespace dtAI
{
   class WaypointPropertyBase;

   /**
    *   WaypointCollection is meant to be a container for waypoints that is also
    * a derivative of the WaypointInterface base.  Semantically all children of
    * a WaypointCollection can path to all other children, so it adding children
    * implies interconnectivity.
    */
   class DT_AI_EXPORT WaypointCollection : public dtAI::Tree<const WaypointInterface*>,
                                           public WaypointInterface
   {
   public:
      typedef dtAI::Tree<const WaypointInterface*> WaypointTree;
      typedef dtAI::Tree<const WaypointInterface*>::child_iterator WaypointTreeChildIterator;
      typedef dtAI::Tree<const WaypointInterface*>::child_iterator WaypointTreeConstChildIterator;

      typedef std::pair<WaypointID, WaypointID> ChildEdge;
      typedef std::multimap<WaypointID, ChildEdge> ChildEdgeMap;
      typedef std::vector<ChildEdge> ChildEdgeArray;

      WaypointCollection();
      WaypointCollection(const osg::Vec3& pos);
      WaypointCollection(const WaypointCollection& way);

   protected:
      /*virtual*/~WaypointCollection();

   public:
      /*virtual*/ WaypointTree::ref_pointer clone() const;

      void CleanUp();

      // these are pure virtual from WaypointInterface
      virtual void ref() const
      {
         osg::Referenced::ref();
      }

      virtual void unref() const
      {
         osg::Referenced::unref();
      }

      /**
       * A collection is marked as a leaf if it has any concrete nodes as children
       * as opposed to waypoint collections as children.
       */
      bool IsLeaf();

      /**
       * Our position and radius make up a bounding sphere.
       */
      float GetRadius();

      /*virtual*/ const osg::Vec3& GetPosition() const;
      /*virtual*/ void SetPosition(const osg::Vec3& pVec);

      /**
       * These are generic insert and remove functions which key off of the waypoint type
       * and cast it to a WaypointCollection if it is one.  Do not use the derived tree insert functions.
       */
      void Insert(const WaypointInterface* waypoint);
      void Remove(const WaypointInterface* waypoint);

      /**
      *  @return true if there is a waypoint with specified id as an immediate child.
      */
      bool HasChild(WaypointID id);

      WaypointCollection* GetParent();
      const WaypointCollection* GetParent() const;

      void Recalculate();
      
      void AddEdge(WaypointID sibling, const ChildEdge& edge);
      bool RemoveEdge(WaypointID sibling, const ChildEdge& edge);
      bool ContainsEdge(WaypointID sibling, const ChildEdge& edge);
      void GetEdges(WaypointID sibling, ChildEdgeArray& result) const;
      void ClearEdges();

      /*virtual*/ void CreateProperties(WaypointPropertyBase& container);

   protected:
      /**
       * If you specifically know whether you are adding a concrete waypoint or a child collection
       * you can use these functions, else use Insert() and Remove() above.
       */
      virtual void AddChild(dtCore::RefPtr<const WaypointCollection> waypoint);
      virtual void RemoveChild(dtCore::RefPtr<const WaypointCollection> waypoint);

      virtual void InsertWaypoint(const WaypointInterface* waypoint);
      virtual void RemoveWaypoint(const WaypointInterface* waypoint);

   private:
      // leaf nodes have only waypoints as children, not leafs have only waypoint collections as children
      bool mLeaf;
      float mRadius;
      osg::Vec3 mPosition;
      ChildEdgeMap mChildEdges;
   };

} // namespace dtAI

#endif // DELTA_WAYPOINTCOLLECTION
