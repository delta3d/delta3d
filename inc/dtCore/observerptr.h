/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
*
* This library is open source and may be redistributed and/or modified under
* the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
* (at your option) any later version.  The full license is in LICENSE file
* included with this distribution, and on the openscenegraph.org website.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* OpenSceneGraph Public License for more details.
*/

#ifndef DELTA_OBSERVER_PTR
#define DELTA_OBSERVER_PTR

#include <osg/observer_ptr>

// Delta3D ObserverPtr, a wrap of the OSG observer_ptr but in the dtCore namespace.
//
//////////////////////////////////////////////////////////////////////

namespace dtCore
{
   template <class T>
   class ObserverPtr: public osg::observer_ptr<T>
   {
      public:
         ObserverPtr(): osg::observer_ptr<T>() {}

         ObserverPtr(T* t): osg::observer_ptr<T>(t) {}

         ObserverPtr(const osg::observer_ptr<T>& rp):
            osg::observer_ptr<T>(rp) {}
   };
}

#endif
