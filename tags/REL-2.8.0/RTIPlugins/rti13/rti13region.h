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

#ifndef RTI13Region_H_
#define RTI13Region_H_

#include <dtHLAGM/rtiregion.h>

#ifndef RTI_USES_STD_FSTREAM
#define RTI_USES_STD_FSTREAM
#endif
#include <RTI.hh>

namespace dtHLAGM
{
   /*
    *  This wraps an HLA region to hide different RTI api specs
    */
   class RTI13Region : public RTIRegion
   {
   public:
      RTI13Region(RTI::Region& region);
      RTI::Region& GetRTI13Region();

      virtual bool operator==(RTIRegion& h)
      {
         RTI13Region* castH = dynamic_cast<RTI13Region*>(&h);
         if (castH == NULL)
         {
            return false;
         }
         return &castH->mRegion == &mRegion;
      }
   protected:
      virtual ~RTI13Region();
   private:
      RTI::Region& mRegion;
   };
}
#endif /* RTI13Region_H_ */
