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

#ifndef RTI13HANDLE_H_
#define RTI13HANDLE_H_

#include <dtHLAGM/rtihandle.h>

#ifndef RTI_USES_STD_FSTREAM
#define RTI_USES_STD_FSTREAM
#endif
#include <RTI.hh>
#include <dtUtil/mswinmacros.h>

namespace dtHLAGM
{
   template <typename InternalHandle>
   class RTI13Handle : public RTIHandle
   {
   public:
      typedef RTI13Handle<InternalHandle> ThisType;
      RTI13Handle(InternalHandle handle) { mHandle = handle; }

      InternalHandle GetRTI13Handle() const { return mHandle; }
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
      virtual ~RTI13Handle() {}
   private:
      InternalHandle mHandle;
   };

   typedef RTI13Handle<RTI::ObjectClassHandle> RTI13ObjectClassHandle;
   typedef RTI13Handle<RTI::ObjectHandle> RTI13ObjectInstanceHandle;
   typedef RTI13Handle<RTI::AttributeHandle> RTI13AttributeHandle;
   typedef RTI13Handle<RTI::InteractionClassHandle> RTI13InteractionClassHandle;
   typedef RTI13Handle<RTI::ParameterHandle> RTI13ParameterHandle;
   typedef RTI13Handle<RTI::DimensionHandle> RTI13DimensionHandle;
}


#endif /* RTI13HANDLE_H_ */
