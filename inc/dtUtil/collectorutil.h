/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 * Matthew "w00by" Stokes with much help from Brad Anderegg
 */

#ifndef DELTA_COLLECTOR_UTIL
#define DELTA_COLLECTOR_UTIL

#include <string>
#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_START_CLANG("-Woverloaded-virtual")

namespace dtUtil
{

   namespace CollectorUtil
   {

      //This is needed VS 2003 does not have a value type on std::map

      /**
       * Template Function that is used to get the RefPtr's Element type from the map. In other words
       * this function gets the type of the Node or the Object that is stored in the map.
       */
      template <typename T>
      struct GetElementType
      {
      public:
         typedef typename T::allocator_type::value_type::second_type::element_type value_type;
      };

      /**
       * Template function that is used to find a node or object that is inside a map
       * @param nodeName The name of the Node or Object that you wish to find
       * @param nodeMap The map that you wish to search for the Node / Object
       * @return Will return a const pointer to a node or object that you were looking for if it is in the map.  Otherwise it will return null.
       */
      template <class mapType>
      const typename GetElementType<mapType>::value_type* FindNodePointer(const std::string& nodeName, const mapType& nodeMap)
      {
         typename mapType::const_iterator iter = nodeMap.find(nodeName);

         if (iter != nodeMap.end())
         {
            return (*iter).second.get();
         }
         return NULL;
      }

      /**
       * Template function that is used to find a node or object that is inside a map
       * @param nodeName The name of the Node or Object that you wish to find
       * @param nodeMap The map that you wish to search for the Node / Object
       * @return Will return a const pointer to the node or object that you were looking for if it is in the map.  Otherwise it will return null.
       */
      template <class mapType>
      typename GetElementType<mapType>::value_type* FindNodePointer(const std::string& nodeName, mapType& nodeMap)
      {
         typename mapType::iterator iter = nodeMap.find(nodeName);

         if (iter != nodeMap.end())
         {
            return (*iter).second.get();
         }
         return NULL;
      }

      /**
       * Template function that is used to Add a unique node or object to its respective map.
       * @param nodeName The name of the Node or Object that is being added to its map
       * @param nodeType The address of the node or object that you wish to add to its map
       * @param nodeMap The map that you want to add the node or object to
       * @return Will return true if the node / object was added; otherwise false will be returned
       */
      template <class mapType>
      bool AddNode (const std::string& nodeName, typename GetElementType<mapType>::value_type* nodeType, mapType& nodeMap )
      {
         if (FindNodePointer(nodeName, nodeMap) == NULL)
         {
            nodeMap.insert(std::make_pair(nodeName, nodeType));
            return true;
         }
         return false;
      }

      /**
       * Template function that is used to Remove a unique node or object from its respective map.
       * @param nodeName The name of the Node or Object that is being removed from its map
       * @param nodeMap The map that you want to remove the node or object from
       * @return Will return true if the node / object was found AND removed; otherwise false will be returned
       */
      template <class mapType>
      bool RemoveNode(const std::string& nodeName, mapType& nodeMap )
      {
         typename mapType::iterator foundIter = nodeMap.find(nodeName);
         if (foundIter != nodeMap.end())
         {
            nodeMap.erase(foundIter);
            return true;
         }
         return false;
      }


   } // namespace CollectorUtil


} // namespace dtUtil
DT_DISABLE_WARNING_END

#endif // DELTA_COLLECTOR_UTIL
