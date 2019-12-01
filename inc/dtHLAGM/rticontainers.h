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

#ifndef RTICONTAINERS_H_
#define RTICONTAINERS_H_

#include <map>
#include <vector>
#include <dtCore/refptr.h>
#include <dtHLAGM/rtihandle.h>
#include <dtHLAGM/rtiregion.h>

namespace dtHLAGM
{
   typedef std::set<dtCore::RefPtr<RTIAttributeHandle> > RTIAttributeHandleSet;
   typedef std::set<dtCore::RefPtr<RTIParameterHandle> > RTIParameterHandleSet;

   typedef std::set<dtCore::RefPtr<RTIDimensionHandle> > RTIDimensionHandleSet;

   struct RTIContainerValueData
   {
      std::string mData;
      dtCore::RefPtr< RTIRegion > mRegion;
   };

   typedef std::map<dtCore::RefPtr<RTIAttributeHandle>, RTIContainerValueData> RTIAttributeHandleValueMap;
   typedef std::map<dtCore::RefPtr<RTIParameterHandle>, RTIContainerValueData> RTIParameterHandleValueMap;

   struct RTIDimensionData
   {
      dtCore::RefPtr<RTIDimensionHandle> mDimHandle;
      // this accomodates rti 1.3, though really it needs to be different for 1516.
      // Apparently algorthms have been written to convert 1.3DDM to 1516, that could be used internally
      // though it would probably be better to configure a 1516 DDM scheme in a 1516 way.  In this case
      // mMin would always be 0.
      // It uses unsigned int not long because the rti 1.3 actually wants it to be 32 bit.
      unsigned int mMin, mMax;
   };

   typedef std::vector<RTIDimensionData> RTIDimensionVector;

}

#endif /* RTICONTAINERS_H_ */
