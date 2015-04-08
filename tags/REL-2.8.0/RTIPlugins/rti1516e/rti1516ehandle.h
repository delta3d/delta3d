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

#ifndef RTI1516eHANDLE_H_
#define RTI1516eHANDLE_H_

#include <dtHLAGM/rtihandle.h>

#include <RTI/RTI1516.h>

namespace dtHLAGM
{
   template <typename InternalHandle>
   class RTI1516eHandle : public RTIHandle
   {
   public:
      typedef RTI1516eHandle<InternalHandle> ThisType;
      RTI1516eHandle(InternalHandle handle) { mHandle = handle; }

      InternalHandle GetRTI1516eHandle() const { return mHandle; }

      virtual bool operator==(RTIHandle& h)
      {
         ThisType* castH = dynamic_cast<ThisType*>(&h);
         if (castH == NULL)
         {
            return false;
         }
         return castH->mHandle == mHandle;
      }
   protected:
      virtual ~RTI1516eHandle() {}
   private:
      InternalHandle mHandle;
   };

   typedef RTI1516eHandle<rti1516e::ObjectClassHandle> RTI1516eObjectClassHandle;
   typedef RTI1516eHandle<rti1516e::ObjectInstanceHandle> RTI1516eObjectInstanceHandle;
   typedef RTI1516eHandle<rti1516e::AttributeHandle> RTI1516eAttributeHandle;
   typedef RTI1516eHandle<rti1516e::InteractionClassHandle> RTI1516eInteractionClassHandle;
   typedef RTI1516eHandle<rti1516e::ParameterHandle> RTI1516eParameterHandle;
   typedef RTI1516eHandle<rti1516e::DimensionHandle> RTI1516eDimensionHandle;

}


#endif /* RTI1516eHANDLE_H_ */
