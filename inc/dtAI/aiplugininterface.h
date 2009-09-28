/* -*-c++-*-
 * Delta3D
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Bradley Anderegg
 */

#ifndef DELTA_AIPLUGININTERFACE_H
#define DELTA_AIPLUGININTERFACE_H

#include <string>
#include <vector>

#include <osg/Referenced>

#include <dtAI/pathfinding.h>
#include <dtAI/export.h>
#include <dtAI/waypointinterface.h>
#include <osg/Vec3>

#include <dtDAL/objecttype.h>
#include <dtUtil/objectfactory.h>

#include <dtAI/waypointpropertycache.h>

namespace dtAI
{
   class AIDebugDrawable;
   class WaypointCollection;
   class WaypointGraph;

   class DT_AI_EXPORT AIPluginInterface : public osg::Referenced
   {
   public:
      typedef std::vector<WaypointInterface*> WaypointArray;
      typedef std::vector<const WaypointInterface*> ConstWaypointArray;

      typedef dtUtil::ObjectFactory< dtCore::RefPtr<const dtDAL::ObjectType>, WaypointInterface > WaypointFactory;

   public: // interface declaration
      AIPluginInterface();
      virtual ~AIPluginInterface();

      /**
       * Inserts a waypoint into the system.
       * @note you do not need to call this if you created
       *  the waypoint through this interface.
       *
       * @return the waypoint created
       */
      virtual void InsertWaypoint(WaypointInterface* waypoint) = 0;

      /**
       * Removes a waypoint by Id from the system
       * @param the ID of the waypoint to remove
       * @return whether the waypoint was found and successfully removed
       */
      virtual bool RemoveWaypoint(WaypointInterface* waypoint) = 0;

      /**
       * The proper way to move a waypoint, re-inserting may will create duplicates
       * @param the waypoint which has presumably been moved
       * @return whether the waypoint was found and successfully moved
       */
      virtual bool MoveWaypoint(WaypointInterface* wi, const osg::Vec3& newPos) = 0;

      /**
       * This is for inserting a WaypointCollection at a specific search level,
       *  this is done on CreateSearchGraph(), use InsertWaypoint()
       *  for default inserting of WaypointCollections.
       */
      virtual void InsertCollection(WaypointCollection* waypoint, unsigned level) = 0;

      /**
       *  Takes the WaypointID of a waypoint already added, and assigns it
       *     to a parent waypoint which may or may not be already inserted.
       *
       *  @return returns false if childWP was not found, or if parentWp has already been added
       *             at the wrong search level
       *
       *  @note Calling this function will add the childWp to the parentWp using Insert().
       */
      virtual bool Assign(WaypointID childWp, WaypointCollection* parentWp) = 0;

      /**
       *  Returns a reference to the internal waypoint graph.  All operations on this
       *     interface are automatically applied to the waypoint graph.
       */
      virtual WaypointGraph& GetWaypointGraph() = 0;
      virtual const WaypointGraph& GetWaypointGraph() const = 0;

      /**
       *  Copies all waypoints into a vector
       *  @param the vector of waypoints to fill
       */
      virtual void GetWaypoints(WaypointArray& toFill) = 0;
      virtual void GetWaypoints(ConstWaypointArray& toFill) const = 0;

      /**
       * Finds the closest waypoint to a given point
       * @return the waypoint found, or NULL if no waypoints exist
       */
      virtual WaypointInterface* GetClosestWaypoint(const osg::Vec3& pos, float maxDistance) = 0;

      /**
       * Searches for waypoints within radius of a point.
       * @param pos, the point to search from
       * @param radius, it will find waypoints up to and not including this distance
       * @param arrayToFill, a std::vector<WaypointInterface*> by reference for the result
       */
      virtual bool GetWaypointsAtRadius(const osg::Vec3& pos, float radius, WaypointArray& arrayToFill) = 0;

      /**
       * Returns a waypoint by waypoint Id
       * @return the waypoint found, or NULL if no waypoint exists with that Id
       */
      virtual WaypointInterface* GetWaypointById(WaypointID id) = 0;
      virtual const WaypointInterface* GetWaypointById(WaypointID id) const = 0;

      /**
       * Returns a waypoint from the specific waypoint Id
       * @return the waypoint found, or NULL if no waypoint exists with that Id
       */
      virtual WaypointInterface* GetWaypointByName(const std::string& name) = 0;

      /**
       * Fills a vector of waypoints from all matching the specific name
       */
      virtual void GetWaypointsByName(const std::string& name, WaypointArray& arrayToFill) = 0;

      /**
       *  Copies all waypoints of a specific type into a vector
       *  @param the vector of waypoints to fill
       */
      virtual void GetWaypointsByType(const dtDAL::ObjectType& type, WaypointArray& toFill) = 0;

