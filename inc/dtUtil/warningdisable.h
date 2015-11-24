/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2013 David Guthrie
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
 * David Guthrie
 */

#ifndef WARNINGDISABLE_H_
#define WARNINGDISABLE_H_

#if defined(_MSC_VER) && _MSC_VER >= 1600
#   define DT_DISABLE_WARNING_START_CLANG(...) \
            __pragma(warning(push))
#   define DT_DISABLE_WARNING_START_MSVC(...) \
            __pragma(warning(push)) \
            __pragma(warning(disable:__VA_ARGS__))
#   define DT_DISABLE_WARNING_ALL_START \
            __pragma(warning(push,0))
#   define DT_DISABLE_WARNING_END \
            __pragma(warning(pop))
__pragma(warning(disable:4251))
#elif defined __clang__
#   define _STR_(X) #X
// Because MSVC warning codes
#   define DT_DISABLE_WARNING_START_MSVC(...) \
        _Pragma("clang diagnostic push")
#   define DT_DISABLE_WARNING_START_CLANG(...) \
        _Pragma("clang diagnostic push")\
        _Pragma(_STR_(GCC diagnostic ignored __VA_ARGS__ ))
#   define DT_DISABLE_WARNING_ALL_START \
        _Pragma("clang diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wall\"") \
        _Pragma("GCC diagnostic ignored \"-Wignored-qualifiers\"")
         _Pragma("GCC diagnostic ignored \"-Winconsistent-missing-override\"")
#   define DT_DISABLE_WARNING_END \
        _Pragma("clang diagnostic pop")
// warning is triggered on things that are not a problem.
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wdeprecated-register"
#else
#   define DT_DISABLE_WARNING_START_MSVC(...)
#   define DT_DISABLE_WARNING_START_CLANG(...)
#   define DT_DISABLE_WARNING_ALL_START
#   define DT_DISABLE_WARNING_END
#endif



#endif /* WARNINGDISABLE_H_ */
