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
 * FOR A PARTI1516eCULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1516e07 USA
 *
 * David Guthrie
 */

#ifndef RTI1516eRegion_H_
#define RTI1516eRegion_H_

#include <dtHLAGM/rtiregion.h>
#include <RTI/RTI1516.h>

namespace dtHLAGM
{
   /*
    *  This wraps an HLA region to hide different RTI api specs
    */
   class RTI1516eRegion : public RTIRegion
   {
   public:
      RTI1516eRegion(rti1516e::RegionHandle& region);
      rti1516e::RegionHandle& GetRTI1516eRegion();

      virtual bool operator==(RTIRegion& h)
      {
         RTI1516eRegion* castH = dynamic_cast<RTI1516eRegion*>(&h);
         if (castH == NULL)
         {
            return false;
         }
         return castH->mRegion == mRegion;
      }
   protected:
      virtual ~RTI1516eRegion();
   private:
      rti1516e::RegionHandle mRegion;
   };
}
#endif /* RTI1516eRegion_H_ */
