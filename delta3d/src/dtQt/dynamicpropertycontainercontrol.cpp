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

#include <prefix/dtqtprefix.h>
#include <dtQt/dynamicpropertycontainercontrol.h>

#include <dtCore/datatype.h>
#include <dtUtil/log.h>

namespace dtQt
{
   DynamicPropertyContainerControl::DynamicPropertyContainerControl()
   {
   }

   DynamicPropertyContainerControl::~DynamicPropertyContainerControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicPropertyContainerControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::PROPERTY_CONTAINER)
      {
         mProperty = static_cast<dtCore::BasePropertyContainerActorProperty*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);

         dtCore::PropertyContainer* pc = mProperty->GetValue();
         if (pc != NULL)
         {
            std::vector<dtCore::ActorProperty*> properties;
            pc->GetPropertyList(properties);
            std::vector<dtCore::ActorProperty*>::iterator i, iend;
            i = properties.begin();
            iend = properties.end();
            for (; i != iend ; ++i)
            {
               dtCore::ActorProperty* propType = *i;
               if (propType != NULL)
               {
                  DynamicAbstractControl* propertyControl = GetDynamicControlFactory()->CreateDynamicControl(*propType);
                  if (propertyControl != NULL)
                  {
                     propertyControl->SetTreeView(mPropertyTree);
                     propertyControl->SetDynamicControlFactory(GetDynamicControlFactory());

                     int linkCount = (int)mLinkedProperties.size();
                     for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
                     {
                        LinkedPropertyData& data = mLinkedProperties[linkIndex];
                        dtCore::BasePropertyContainerActorProperty* linkedProp =
                           dynamic_cast<dtCore::BasePropertyContainerActorProperty*>(data.property);
                        if (linkedProp)
                        {
                           dtCore::RefPtr<dtCore::PropertyContainer> linkedCon = linkedProp->GetValue();
                           if (linkedCon)
                           {
                              propertyControl->AddLinkedProperty(linkedCon, linkedCon->GetProperty(propType->GetName()));
                           }
                        }
                     }

                     // Note using the new property container as the container for this sub control.
                     propertyControl->InitializeData(this, newModel, pc, propType);

                     connect(propertyControl, SIGNAL(PropertyAboutToChange(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                                       const std::string&, const std::string&)),
                              this, SLOT(PropertyAboutToChangePassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                                       const std::string&, const std::string&)));

                     connect(propertyControl, SIGNAL(PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)),
                              this, SLOT(PropertyChangedPassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&)));

                     mChildren.push_back(propertyControl);
                  }
               }
            }
         }
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicPropertyContainerControl::getDisplayName()
   {
      QString name = DynamicAbstractParentControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }

      if (mProperty.valid())
      {
         return QString(tr(mProperty->GetLabel().c_str()));
      }
      else
      {
         LOG_ERROR("Dynamic array control has an invalid property type");
         return tr("");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicPropertyContainerControl::getDescription()
   {
      if(mProperty.valid())
      {
         return QString(tr(mProperty->GetDescription().c_str())) + QString("  [Type: ") +
            QString(tr(mProperty->GetDataType().GetName().c_str())) + QString("]");
      }
      else
      {
         LOG_ERROR("Dynamic property container control has an invalid property type");
         return tr("");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicPropertyContainerControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      if (doPropertiesMatch())
      {
         return getDescription();
      }
      else
      {
         return "<Multiple Values...>";
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   bool DynamicPropertyContainerControl::updateData(QWidget* widget)
   {
      // this guy doesn't have any editors.  All the data is edited in child controls
      return false;
   }
}
