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

#ifndef RTI13UTIL_H_
#define RTI13UTIL_H_

#include <dtHLAGM/rticontainers.h>

#ifndef RTI_USES_STD_FSTREAM
#define RTI_USES_STD_FSTREAM
#endif
#include <RTI.hh>

// This is a macro instead of a function so that the exception line numbers will work out.
#define RethrowRTIException(ex) \
   {\
      std::ostringstream ss;\
      ::operator<<(ss, ex);\
      throw RTIException(ss.str(), __FILE__, __LINE__);\
   }


namespace dtHLAGM
{
   RTI::AttributeHandleSet* ConvertAHStoRTI13AHS(const RTIAttributeHandleSet& dtHandleSet);

   RTI::AttributeHandleValuePairSet* ConvertAHVMToRTI13AHVS(const RTIAttributeHandleValueMap& dtHandleValueMap);

   RTI::ParameterHandleValuePairSet* ConvertPHVMToRTI13PHVS(const RTIParameterHandleValueMap& dtHandleValueMap);

   template<typename rtiHandleType, typename dtHandleType, typename dtCreateType>
   dtCore::RefPtr<dtHandleType> WrapHandle(rtiHandleType handle, std::map<rtiHandleType, dtCore::RefPtr<dtHandleType> >&  cache)
   {
      dtCore::RefPtr<dtHandleType> wrappedHandle = cache[handle];
      if (!wrappedHandle.valid())
      {
         wrappedHandle = new dtCreateType(handle);
         cache[handle] = wrappedHandle;
      }
      return wrappedHandle;
   }

}


#endif /* RTI13UTIL_H_ */
