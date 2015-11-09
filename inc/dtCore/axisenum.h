/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015 Caper Holdings, LLC
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

#ifndef INC_DTCORE_AXISENUM_H_
#define INC_DTCORE_AXISENUM_H_

#include <dtUtil/enumeration.h>
#include <dtCore/export.h>
#include <osg/Vec3>

namespace dtCore
{

   /**
    * A type safe enumeration representing an axis.
    */
   class DT_CORE_EXPORT AxisEnum : public dtUtil::Enumeration
   {
   public:
      DECLARE_ENUM(AxisEnum)

      static AxisEnum AXIS_X;
      static AxisEnum AXIS_Y;
      static AxisEnum AXIS_Z;

      unsigned GetIndex() const;
      const osg::Vec3& AsVector() const;
   protected:
      AxisEnum(const std::string& name, const osg::Vec3& vec, unsigned idx);
      virtual ~AxisEnum();
   private:
      osg::Vec3 mVector;
      unsigned mIndex;
   };

} /* namespace dtCore */

#endif /* INC_DTCORE_AXISENUM_H_ */
