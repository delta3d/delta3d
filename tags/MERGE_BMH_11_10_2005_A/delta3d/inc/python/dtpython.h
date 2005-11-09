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

// dtpython.h: Private header file for bindings.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#ifndef WIN32
#include <ode/src/collision_kernel.h>
#include <ode/src/objects.h>
#endif

#include <dtCore/deltadrawable.h>
#include <dtCore/refptr.h>
#include <osg/ref_ptr>

namespace boost
{
   namespace python
   {
      template <typename T>
      struct pointee< osg::ref_ptr<T> >
      {
         typedef T type;
      };
      
      template <class T> T const* get_pointer( dtCore::RefPtr<T> const& p)
      {
         return p.get();
      }
      template <class T> T const* get_pointer( osg::ref_ptr<T> const& p)
      {
         return p.get(); 
      }
      template <class T> T* get_pointer( dtCore::RefPtr<T>& p)
      {
         return p.get();
      }
      template <class T> T* get_pointer( osg::ref_ptr<T>& p)
      {
         return p.get(); 
      }
   }
}
