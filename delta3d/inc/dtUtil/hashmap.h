/*
 * hashset.h
 *
 *  Created on: Jul 25, 2011
 *      Author: david
 */

#ifndef HASHSET_H_
#define HASHSET_H_

#if defined( _LIBCPP_VERSION ) || (defined(_MSC_VER) && _MSC_VER >= 1700)
#  include <unordered_map>
#  define _UNORDERED_MAP
#elif defined(__GNUG__)
#  include <ext/hash_map>
#elif defined(_MSC_VER) 
#  include <hash_map>
#else
#  include <map>
#endif

#include <dtUtil/hash.h>

namespace dtUtil
{

#if defined(_MSC_VER) && ! defined(_UNORDERED_MAP)
   template<class _Key, class _HashFcn, class _LessKey>
   struct HashCompare
   {
      size_t operator() (const _Key& k) const
      {
         _HashFcn h;
         return h(k);
      }
      bool operator() (const _Key& k1, const _Key& k2) const
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
#else
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

   template<class _Key, class _Tp, class _HashFcn = dtUtil::hash<_Key>, class _LessKey = std::less<_Key>, class _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
   class HashMap : public
#if defined(_UNORDERED_MAP)
   std::unordered_map<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc >
   {
   public:
      typedef std::unordered_map<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc > BaseClass;
#elif defined(__GNUG__)
   __gnu_cxx::hash_map<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc >
   {
   public:
      typedef __gnu_cxx::hash_map<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc > BaseClass;
#elif defined(_MSC_VER)
   stdext::hash_map<_Key, _Tp, dtUtil::HashCompare<_Key, _HashFcn, _LessKey>, _Alloc >
   {
   public:
      typedef stdext::hash_map<_Key, _Tp, dtUtil::HashCompare<_Key, _HashFcn, _LessKey>, _Alloc > BaseClass;
#else
   std::map<_Key, _Tp, _LessKey, _Alloc >
   {
   public:
      typedef std::map<_Key, _Tp, _LessKey, _Alloc > BaseClass;
#endif
      using typename BaseClass::iterator;
      using typename BaseClass::const_iterator;
      HashMap() {}
   };

   template<class _Key, class _Tp, class _HashFcn = dtUtil::hash<_Key>, class _LessKey = std::less<_Key>, class _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
   class HashMultiMap : public
#if defined(_UNORDERED_MAP)
   std::unordered_multimap<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc >
   {
   public:
      typedef std::unordered_multimap<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc > BaseClass;
#elif defined(__GNUG__)
   __gnu_cxx::hash_multimap<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc >
   {
   public:
      typedef __gnu_cxx::hash_multimap<_Key, _Tp, _HashFcn, dtUtil::HashEqual<_Key, _LessKey>, _Alloc > BaseClass;
#elif defined(_MSC_VER)
   stdext::hash_multimap<_Key, _Tp, dtUtil::HashCompare<_Key, _HashFcn, _LessKey>, _Alloc >
   {
   public:
      typedef stdext::hash_multimap<_Key, _Tp, dtUtil::HashCompare<_Key, _HashFcn, _LessKey>, _Alloc > BaseClass;
#else
   std::multimap<_Key, _Tp, _LessKey, _Alloc >
   {
   public:
      typedef std::multimap<_Key, _Tp, _LessKey, _Alloc > BaseClass;
#endif
      using typename BaseClass::iterator;
      using typename BaseClass::const_iterator;
      HashMultiMap() {}
   };
}


#endif /* HASHSET_H_ */
