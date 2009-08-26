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

#ifndef __DELTA_AIPLUGININTERFACE_H__
#define __DELTA_AIPLUGININTERFACE_H__

#include <string>
#include <vector>

#include <osg/Referenced>

#include <dtAI/export.h>
#include <dtAI/waypointinterface.h>
#include <osg/Vec3>

#include <dtDAL/objecttype.h>
#include <dtUtil/objectfactory.h>

#include <dtAI/waypointpropertycache.h>

namespace dtAI
{
   class AIDebugDrawable;

   class DT_AI_EXPORT AIPluginInterface: public osg::Referenced
   {
      public: 
         typedef std::vector<WaypointInterface*> WaypointArray;      
         typedef dtUtil::ObjectFactory< dtCore::RefPtr<const dtDAL::ObjectType>, WaypointInterface> WaypointFactory;

      public: //interface declaration
         AIPluginInterface();
         virtual ~AIPluginInterface();

         /**
         * Inserts a waypoint by position into the system
         * @return the waypoint created
         */
         virtual WaypointID InsertWaypoint(const osg::Vec3& pos) = 0;

         /**
         * Removes a waypoint by Id from the system
         * @param the ID of the waypoint to remove
         * @return whether the waypoint was found and successfully removed
         */
         virtual bool RemoveWaypoint(WaypointID id) = 0;

         /**
         * Finds the closest waypoint to a given point
         * @return the waypoint found, or NULL if no waypoints exist
         */
         virtual WaypointInterface* GetClosestWaypoint(const osg::Vec3& pos) = 0;


         /**
         * Returns a waypoint by waypoint Id
         * @return the waypoint found, or NULL if no waypoint exists with that Id
         */
         virtual WaypointInterface* GetWaypointById(WaypointID id) = 0;


         /**
         * Returns a waypoint from the specific waypoint Id
         * @return the waypoint found, or NULL if no waypoint exists with that Id
         */
         virtual WaypointInterface* GetWaypointByName(const std::string& name) = 0;


         /**
         *  Copies all waypoints into a vector
         *  @param the vector of waypoints to fill
         */
         virtual void GetWaypoints(WaypointArray& toFill) = 0;


         /**
         * Creates a path between the specified waypoints
         */
         virtual void AddPathSegment(WaypointID pFrom, WaypointID pTo) = 0;

         /**
         * Removes the path segment between the two waypoints if one exists         
         * @returns true if a path existed and it was succesfully new AIPluginInterface
         */
         virtual bool RemovePathSegment(WaypointID pFrom, WaypointID pTo) = 0;

         /**
         * Removes all path segments from the specified waypoint
         */
         virtual void RemoveAllPaths(WaypointID pFrom) = 0;

         /**
         * Fills the specified vector with all the waypoints that 
         *    are traversable from this waypoint
         *  @param the waypoint from which to find paths
         *  @param the waypoint array to fill
         */
         virtual void GetAllPathsFromWaypoint(WaypointID pFrom, WaypointArray& result) = 0;


         /**
         * Attempts to find a path between the specified waypoints 
         * @param from: the waypoint to start pathing from
         * @param to: the waypoint to path to
         * @param result: an empty waypoint vector to fill with the resulting path
         * @return whether a path was found successfully
         */
         virtual bool FindPath(WaypointID from, WaypointID to, WaypointArray& result) = 0;


         /**
         * Loads the waypoint file into the system
         * @param the name of the waypoint file
         * @return whether or not the load was successful
         */
         virtual bool LoadWaypointFile(const std::string& filename) = 0;

         /**
         * Serializes the waypoint data to file
         * @param the name of the waypoint file
         * @return whether the waypoint file was saved successfully
         */
         virtual bool SaveWaypointFile(const std::string& filename) = 0;


         /**
          *	This method is used for visualizing the internal data in the AI system
          * @return A DeltaDrawable to add to your Scene, this will most likely create
          *            one the first time this is called.
          */
         virtual AIDebugDrawable* GetDebugDrawable() = 0;


         /**
         *	Purges the cache, clears all pathing information
         * most likely you will want to call this on map close or reload
         */
         virtual void ClearMemory() = 0;

         /**
         *	Use the Waypoint factory to create a Waypoint.
         */
         WaypointInterface* CreateWaypoint(const dtDAL::ObjectType& type) const;

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

         template <class WaypointDerivative> 
         WaypointPropertyBase* CreateWaypointPropertyContainer(const dtDAL::ObjectType& type, WaypointDerivative* wp) const
         {
            return mPropertyCache->GetPropertyContainer(type, wp);
         }

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

}// namespace dtAI

#endif /* __DELTA_AIPLUGININTERFACE_H__ */
