/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_CONTAINER_UTILS_H__
#define __DELTA_DTDIS_CONTAINER_UTILS_H__

namespace dtUtil
{
   /// find the iterator for the unique key-value pair.
   /// @param mmap the container instance to be searched.
   /// @param key the container's key to be found.
   /// @param the container's value matching key to be found.
   /// @return the found iterator, or the end of the container.
   template<class MMType>
   typename MMType::iterator find_multimap_pair(MMType& mmap,
                                                const typename MMType::key_type &key,
                                                const typename MMType::mapped_type &value)
   {
      typedef std::pair<typename MMType::iterator, typename MMType::iterator> IterPair;

      IterPair rangeIterators = mmap.equal_range(key);

      // Check to make sure that the processor we're trying to add is not already there
      while( rangeIterators.first != rangeIterators.second )
      {
         // If this processor with a matching id has a the exact same processor, bail
         if( (rangeIterators.first)->second == value )
         {
            return rangeIterators.first;
         }

         rangeIterators.first++;
      }

      // No matches were found in the loop
      return mmap.end();
   }

}

#endif  // __DELTA_DTDIS_CONTAINER_UTILS_H__
