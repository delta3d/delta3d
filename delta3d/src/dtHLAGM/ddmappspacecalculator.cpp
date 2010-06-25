/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation.
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
#include <dtHLAGM/ddmappspacecalculator.h>

#include <dtDAL/datatype.h>
#include <dtDAL/longactorproperty.h>

#include <dtHLAGM/ddmutil.h>

namespace dtHLAGM
{
   const std::string DDMAppSpaceCalculator::PROP_APP_SPACE_MIN("App Space Min");
   const std::string DDMAppSpaceCalculator::PROP_APP_SPACE_MAX("App Space Max");

   const std::string DDMAppSpaceCalculator::PROP_DEFAULT_APP_SPACE("Default App Space");


   DDMAppSpaceCalculator::DDMAppSpaceCalculator():
      mAppSpaceMin(0),
      mAppSpaceMax(30),
      mDefaultAppSpace(0)
   {
      AddProperty(new dtDAL::LongActorProperty(PROP_APP_SPACE_MIN, PROP_APP_SPACE_MIN,
               dtDAL::LongActorProperty::SetFuncType(this, &DDMAppSpaceCalculator::SetAppSpaceMinimum),
               dtDAL::LongActorProperty::GetFuncType(this, &DDMAppSpaceCalculator::GetAppSpaceMinimum)
            ));
      AddProperty(new dtDAL::LongActorProperty(PROP_APP_SPACE_MAX, PROP_APP_SPACE_MAX,
               dtDAL::LongActorProperty::SetFuncType(this, &DDMAppSpaceCalculator::SetAppSpaceMaximum),
               dtDAL::LongActorProperty::GetFuncType(this, &DDMAppSpaceCalculator::GetAppSpaceMaximum)
            ));

      AddProperty(new dtDAL::LongActorProperty(PROP_DEFAULT_APP_SPACE, PROP_DEFAULT_APP_SPACE,
               dtDAL::LongActorProperty::SetFuncType(this, &DDMAppSpaceCalculator::SetDefaultAppSpace),
               dtDAL::LongActorProperty::GetFuncType(this, &DDMAppSpaceCalculator::GetDefaultAppSpace)
            ));
   }

   DDMAppSpaceCalculator::~DDMAppSpaceCalculator()
   {
   }

   unsigned long DDMAppSpaceCalculator::MapAppSpaceValue(unsigned spaceNumber) const
   {
      return DDMUtil::MapEnumerated(spaceNumber, mAppSpaceMin, mAppSpaceMax);
   }

}
