/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, Alion Science and Technology.
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

#include <dtUtil/typetraits.h>
#include <dtCore/typetoactorproperty.h>

namespace dtCore
{
   template <typename T>
   inline void NamedGroupParameter::AddValue(const dtUtil::RefString& name, const T& value)
   {
      AddParameter(*new typename TypeToActorProperty<T>::named_parameter_type(name, value));
   }

   template <typename T>
   inline void NamedGroupParameter::SetValue(const dtUtil::RefString& name, const T& value)
   {
      typename TypeToActorProperty<T>::named_parameter_type* param = NULL;
      GetParameter(name, param);
      if (param == NULL)
      {
         AddValue(name, value);
      }
      else
      {
         param->SetValue(value);
      }
   }

   template <typename T>
   inline const T& NamedGroupParameter::GetValue(const dtUtil::RefString& name, const T& defaultVal) const
   {
      const typename TypeToActorProperty<T>::named_parameter_type* param = NULL;
      GetParameter(name, param);
      if (param == NULL)
      {
         return defaultVal;
      }
      return param->GetValue();
   }
}
