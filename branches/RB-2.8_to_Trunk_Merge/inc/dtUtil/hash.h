/*
 * hash.h
 *
 *  Created on: Jul 25, 2011
 *      Author: david
 */

#ifndef HASH_H_
#define HASH_H_

#include <string>
#include <dtCore/refptr.h>

namespace dtUtil
{
   template<class _Key> struct hash;

   inline size_t
   __hash_string(const char* __s)
   {
     unsigned long __h = 0;
     for ( ; *__s; ++__s)
       __h = 5 * __h + *__s;
     return size_t(__h);
   }

   template<>
    struct hash<std::string>
    {
     size_t operator()(const std::string& string) const
     { return __hash_string(string.c_str()); }
    };

   template<>
     struct hash<char*>
     {
       size_t
       operator()(const char* __s) const
       { return __hash_string(__s); }
     };

   template<>
     struct hash<const char*>
     {
       size_t
       operator()(const char* __s) const
       { return __hash_string(__s); }
     };

   template<>
     struct hash<char>
     {
       size_t
       operator()(char __x) const
       { return __x; }
     };

   template<>
     struct hash<unsigned char>
     {
       size_t
       operator()(unsigned char __x) const
       { return __x; }
     };

   template<>
     struct hash<signed char>
     {
       size_t
       operator()(unsigned char __x) const
       { return __x; }
     };

   template<>
     struct hash<short>
     {
       size_t
       operator()(short __x) const
       { return __x; }
     };

   template<>
     struct hash<unsigned short>
     {
       size_t
       operator()(unsigned short __x) const
       { return __x; }
     };

   template<>
     struct hash<int>
     {
       size_t
       operator()(int __x) const
       { return __x; }
     };

   template<>
     struct hash<unsigned int>
     {
       size_t
       operator()(unsigned int __x) const
       { return __x; }
     };

   template<>
     struct hash<long>
     {
       size_t
       operator()(long __x) const
       { return __x; }
     };

   template<>
     struct hash<unsigned long>
     {
       size_t
       operator()(unsigned long __x) const
       { return __x; }
     };

   template<class _Key>
    struct hash<dtCore::RefPtr<_Key> >
    {
     size_t operator()(const dtCore::RefPtr<_Key>& keyPtr) const
     { return size_t(keyPtr->get()); }
    };

   template<class _Key>
    struct hash<osg::ref_ptr<_Key> >
    {
     size_t operator()(const dtCore::RefPtr<_Key>& keyPtr) const
     { return size_t(keyPtr->get()); }
    };

   template<class _Key>
    struct hash<_Key*>
    {
     size_t operator()(const _Key* keyPtr) const
     { return size_t(keyPtr); }
    };
}

#endif /* HASH_H_ */
