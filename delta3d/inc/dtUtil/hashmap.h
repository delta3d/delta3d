/*
 * hashset.h
 *
 *  Created on: Jul 25, 2011
 *      Author: david
 */

#ifndef HASHSET_H_
#define HASHSET_H_

#ifdef __GNUG__
#  include <ext/hash_map>
#elif defined(_MSC_VER)
#  include <hash_map>
#else
#  include <map>
#endif

#include <dtUtil/hash.h>

namespace dtUtil
{

#if defined(_MSC_VER)
   template<class _Key, class _HashFcn, class _LessKey>
   struct HashCompare
   {
      size_t operator() (const _Key& k)
      {
         _HashFcn h
         return h(k);
      }
      bool operator() (const _Key& k1, const _Key& k2)
      {
         _LessKey l;
         return l(k1, k2);
      }
      
      enum
		{	// parameters for hash table
		bucket_size = 4,	// 0 < bucket_size
		min_buckets = 8
      };	// min_buckets = 2 ^^ N, 0 < N

   };
#elif defined(__GNUG__)
   template<class _Key, class _LessKey>
   struct HashEqual
   {
      bool operator() (const _Key& k1, const _Key& k2) const
      {
         _LessKey l;
         return !l(k1, k2) && !l(k2, k1);
      }
   };
#endif

   template<class _Key, class _Tp, class _HashFcn = dtUtil::hash<_Key>, class _LessKey = std::less<_Key>, class _Alloc = std::allocator<_Tp> >
   class HashMap : public
#ifdef __GNUG__
   __gnu_cxx::hash_map<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc >
#elif defined(_MSC_VER)
   stdext::hash_map<_Key, _Tp, dtUtil::HashCompare<_Key, _HashFcn, _LessKey>, _Alloc >
#else
   std::map<_Key, _Tp, _LessKey, _Alloc >
#endif
   {
    public:
         HashMap() {}
   };

   template<class _Key, class _Tp, class _HashFcn = dtUtil::hash<_Key>, class _LessKey = std::less<_Key>, class _Alloc = std::allocator<_Tp> >
   class HashMultiMap : public
#ifdef __GNUG__
   __gnu_cxx::hash_multimap<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc >
#elif defined(_MSC_VER)
   stdext::hash_multimap<_Key, _Tp, dtUtil::HashCompare<_Key, _HashFcn, _LessKey>, _Alloc >
#else
   std::multimap<_Key, _Tp, _LessKey, _Alloc >
#endif
   {
    public:
         HashMultiMap() {}
   };
}


#endif /* HASHSET_H_ */
