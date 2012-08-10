/* -*-c++-*-
 * Delta3D
 * Copyright 2012, MASA Group Inc.
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

#ifndef RTIHANDLE_H_
#define RTIHANDLE_H_

#include <dtHLAGM/export.h>
#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <set>

namespace dtHLAGM
{
   class DT_HLAGM_EXPORT RTIHandle : public osg::Referenced
   {
   public:
      virtual bool operator==(RTIHandle&) = 0;
   protected:
      RTIHandle();
      virtual ~RTIHandle();
   };

   typedef RTIHandle RTIObjectClassHandle;
   typedef RTIHandle RTIObjectInstanceHandle;
   typedef RTIHandle RTIAttributeHandle;
   typedef RTIHandle RTIInteractionClassHandle;
   //typedef RTIHandle RTIInteractionInstanceHandle;
   typedef RTIHandle RTIParameterHandle;

   typedef RTIHandle RTIDimensionHandle;

}


#endif /* RTIHANDLE_H_ */
