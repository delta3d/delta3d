/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2012, MOVES Institute
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
 * Jeff P. Houde
 */

#ifndef CONTAINER_SELECTOR_ACTOR_PROPERTY
#define CONTAINER_SELECTOR_ACTOR_PROPERTY

#include <dtCore/export.h>
#include <dtCore/genericactorproperty.h>
#include <dtCore/observerptr.h>

namespace dtCore
{
   class PropertyContainer;

   ////////////////////////////////////////////////////////////////////////////
   /**
    * This actor property represents a string data member that can be selected
    * by a drop list of pre-defined strings.  Each string represents a property
    * container who's properties are then extracted and displayed as editable
    * properties within this selector.
    */
   ////////////////////////////////////////////////////////////////////////////
   class DT_CORE_EXPORT ContainerSelectorActorProperty : public GenericActorProperty<const std::string&, std::string>
   {
      public:
         typedef GenericActorProperty<const std::string&, std::string> BaseClass;
         typedef BaseClass::SetFuncType SetFuncType;
         typedef BaseClass::GetFuncType GetFuncType;
         typedef dtUtil::Functor<std::vector<std::string>, TYPELIST_0()> GetListFuncType;
         typedef dtUtil::Functor<dtCore::PropertyContainer*, TYPELIST_0()> GetContainerFuncType;

         ContainerSelectorActorProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           SetFuncType set, GetFuncType get,
                           GetListFuncType getList,
                           GetContainerFuncType getContainer,
                           const dtUtil::RefString& desc = "",
                           const dtUtil::RefString& groupName = "");

         /**
          *	Remembered Property index used during save/load.
          */
         void SetCurrentPropertyIndex(int index);
         int GetCurrentPropertyIndex() const;

         /**
          * Returns the available strings to select from.
          */
         std::vector<std::string> GetList() const;

         /**
          *	Returns the container that is currently selected.
          */
         dtCore::PropertyContainer* GetContainer() const;

         /**
          * Does the same thing as SetValue.
          * @note Returns false it the property is read only
          * @param value the value to set.
          * @return true
          */
         virtual bool FromString(const std::string& value);

         /**
          * @return the same as GetValue.
          */
         virtual const std::string ToString() const;

   protected:
         virtual ~ContainerSelectorActorProperty() { }

         int                  mCurrentPropertyIndex;

         GetListFuncType      mGetListFunc;
         GetContainerFuncType mGetContainerFunc;
      };
}

#endif //STRING_SELECTOR_ACTOR_PROPERTY
