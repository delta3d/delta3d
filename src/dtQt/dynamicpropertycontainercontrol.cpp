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

#include <dtDAL/datatype.h>

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
      PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::PROPERTY_CONTAINER)
      {
         mProperty = static_cast<dtDAL::BasePropertyContainerActorProperty*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);

         dtDAL::PropertyContainer* pc = mProperty->GetValue();
         if (pc != NULL)
         {
            std::vector<dtDAL::ActorProperty*> properties;
            pc->GetPropertyList(properties);
            std::vector<dtDAL::ActorProperty*>::iterator i, iend;
            i = properties.begin();
            iend = properties.end();
            for (; i != iend ; ++i)
            {
               dtDAL::ActorProperty* propType = *i;
               if (propType != NULL)
               {
                  DynamicAbstractControl* propertyControl = GetDynamicControlFactory()->CreateDynamicControl(*propType);
                  if (propertyControl != NULL)
                  {
                     propertyControl->SetTreeView(mPropertyTree);
                     propertyControl->SetDynamicControlFactory(GetDynamicControlFactory());
                     // Note using the new property container as the container for this sub control.
                     propertyControl->InitializeData(this, newModel, pc, propType);

                     connect(propertyControl, SIGNAL(PropertyAboutToChange(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                                       const std::string&, const std::string&)),
                              this, SLOT(PropertyAboutToChangePassThrough(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                                       const std::string&, const std::string&)));

                     connect(propertyControl, SIGNAL(PropertyChanged(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)),
                              this, SLOT(PropertyChangedPassThrough(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)));
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
      return getDescription();
   }

   bool DynamicPropertyContainerControl::isEditable()
   {
      return !mProperty->IsReadOnly();
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
