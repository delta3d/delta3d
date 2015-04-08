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

#include <dtCore/datatype.h>
#include <dtCore/longactorproperty.h>

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
      AddProperty(new dtCore::LongActorProperty(PROP_APP_SPACE_MIN, PROP_APP_SPACE_MIN,
               dtCore::LongActorProperty::SetFuncType(this, &DDMAppSpaceCalculator::SetAppSpaceMinimum),
               dtCore::LongActorProperty::GetFuncType(this, &DDMAppSpaceCalculator::GetAppSpaceMinimum)
            ));
      AddProperty(new dtCore::LongActorProperty(PROP_APP_SPACE_MAX, PROP_APP_SPACE_MAX,
               dtCore::LongActorProperty::SetFuncType(this, &DDMAppSpaceCalculator::SetAppSpaceMaximum),
               dtCore::LongActorProperty::GetFuncType(this, &DDMAppSpaceCalculator::GetAppSpaceMaximum)
            ));

      AddProperty(new dtCore::LongActorProperty(PROP_DEFAULT_APP_SPACE, PROP_DEFAULT_APP_SPACE,
               dtCore::LongActorProperty::SetFuncType(this, &DDMAppSpaceCalculator::SetDefaultAppSpace),
               dtCore::LongActorProperty::GetFuncType(this, &DDMAppSpaceCalculator::GetDefaultAppSpace)
            ));
   }

   DDMAppSpaceCalculator::~DDMAppSpaceCalculator()
   {
   }

   unsigned int DDMAppSpaceCalculator::MapAppSpaceValue(unsigned spaceNumber) const
   {
      return DDMUtil::MapEnumerated(spaceNumber, mAppSpaceMin, mAppSpaceMax);
   }

}
