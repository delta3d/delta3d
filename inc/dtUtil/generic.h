/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute
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
 * Aleksei Trunov
 */

// This file contains small collection of generic programming templates etc
// gathered from various sources (Loki library is the main one). Some of them
// has been modified, other has remained intact. This is done with the only goal
// to break external libraries dependencies for this test project.

#ifndef _DTUTIL_GENERIC_H_
#define _DTUTIL_GENERIC_H_

namespace dtUtil
{

// Some algorithms

template <bool flag, typename T, typename U>
struct Select
{
   typedef T Result;
};
template <typename T, typename U>
struct Select<false, T, U>
{
   typedef U Result;
};

template <int v> struct Int2Type { enum { value = v }; };

template<class T, int i> struct IsIntType { enum { value = false }; };
template<int i> struct IsIntType<Int2Type<i>, i> { enum { value = true }; };
template<class T, int i> struct NotIntType { enum { value = !IsIntType<T, i>::value }; };

// Type lists related

class NullType {};
struct EmptyType {};

template <class T, class U>
struct TypeList
{
   typedef T Head;
   typedef U Tail;
};

template <
   class T1 = NullType
   , class T2 = NullType
   , class T3 = NullType
   , class T4 = NullType
   , class T5 = NullType
   , class T6 = NullType
   , class T7 = NullType
   , class T8 = NullType
> struct CreateTL
{
   typedef TypeList<T1, typename CreateTL<T2, T3, T4, T5, T6, T7, T8>::Type> Type;
};
template<> struct CreateTL<NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType>
{
   typedef NullType Type;
};

template <
   int i1 = -1
   , int i2 = -1
   , int i3 = -1
   , int i4 = -1
   , int i5 = -1
   , int i6 = -1
   , int i7 = -1
   , int i8 = -1
> struct CreateIdsTL
{
   typedef TypeList<Int2Type<i1>, typename CreateIdsTL<i2, i3, i4, i5, i6, i7, i8, -1>::Type> Type;
};
template <> struct CreateIdsTL<-1, -1, -1, -1, -1, -1, -1, -1> { typedef NullType Type; };

template <class TL, int i = 0> struct IdsFromTL
{
   typedef TypeList<Int2Type<i>, typename IdsFromTL<typename TL::Tail, i+1>::Type> Type;
};
template <int i> struct IdsFromTL<NullType, i> { typedef NullType Type; };

template <class TList, class T>
struct AppendTL
{
   typedef TypeList<typename TList::Head, typename AppendTL<typename TList::Tail, T>::Type> Type;
};
template <class T>
struct AppendTL<NullType, T>
{
   typedef TypeList<T, NullType> Type;
};

#define TYPELIST_0() dtUtil::NullType
#define TYPELIST_1(T1) dtUtil::TypeList<T1, dtUtil::NullType>
#define TYPELIST_2(T1, T2) dtUtil::TypeList<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3) dtUtil::TypeList<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4) dtUtil::TypeList<T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5(T1, T2, T3, T4, T5) dtUtil::TypeList<T1, TYPELIST_4(T2, T3, T4, T5) >
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) dtUtil::TypeList<T1, TYPELIST_5(T2, T3, T4, T5, T6) >
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) dtUtil::TypeList<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) dtUtil::TypeList<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >

template <class TList> struct Length;
template <> struct Length<NullType>
{
   enum { value = 0 };
};
template <class T, class U>
struct Length< TypeList<T, U> >
{
   enum { value = 1 + Length<U>::value };
};

template <class TList, unsigned int i> struct TypeAt;
template <class T, class U>
struct TypeAt< TypeList<T, U>, 0 >
{
   typedef T Result;
};
template <class T, class U, unsigned int i>
struct TypeAt< TypeList<T, U>, i >
{
   typedef typename TypeAt<U, i - 1>::Result Result;
};

template <class TList, unsigned int i, typename DefType = NullType>
struct TypeAtNonStrict
{
   typedef DefType Result;
};
template <class T, class U, typename DefType>
struct TypeAtNonStrict< TypeList<T, U>, 0, DefType >
{
   typedef T Result;
};
template <class T, class U, unsigned int i, typename DefType>
struct TypeAtNonStrict< TypeList<T, U>, i, DefType >
{
   typedef typename TypeAtNonStrict<U, i - 1, DefType>::Result Result;
};

