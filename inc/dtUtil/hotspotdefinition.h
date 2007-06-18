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
* John Grant
*/

#ifndef __DTUTIL_HOT_SPOT_DEFINITION_H__
#define __DTUTIL_HOT_SPOT_DEFINITION_H__

#include <string>
#include <osg/Vec3>
#include <osg/Quat>
#include <dtUtil/export.h>

namespace dtUtil
{
   struct DT_UTIL_EXPORT HotSpotDefinition
   {
      std::string mName;                  ///< an identifier for this "hot spot" instance
      std::string mParentName;            ///< a coordinate frame identifier for the parent system.
      osg::Vec3 mLocalTranslation;        ///< the desired position of the HotSpot in the parent frame.
      osg::Quat mLocalRotation;           ///< the desired orientation of the HotSpot in the parent frame.
   };
}

#endif // __DTUTIL_HOT_SPOT_DEFINITION_H__
