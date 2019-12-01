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

#ifndef RTI1516EUTIL_H_
#define RTI1516EUTIL_H_

#include <string>
#include <dtHLAGM/rticontainers.h>
#include <dtHLAGM/rtiexception.h>

#include <RTI/RTI1516.h>

// This is a macro instead of a function so that the exception line numbers will work out.
#define RethrowRTI1516eException(ex) \
   {\
      /*std::wostringstream ss;*/\
      /*ss << ex;*/\
      /* must convert to a narrow string */ \
      std::string ns(dtHLAGM::WideToNarrow(ex.what()));\
      throw RTIException(ns, __FILE__, __LINE__);\
   }


namespace dtHLAGM
{
   std::wstring NarrowToWide(const std::string& s);
   std::string WideToNarrow(const std::wstring& ws);

   void ConvertAHStoRTI1516eAHS(const RTIAttributeHandleSet& dtHandleSet, rti1516e::AttributeHandleSet& toFill);

   void ConvertAHVMToRTI1516eAHVM(const RTIAttributeHandleValueMap& dtHandleValueMap, rti1516e::AttributeHandleValueMap& toFill);

   void ConvertPHVMToRTI1516ePHVM(const RTIParameterHandleValueMap& dtHandleValueMap, rti1516e::ParameterHandleValueMap& toFill);

   template<typename rtiHandleType, typename dtHandleType, typename dtCreateType>
   dtCore::RefPtr<dtHandleType> WrapHandle(rtiHandleType handle, std::map<rtiHandleType, dtCore::RefPtr<dtHandleType> >& cache)
   {
      if (!handle.isValid())
      {
         throw RTIException("Invalid handle returned by RTI.", __FILE__, __LINE__);
      }

      dtCore::RefPtr<dtHandleType> wrappedHandle = cache[handle];
      if (!wrappedHandle.valid())
      {
         wrappedHandle = new dtCreateType(handle);
         cache[handle] = wrappedHandle;
      }
      return wrappedHandle;
   }

}


#endif /* RTI1516EUTIL_H_ */
