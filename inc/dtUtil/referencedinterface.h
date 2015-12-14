/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014 David Guthrie
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
#ifndef REFERENCED_INTERFACE
#define REFERENCED_INTERFACE

#include <osg/Observer>

namespace dtUtil
{
   class ReferencedInterface
   {
   protected:
      virtual ~ReferencedInterface() {}
   public:
      ReferencedInterface(){}
      virtual int ref() = 0;
      virtual int unref() = 0;
      virtual int unref_nodelete() = 0;
      virtual osg::ObserverSet* getOrCreateObserverSet() const = 0;
   private:
      // Hide copying
      ReferencedInterface(ReferencedInterface&) {}
      ReferencedInterface& operator=(ReferencedInterface&) {return *this;}
   };
}

///////////
/*
 * Declare this in any class that implements and Referenced interface so that you can map the functions
 * to the ones of referenced
 */
#if 0
#define DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE \
   using osg::Referenced::ref;\
   using osg::Referenced::unref;\
   using osg::Referenced::unref_nodelete;\
   using osg::Referenced::getOrCreateObserverSet;
#endif

#define DT_DECLARE_VIRTUAL_REF_INTERFACE_OVERRIDE_INLINE \
      int ref() override { return osg::Referenced::ref(); }\
      int unref() override { return osg::Referenced::unref(); }\
      int unref_nodelete() override { return osg::Referenced::unref_nodelete(); }\
      osg::ObserverSet* getOrCreateObserverSet() const override { return osg::Referenced::getOrCreateObserverSet(); }

#define DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE \
      virtual int ref() { return osg::Referenced::ref(); }\
      virtual int unref() { return osg::Referenced::unref(); }\
      virtual int unref_nodelete() { return osg::Referenced::unref_nodelete(); }\
      virtual osg::ObserverSet* getOrCreateObserverSet() const { return osg::Referenced::getOrCreateObserverSet(); }

#endif //REFERENCED_INTERFACE
