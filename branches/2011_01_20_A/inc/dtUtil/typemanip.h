/* -*-c++-*-
 * Delta3D
 * Copyright 2007-2008, Alion Science and Technology
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
 * david
 */

#ifndef TYPEMANIP_H_
#define TYPEMANIP_H_

#include <dtUtil/generic.h>
////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

namespace dtUtil
{

////////////////////////////////////////////////////////////////////////////////
// class template IsSameType
// Return true iff two given types are the same
// Invocation: SameType<T, U>::value
// where:
// T and U are types
// Result evaluates to true iff U == T (types equal)
////////////////////////////////////////////////////////////////////////////////

    template <typename T, typename U>
    struct IsSameType
    {
        enum { value = false };
    };

    template <typename T>
    struct IsSameType<T,T>
    {
        enum { value = true };
    };

////////////////////////////////////////////////////////////////////////////////
// Helper types Small and Big - guarantee that sizeof(Small) < sizeof(Big)
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        template <class T, class U>
        struct ConversionHelper
        {
            typedef char Small;
            struct Big { char dummy[2]; };
            static Big   Test(...);
            static Small Test(U);
            static T MakeT();
        };
    }

////////////////////////////////////////////////////////////////////////////////
// class template Conversion
// Figures out the conversion relationships between two types
// Invocations (T and U are types):
// a) Conversion<T, U>::exists
// returns (at compile time) true if there is an implicit conversion from T
// to U (example: Derived to Base)
// b) Conversion<T, U>::exists2Way
// returns (at compile time) true if there are both conversions from T
// to U and from U to T (example: int to char and back)
// c) Conversion<T, U>::sameType
// returns (at compile time) true if T and U represent the same type
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

    template <class T, class U>
    struct Conversion
    {
        typedef Private::ConversionHelper<T, U> H;
#ifndef __MWERKS__
        enum { exists = sizeof(typename H::Small) == sizeof((H::Test(H::MakeT()))) };
#else
        enum { exists = false };
#endif
        enum { exists2Way = exists && Conversion<U, T>::exists };
        enum { sameType = false };
    };

    template <class T>
    struct Conversion<T, T>
    {
        enum { exists = 1, exists2Way = 1, sameType = 1 };
    };

    template <class T>
    struct Conversion<void, T>
    {
        enum { exists = 0, exists2Way = 0, sameType = 0 };
    };

    template <class T>
    struct Conversion<T, void>
    {
        enum { exists = 0, exists2Way = 0, sameType = 0 };
    };

    template <>
    struct Conversion<void, void>
    {
    public:
        enum { exists = 1, exists2Way = 1, sameType = 1 };
    };

////////////////////////////////////////////////////////////////////////////////
// class template SuperSubclass
// Invocation: SuperSubclass<B, D>::value where B and D are types.
// Returns true if B is a public base of D, or if B and D are aliases of the
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

template <class T, class U>
struct SuperSubclass
{
    enum { value = (::dtUtil::Conversion<const volatile U*, const volatile T*>::exists &&
                  !::dtUtil::Conversion<const volatile T*, const volatile void*>::sameType) };

    // Dummy enum to make sure that both classes are fully defined.
    enum{ dontUseWithIncompleteTypes = ( sizeof (T) == sizeof (U) ) };
};

template <>
struct SuperSubclass<void, void>
{
    enum { value = false };
};

template <class U>
struct SuperSubclass<void, U>
{
    enum { value = (::dtUtil::Conversion<const volatile U*, const volatile void*>::exists &&
                  !::dtUtil::Conversion<const volatile void*, const volatile void*>::sameType) };

    // Dummy enum to make sure that both classes are fully defined.
    enum{ dontUseWithIncompleteTypes = ( 0 == sizeof (U) ) };
};

template <class T>
struct SuperSubclass<T, void>
{
    enum { value = (::dtUtil::Conversion<const volatile void*, const volatile T*>::exists &&
                  !::dtUtil::Conversion<const volatile T*, const volatile void*>::sameType) };

    // Dummy enum to make sure that both classes are fully defined.
    enum{ dontUseWithIncompleteTypes = ( sizeof (T) == 0 ) };
};

////////////////////////////////////////////////////////////////////////////////
// class template SuperSubclassStrict
// Invocation: SuperSubclassStrict<B, D>::value where B and D are types.
// Returns true if B is a public base of D.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

template<class T,class U>
struct SuperSubclassStrict
{
    enum { value = (::dtUtil::Conversion<const volatile U*, const volatile T*>::exists &&
                 !::dtUtil::Conversion<const volatile T*, const volatile void*>::sameType &&
                 !::dtUtil::Conversion<const volatile T*, const volatile U*>::sameType) };

    // Dummy enum to make sure that both classes are fully defined.
    enum{ dontUseWithIncompleteTypes = ( sizeof (T) == sizeof (U) ) };
};

template<>
struct SuperSubclassStrict<void, void>
{
    enum { value = false };
};

template<class U>
struct SuperSubclassStrict<void, U>
{
    enum { value = (::dtUtil::Conversion<const volatile U*, const volatile void*>::exists &&
                 !::dtUtil::Conversion<const volatile void*, const volatile void*>::sameType &&
                 !::dtUtil::Conversion<const volatile void*, const volatile U*>::sameType) };

    // Dummy enum to make sure that both classes are fully defined.
    enum{ dontUseWithIncompleteTypes = ( 0 == sizeof (U) ) };
};

template<class T>
struct SuperSubclassStrict<T, void>
{
    enum { value = (::dtUtil::Conversion<const volatile void*, const volatile T*>::exists &&
                 !::dtUtil::Conversion<const volatile T*, const volatile void*>::sameType &&
                 !::dtUtil::Conversion<const volatile T*, const volatile void*>::sameType) };

    // Dummy enum to make sure that both classes are fully defined.
    enum{ dontUseWithIncompleteTypes = ( sizeof (T) == 0 ) };
};


}   // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D, or if B and D are aliases of the
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
// Deprecated: Use SuperSubclass class template instead.
////////////////////////////////////////////////////////////////////////////////

#define DTUTIL_SUPERSUBCLASS(T, U) \
    ::dtUtil::SuperSubclass<T,U>::value

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS_STRICT
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
// Deprecated: Use SuperSubclassStrict class template instead.
////////////////////////////////////////////////////////////////////////////////

#define DTUTIL_SUPERSUBCLASS_STRICT(T, U) \
    ::dtUtil::SuperSubclassStrict<T,U>::value


#endif /* TYPEMANIP_H_ */
