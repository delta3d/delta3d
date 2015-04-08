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

#include "rti1516eutil.h"
#include "rti1516ehandle.h"

namespace dtHLAGM
{
   std::wstring NarrowToWide(const std::string& s)
   {
      setlocale(LC_CTYPE, "");
      std::wstring result(s.begin(), s.end());
      return result;
   }

   std::string WideToNarrow(const std::wstring& ws)
   {
      setlocale(LC_CTYPE, "");
      std::string result(ws.begin(), ws.end());
      return result;
   }

   void ConvertAHStoRTI1516eAHS(const RTIAttributeHandleSet& dtHandleSet, rti1516e::AttributeHandleSet& toFill)
   {
      RTIAttributeHandleSet::const_iterator i, iend;
      i = dtHandleSet.begin();
      iend = dtHandleSet.end();
      for (; i != iend; ++i)
      {
         toFill.insert(static_cast<RTI1516eAttributeHandle*>(i->get())->GetRTI1516eHandle());
      }
   }

   void ConvertAHVMToRTI1516eAHVM(const RTIAttributeHandleValueMap& dtHandleValueMap, rti1516e::AttributeHandleValueMap& toFill)
   {
      RTIAttributeHandleValueMap::const_iterator i, iend;
      i = dtHandleValueMap.begin();
      iend = dtHandleValueMap.end();
      for (; i != iend; ++i)
      {
         rti1516e::VariableLengthData rtiData(i->second.mData.c_str(), i->second.mData.size());
         toFill.insert(std::make_pair(static_cast<const RTI1516eAttributeHandle*>(i->first.get())->GetRTI1516eHandle(), rtiData));
      }

   }

   void ConvertPHVMToRTI1516ePHVM(const RTIParameterHandleValueMap& dtHandleValueMap, rti1516e::ParameterHandleValueMap& toFill)
   {
      RTIParameterHandleValueMap::const_iterator i, iend;
      i = dtHandleValueMap.begin();
      iend = dtHandleValueMap.end();
      for (; i != iend; ++i)
      {
         rti1516e::VariableLengthData rtiData(i->second.mData.c_str(), i->second.mData.size());
         toFill.insert(std::make_pair(static_cast<const RTI1516eParameterHandle*>(i->first.get())->GetRTI1516eHandle(), rtiData));
      }
   }
}
