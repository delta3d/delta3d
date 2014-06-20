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
      virtual void ref() = 0;
      virtual void unref() = 0;
      virtual void unref_nodelete() = 0;
      virtual osg::ObserverSet* getOrCreateObserverSet() = 0;
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
#define DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE \
      virtual void ref() { osg::Referenced::ref(); }\
      virtual void unref() { osg::Referenced::unref(); }\
      virtual void unref_nodelete() { osg::Referenced::unref_nodelete(); }\
      virtual osg::ObserverSet* getOrCreateObserverSet() { return osg::Referenced::getOrCreateObserverSet(); }

#endif //REFERENCED_INTERFACE
