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
 */

#ifndef DELTA_REF_PTR
#define DELTA_REF_PTR

// Delta3D RefPtr, a wrap of the OSG RefPtr but in the dtCore namespace.
//
//////////////////////////////////////////////////////////////////////

#include <osg/ref_ptr>
#include <ostream>
// Had to add this to disable a few warnings on clang.
#include <dtUtil/warningdisable.h>

namespace dtCore
{

   // Smart pointer for handling referenced counted objects.
   template<class T>
   class RefPtr : public osg::ref_ptr<T>
   {
   public:
      RefPtr(): osg::ref_ptr<T>() {}
      RefPtr(T* t): osg::ref_ptr<T>(t) {}
      RefPtr(const RefPtr& rp):osg::ref_ptr<T>(rp) {}

      // added specifically for Delta3D
      template <typename TSub>
      RefPtr(const osg::ref_ptr<TSub>& rp):osg::ref_ptr<T>(rp.get()) {}

      // added specifically for Delta3D
      friend inline std::ostream& operator<<(std::ostream& os, const RefPtr& rp)
      {
         os << rp.get();
         return os;
      }

      // added specifically for Delta3D
      template <typename TSub>
      RefPtr& operator = (const osg::ref_ptr<TSub>& rp)
      {
         *this = rp.get();
         return *this;
      }
   };

   template <class T>
   class ConvertToPointerUnary
   {
   public:
      T* operator()(dtCore::RefPtr<T>& ptr) const
      {
         return ptr.get();
      }

      const T* operator()(const dtCore::RefPtr<T>& ptr) const
      {
         return ptr.get();
      }
   };

} // namespace dtCore

#endif // DELTA_REF_PTR
