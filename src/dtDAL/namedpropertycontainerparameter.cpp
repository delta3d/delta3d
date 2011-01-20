/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include <prefix/dtdalprefix.h>
#include <dtDAL/namedpropertycontainerparameter.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/propertycontaineractorproperty.h>

namespace dtDAL
{

   ///////////////////////////////////////////////////////////////////////////////
   NamedPropertyContainerParameter::NamedPropertyContainerParameter(const dtUtil::RefString& name)
   : NamedGroupParameter(dtDAL::DataType::PROPERTY_CONTAINER, name)
   {}

   ///////////////////////////////////////////////////////////////////////////////
   NamedPropertyContainerParameter::NamedPropertyContainerParameter(dtDAL::DataType& newDataType, const dtUtil::RefString& name)
   : NamedGroupParameter(newDataType, name)
   {}

   ///////////////////////////////////////////////////////////////////////////////
   NamedPropertyContainerParameter::NamedPropertyContainerParameter(const NamedPropertyContainerParameter& toCopy)
   :  NamedGroupParameter(dtDAL::DataType::PROPERTY_CONTAINER, toCopy.GetName())
   {
      CopyFrom(toCopy);
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedPropertyContainerParameter::~NamedPropertyContainerParameter()
   {}

   ///////////////////////////////////////////////////////////////////////////////
   void NamedPropertyContainerParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      RemoveAllParameters();

      const dtDAL::BasePropertyContainerActorProperty* pcap = dynamic_cast<const dtDAL::BasePropertyContainerActorProperty*>(&property);
      if (pcap != NULL)
      {
         dtDAL::PropertyContainer* pc = pcap->GetValue();
         if (pc != NULL)
         {
            std::vector<const ActorProperty*> propList;
            pc->GetPropertyList(propList);
            std::vector<const ActorProperty*>::iterator i, iend;
            i = propList.begin();
            iend = propList.end();
            for (; i != iend; ++i)
            {
               const ActorProperty* ap = *i;
               NamedParameter* np = AddParameter(ap->GetName(), ap->GetDataType(), false);
               np->SetFromProperty(*ap);
            }
         }
      }
   }

   struct ApplyParameterFunc
   {
      ApplyParameterFunc(dtDAL::PropertyContainer& pc)
      : mPC(pc)
      {
      }

      void operator() (const dtCore::RefPtr<NamedParameter>& np)
      {
         dtUtil::RefString paramName = np->GetName();

         dtCore::RefPtr<dtDAL::ActorProperty> property = mPC.GetProperty(paramName);
         if (!property.valid())
         {
            property = mPC.GetDeprecatedProperty(paramName);
         }

         if (!property.valid())
         {
            LOG_WARNING("Property \"" + paramName +
               "\" was not found on property container \"");
            return;
         }

         //can't set a read-only property.
         if (property->IsReadOnly())
         {
            return;
         }


         // We specifically don't support the special case of an ActorActorProperty here.  They are sort of deprecated anyway.
         try
         {
            np->ApplyValueToProperty(*property);
         }
         catch (const dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR);
         }
      }

      dtDAL::PropertyContainer& mPC;
   };

   ///////////////////////////////////////////////////////////////////////////////
   void NamedPropertyContainerParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::BasePropertyContainerActorProperty* pcap = dynamic_cast<dtDAL::BasePropertyContainerActorProperty*>(&property);
      if (pcap != NULL)
      {
         dtDAL::PropertyContainer* pc = pcap->GetValue();
         if (pc == NULL && GetParameterCount() > 0U)
         {
            // TODO find some way to specify the type to create.
            pcap->CreateNew();
            pc = pcap->GetValue();
         }

         if (pc != NULL)
         {
            ApplyParameterFunc func(*pc);
            ForEachParameter(func);
         }
      }
   }

}
