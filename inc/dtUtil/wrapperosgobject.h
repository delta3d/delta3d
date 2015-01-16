/* -*-c++-*-
 * Delta3D
 * Copyright 2011, Alion Science and Technology
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
 * 
 * David Guthrie
 */

#ifndef WRAPPEROSGOBJECT_H_
#define WRAPPEROSGOBJECT_H_

#include <osg/Object>

namespace dtUtil
{

   /**
    * This simple object subclass exists for attaching rider referenced objects (as user data) so they can
    * be passed through readObject and writeObject is osgDB.
    */
   class WrapperOSGObject : public osg::Object
   {
   public:
      WrapperOSGObject() : osg::Object() {}
      explicit WrapperOSGObject(bool threadSafeRefUnref) : osg::Object(threadSafeRefUnref) {}

      WrapperOSGObject(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
      : osg::Object(obj, copyop)
      {}

      META_Object("dtCore", WrapperOSGObject);
   };
}

#endif /* WRAPPEROSGOBJECT_H_ */
