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
 * FOR A PARTI13CULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * David Guthrie
 */

#include "rti13region.h"

namespace dtHLAGM
{

RTI13Region::RTI13Region(RTI::Region& region)
: mRegion(region)
{
}

RTI13Region::~RTI13Region()
{
   // The delete region call on the rti handles this, it seems.
   //delete &mRegion;
}

RTI::Region& RTI13Region::GetRTI13Region()
{
   return mRegion;
}

}
