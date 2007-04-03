/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_ACKNOWLEDGE_CONSTANTS_H__
#define __DELTA_DTDIS_ACKNOWLEDGE_CONSTANTS_H__

namespace dtDIS
{
   enum AcknowledgeFlag
   {
      ACKNOWLEDGE_CREATE_ENTITY = 1,
      ACKNOWLEDGE_REMOVE_ENTITY = 2,
      ACKNOWLEDGE_START_RESUME = 3,
      ACKNOWLEDGE_STOP_FREEZE = 4
   };

   enum ResponseFlag
   {
      RESPONSE_OTHER = 0,
      RESPONSE_ABLE_TO_COMPLY = 1,
      RESPONSE_UNABLE_TO_COMPLY = 2,
   };
}

#endif // __DELTA_DTDIS_ACKNOWLEDGE_CONSTANTS_H__
