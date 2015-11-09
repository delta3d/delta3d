/*
 * enumtofunction.h
 *
 *  Created on: Dec 2, 2014
 *      Author: david
 */

#ifndef ENUMTOFUNCTION_H_
#define ENUMTOFUNCTION_H_

#include <dtUtil/enumeration.h>
#include <dtUtil/functor.h>
#include <dtUtil/hashmap.h>

namespace dtUtil
{
   template <typename EnumType, typename TList, typename R = void>
   class EnumerationToFunction
   {
   public:
      // The functor can have a return type, but since this will call a list of them, it will be lost.
      typedef Functor<R, TList> FunctorType;
   private:
      typedef HashMultiMap<EnumType*, FunctorType> TableType;
      typedef typename TableType::iterator TableIter;
      typedef typename std::pair<TableIter, TableIter> TableRangePair;

   public:

      EnumerationToFunction() {}

      void Insert(EnumType& key, FunctorType& func)
      {
         mCallTable.insert(std::make_pair(&key, func));
      }

      /**
       * Removes all calls with the given key.
       */
      void Remove(EnumType& key)
      {
         mCallTable.erase(&key);
      }

      void Remove(EnumType& key, FunctorType& func)
      {
         TableRangePair range = mCallTable.equal_range(&key);
         while (range.first != range.second)
         {
            if (range.first->second == func)
            {
               range.first = mCallTable.erase(range.first);
            }
            else
            {
               ++range.first;
            }
         }
      }

      typedef typename TypeAtNonStrict<TList, 0, NullType>::Result Parm1;
      typedef typename TypeAtNonStrict<TList, 1, NullType>::Result Parm2;
      typedef typename TypeAtNonStrict<TList, 2, NullType>::Result Parm3;
      typedef typename TypeAtNonStrict<TList, 3, NullType>::Result Parm4;
      typedef typename TypeAtNonStrict<TList, 4, NullType>::Result Parm5;
      typedef typename TypeAtNonStrict<TList, 5, NullType>::Result Parm6;
      typedef typename TypeAtNonStrict<TList, 6, NullType>::Result Parm7;
      inline void operator()(EnumType& key) const
      {
         TableRangePair range = mCallTable.equal_range(&key);
         for (; range.first != range.second; ++range.first)
            range.first->second();
      }
      inline void operator()(EnumType& key, Parm1 p1) const
      {
         TableRangePair range = mCallTable.equal_range(&key);
         for (; range.first != range.second; ++range.first)
            range.first->second(p1);
      }

      inline void operator()(EnumType& key, Parm1 p1, Parm2 p2) const
      {
         TableRangePair range = mCallTable.equal_range(&key);
         for (; range.first != range.second; ++range.first)
            range.first->second(p1, p2);
      }
      inline void operator()(EnumType& key, Parm1 p1, Parm2 p2, Parm3 p3) const
      {
         TableRangePair range = mCallTable.equal_range(&key);
         for (; range.first != range.second; ++range.first)
            range.first->second(p1, p2, p3);
      }
      inline void operator()(EnumType& key, Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4) const
      {
         TableRangePair range = mCallTable.equal_range(&key);
         for (; range.first != range.second; ++range.first)
            range.first->second(p1, p2, p3, p4);
      }
      inline void operator()(EnumType& key, Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5) const
      {
         TableRangePair range = mCallTable.equal_range(&key);
         for (; range.first != range.second; ++range.first)
            range.first->second(p1, p2, p3, p4, p5);
      }
      inline void operator()(EnumType& key, Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6) const
      {
         TableRangePair range = mCallTable.equal_range(&key);
         for (; range.first != range.second; ++range.first)
            range.first->second(p1, p2, p3, p4, p5, p6);
      }
      inline void operator()(EnumType& key, Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7) const
      {
         TableRangePair range = mCallTable.equal_range(&key);
         for (; range.first != range.second; ++range.first)
            range.first->second(p1, p2, p3, p4, p5, p6, p7);
      }

   private:
      TableType mCallTable;
   };

}
#endif /* ENUMTOFUNCTION_H_ */