// Tuples-related

template <typename T, unsigned int i = 0> struct TupleHolder
{
   typedef T Type;
   typedef T StoredType;
   StoredType value;
   TupleHolder() {}
   TupleHolder(Type t) : value(t) {}
   TupleHolder& operator=(TupleHolder const& v) { value = v.value; }
};

template <typename TList, template <class, unsigned int> class Holder, unsigned int i = 0> struct InstantiateH;
template <typename T, typename U, template <class, unsigned int> class Holder, unsigned int i>
struct InstantiateH<TypeList<T, U>, Holder, i>
   : public Holder<typename TypeList<T, U>::Head, i>
   , public InstantiateH<typename TypeList<T, U>::Tail, Holder, i+1>
{
    enum { ordern = i };
   typedef Holder<typename TypeList<T, U>::Head, i> LeftBase;
   typedef InstantiateH<typename TypeList<T, U>::Tail, Holder, i+1> RightBase;
   InstantiateH(typename TypeList<T, U>::Head h, RightBase const& t) : LeftBase(h), RightBase(t) {}
   InstantiateH(typename TypeList<T, U>::Head h, NullType) : LeftBase(h) {}
   InstantiateH(typename TypeList<T, U>::Head h) : LeftBase(h) {}
   InstantiateH() {}
};
template <template <class, unsigned int> class Holder, unsigned int i>
struct InstantiateH<NullType, Holder, i>
{
   InstantiateH() {}
};

template <typename InstH, unsigned int j, unsigned int i = 0> struct TailAt;
template <typename T, typename U, template <class, unsigned int> class Holder, unsigned int i>
struct TailAt<InstantiateH<TypeList<T, U>, Holder, i>, 0, i>
{
   typedef InstantiateH<typename TypeList<T, U>::Tail, Holder, i+1> Result;
};
template <typename T, typename U, template <class, unsigned int> class Holder, unsigned int j, unsigned int i>
struct TailAt<InstantiateH<TypeList<T, U>, Holder, i>, j, i>
{
   typedef typename TailAt<InstantiateH<typename TypeList<T, U>::Tail, Holder, i+1>, j-1, i+1>::Result Result;
};

template <unsigned int j, typename InstH, unsigned int i = 0> struct InstantiateHAccessor;
template <typename T, typename U, template <class, unsigned int> class Holder, unsigned int i>
struct InstantiateHAccessor<0, InstantiateH<TypeList<T, U>, Holder, i>, i>
{
   typedef InstantiateH<TypeList<T, U>, Holder, i> Instance;
   typedef typename Instance::LeftBase TargetHolder;
   static inline TargetHolder& Get(Instance& h) { return static_cast<TargetHolder&>(h); }
   static inline TargetHolder const& Get(Instance const& h) { return static_cast<TargetHolder const&>(h); }
};
template <unsigned int j, typename T, typename U, template <class, unsigned int> class Holder, unsigned int i>
struct InstantiateHAccessor<j, InstantiateH<TypeList<T, U>, Holder, i>, i>
{
   typedef InstantiateH<TypeList<T, U>, Holder, i> Instance;
   typedef Holder<typename TypeAt<TypeList<T, U>, j>::Result, j+i> TargetHolder;
   typedef typename Instance::RightBase RightBase;
   static inline TargetHolder& Get(Instance& h) { return InstantiateHAccessor<j-1, RightBase, i+1>::Get(static_cast<RightBase&>(h)); }
   static inline TargetHolder const& Get(Instance const& h) { return InstantiateHAccessor<j-1, RightBase, i+1>::Get(static_cast<RightBase const&>(h)); }
};
template <unsigned int j, class Instantiated> inline
typename InstantiateHAccessor<j, Instantiated, Instantiated::ordern>::TargetHolder&
GetH(Instantiated& h)
{
   return InstantiateHAccessor<j, Instantiated, Instantiated::ordern>::Get(h);
}
template <unsigned int j, class Instantiated> inline
typename InstantiateHAccessor<j, Instantiated, Instantiated::ordern>::TargetHolder const&
GetH(Instantiated const& h)
{
   return InstantiateHAccessor<j, Instantiated, Instantiated::ordern>::Get(h);
}

}

#endif // _DTUTIL_GENERIC_H_
