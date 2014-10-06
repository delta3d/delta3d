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
#include "rti13util.h"
#include "rti13handle.h"
#include <dtHLAGM/rtiexception.h>
#include <sstream>

namespace dtHLAGM
{
   RTI::AttributeHandleSet* ConvertAHStoRTI13AHS(const RTIAttributeHandleSet& dtHandleSet)
   {
      RTI::AttributeHandleSet* ahsResult = NULL;
      try
      {
         ahsResult = RTI::AttributeHandleSetFactory::create(dtHandleSet.size());
      }
      catch (const RTI::Exception& ex)
      {
         RethrowRTIException(ex);
      }

      RTIAttributeHandleSet::const_iterator i, iend;
      i = dtHandleSet.begin();
      iend = dtHandleSet.end();
      for (; i != iend; ++i)
      {
         ahsResult->add(static_cast<RTI13AttributeHandle*>(i->get())->GetRTI13Handle());
      }

      return ahsResult;
   }

   RTI::AttributeHandleValuePairSet* ConvertAHVMToRTI13AHVS(const RTIAttributeHandleValueMap& dtHandleValueMap)
   {
      RTI::AttributeHandleValuePairSet* result = NULL;
      try
      {
         result = RTI::AttributeSetFactory::create(dtHandleValueMap.size());
      }
      catch (const RTI::Exception& ex)
      {
         RethrowRTIException(ex);
      }

      RTIAttributeHandleValueMap::const_iterator i, iend;
      i = dtHandleValueMap.begin();
      iend = dtHandleValueMap.end();
      for (; i != iend; ++i)
      {
         result->add(static_cast<RTI13AttributeHandle*>(
                  i->first.get())->GetRTI13Handle(),
                  i->second.mData.c_str(),
                  RTI::ULong(i->second.mData.length()));
      }

      return result;
   }

   RTI::ParameterHandleValuePairSet* ConvertPHVMToRTI13PHVS(const RTIParameterHandleValueMap& dtHandleValueMap)
   {
      RTI::ParameterHandleValuePairSet* result = NULL;
      try
      {
         result = RTI::ParameterSetFactory::create(dtHandleValueMap.size());
      }
      catch (const RTI::Exception& ex)
      {
         RethrowRTIException(ex);
      }

      RTIParameterHandleValueMap::const_iterator i, iend;
      i = dtHandleValueMap.begin();
      iend = dtHandleValueMap.end();
      for (; i != iend; ++i)
      {
         result->add(static_cast<RTI13ParameterHandle*>(
                  i->first.get())->GetRTI13Handle(),
                  i->second.mData.c_str(),
                  RTI::ULong(i->second.mData.length()));
      }

      return result;
   }
}
