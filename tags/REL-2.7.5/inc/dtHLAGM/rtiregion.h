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

#ifndef RTIRegion_H_
#define RTIRegion_H_

#include <dtHLAGM/export.h>
#include <osg/Referenced>

namespace dtHLAGM
{
   /*
    *  This wraps an HLA region to hide different RTI api specs
    */
   class DT_HLAGM_EXPORT RTIRegion : public osg::Referenced
   {
   public:
      RTIRegion();

   protected:
      virtual ~RTIRegion();
   };
}
#endif /* RTIRegion_H_ */
