#ifndef _DELTA_CONTAINER_UTILS_H_
#define _DELTA_CONTAINER_UTILS_H_

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

#endif  // _DELTA_CONTAINER_UTILS_H_