      /**
       * Creates a path between the specified waypoints
       */
      virtual void AddEdge(WaypointID pFrom, WaypointID pTo) = 0;

      /**
       * Removes the path segment between the two waypoints if one exists
       * @returns true if a path existed and it was succesfully new AIPluginInterface
       */
      virtual bool RemoveEdge(WaypointID pFrom, WaypointID pTo) = 0;

      /**
       * Removes all path segments from the specified waypoint
       */
      virtual void RemoveAllEdges(WaypointID pFrom) = 0;

      /**
       * Fills the specified vector with all the waypoints that
       *    are traversable from this waypoint
       *  @param the waypoint from which to find paths
       *  @param the waypoint array to fill
       */
      virtual void GetAllEdgesFromWaypoint(WaypointID pFrom, ConstWaypointArray& result) = 0;

      /**
       * Attempts to find a path between the specified waypoints at whichever search level the
       *   waypoints are currently on, hence it is a flat search.
       * @param from: the waypoint to start pathing from
       * @param to: the waypoint to path to
       * @param result: an empty waypoint vector to fill with the resulting path
       * @return whether a path was found successfully
       */
      virtual PathFindResult FindPath(WaypointID from, WaypointID to, ConstWaypointArray& result) = 0;

      /**
       * Attempts to find a hierarchical path between the specified waypoints by finding their common WaypointCollection
       *   parent and resolving the path at each step.
       * @param from: the waypoint to start pathing from
       * @param to: the waypoint to path to
       * @param result: an empty waypoint vector to fill with the resulting path
       * @return whether a path was found successfully
       */
      virtual PathFindResult HierarchicalFindPath(WaypointID from, WaypointID to, ConstWaypointArray& result) = 0;

      /**
       * Loads the waypoint file into the system
       * @param the name of the waypoint file
       * @return whether or not the load was successful
       */
      virtual bool LoadWaypointFile(const std::string& filename) = 0;

      /**
       * Serializes the waypoint data to file
       * @param the name of the waypoint file, or empty for save the last opened file.
       * @return whether the waypoint file was saved successfully
       */
      virtual bool SaveWaypointFile(const std::string& filename = "") = 0;

      /**
       *   This method is used for visualizing the internal data in the AI system
       * @return A DeltaDrawable to add to your Scene, this will most likely create
       *            one the first time this is called.
       */
      virtual AIDebugDrawable* GetDebugDrawable() = 0;

      /**
       *   Purges the cache, clears all pathing information
       * most likely you will want to call this on map close or reload
       */
      virtual void ClearMemory() = 0;

      /**
       *   Use the Waypoint factory to create a Waypoint.
       *  @note This will auto insert, no need to call Insert() afterwards
       */
      WaypointInterface* CreateWaypoint(const osg::Vec3& pos, const dtDAL::ObjectType& type);

      /**
       *   This creates a Waypoint like above but ensures there is not already a Waypoint within radius from at that point.
       *  @note This will auto insert, no need to call Insert() afterwards
       *
       *  @return Either a new waypoint or the waypoint that already exists within radius
       */
      WaypointInterface* CreateWaypointNoDuplicates(const osg::Vec3& pos, float radius, const dtDAL::ObjectType& type);

      /**
       * Checks to see if this registry supports the given waypoint type.
       * @param type The type to check support for.
       * @return true if supported, false otherwise.
       */
      bool IsWaypointTypeSupported(dtCore::RefPtr<const dtDAL::ObjectType> type) const;

      /**
       * Gets a list of waypoint types supported.
       */
      void GetSupportedWaypointTypes(std::vector<dtCore::RefPtr<const dtDAL::ObjectType> >& actors) const;

      const dtDAL::ObjectType* GetWaypointTypeByName(const std::string& name) const;

      WaypointPropertyBase* CreateWaypointPropertyContainer(const dtDAL::ObjectType& type, WaypointInterface* wp) const
      {
         return mPropertyCache->GetPropertyContainer(type, wp);
      }

      /**
       *  This simply uses the factory to create the waypoint, it does not insert it.
       */
      WaypointInterface* CreateNoInsert(const dtDAL::ObjectType& type);

      template <class WaypointDerivative>
      void RegisterWaypointType(dtCore::RefPtr<const dtDAL::ObjectType> type)
      {
         mFactory->RegisterType<WaypointDerivative>(type);
         mPropertyCache->RegisterType<WaypointDerivative>(type);
      }

   protected:

   private:
      dtCore::RefPtr<WaypointFactory> mFactory;
      dtCore::RefPtr<WaypointPropertyCache> mPropertyCache;
   };

} // namespace dtAI

#endif // DELTA_AIPLUGININTERFACE_H
