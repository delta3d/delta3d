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

#include <osg/Vec3>


namespace dtAI 
{
   class WaypointPropertyBase;
   class NavMesh;

   /**
    *	WaypointCollection is meant to be a container for waypoints that is also
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

         WaypointCollection();
         WaypointCollection(const osg::Vec3& pos);
         //WaypointCollection(const NavMesh& children);
         WaypointCollection(const WaypointCollection& way);

   protected:
         /*virtual*/~WaypointCollection();

   public:

         /*virtual*/ WaypointTree::ref_pointer clone() const;

         void CleanUp();

         /**
         * A collection is marked as a leaf if it has any concrete nodes as children
         *   as opposed to waypoint collections as children.
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
         *  and cast it to a WaypointCollection if it is one.
         *
         *  If you know whether you are adding a concrete waypoint or collection you may use
         *  the functions below to short circuit the check.
         */
         void Insert(const WaypointInterface* waypoint);
         void Remove(const WaypointInterface* waypoint);

         //may return null if is leaf node
         //WaypointCollection* FindClosestChild(const WaypointInterface& waypoint);
         //const WaypointCollection* FindClosestChild(const WaypointInterface& waypoint) const;

         void Recalculate();      

         /*virtual*/ void CreateProperties(WaypointPropertyBase& container);

         NavMesh& GetNavMesh();
         const NavMesh& GetNavMesh() const;
         
         //void GetAllChildPathsFrom(const WaypointInterface* wi, WaypointGraph::ConstWaypointArray& result);
         //void GetAllChildPathsTo(const WaypointInterface* wi, WaypointGraph::ConstWaypointArray& result);


         //these are pure virtual from WaypointInterface
         virtual void ref() const
         {
            osg::Referenced::ref();
         }

         virtual void unref() const
         {
            osg::Referenced::unref();
         }

      protected:

         /**
         * If you specifically know whether you are adding a concrete waypoint or a child collection
         *  you can use these functions, else use Insert() and Remove() above.
         */
         virtual void AddChild(dtCore::RefPtr<const WaypointCollection> waypoint);
         virtual void RemoveChild(dtCore::RefPtr<const WaypointCollection> waypoint);

         virtual void InsertWaypoint(const WaypointInterface* waypoint);
         virtual void RemoveWaypoint(const WaypointInterface* waypoint);

      private:

         //leaf nodes have only waypoints as children, not leafs have only waypoint collections as children
         bool mLeaf;
         float mRadius;
         osg::Vec3 mPosition;
         dtCore::RefPtr<NavMesh> mChildEdges;
   };


}//namespace 

#endif // DELTA_WAYPOINTCOLLECTION
