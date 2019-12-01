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
#include <dtQt/basepropertyeditor.h> // just for 2 static methods.
#include <dtCore/namedpropertycontainerparameter.h>
#include <dtCore/namedgroupparameter.inl>
#include <dtCore/datatype.h>
#include <dtUtil/log.h>

namespace dtQt
{
   DynamicPropertyContainerControl::DynamicPropertyContainerControl()
   : DynamicGroupControl("PropertyContainer")
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
         dtCore::PropContRefPtrVector containers;
         if (pc != NULL)
         {
            containers.push_back(pc);
            size_t linkCount = mLinkedProperties.size();
            for (size_t linkIndex = 0; linkIndex < linkCount; ++linkIndex)
            {
               LinkedPropertyData& data = mLinkedProperties[linkIndex];
               dtCore::BasePropertyContainerActorProperty* linkedProp =
                  dynamic_cast<dtCore::BasePropertyContainerActorProperty*>(data.property);
               if (linkedProp != NULL)
               {
                  dtCore::PropertyContainerPtr linkedCon = linkedProp->GetValue();
                  if (linkedCon.valid())
                  {
                     containers.push_back(linkedCon);
                  }
               }
            }
            InitWithPropertyContainers(containers, newModel);
         }
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }

   }

   void DynamicPropertyContainerControl::InitWithPropertyContainers(const dtCore::PropContRefPtrVector& propContainers, PropertyEditorModel* newModel)
   {
      if (propContainers.empty())
         return;

      dtCore::PropertyContainerPtr pc = propContainers[0];
      dtQt::BasePropertyEditor::ForEachNestedProperty(*pc, [&](dtCore::PropertyContainer& cont, dtCore::ActorProperty& prop)
            {
         dtCore::ActorProperty* baseProp = &prop;

         try
         {
            // first create the control.  Sometimes the controls aren't creatable, so
            // check that first before we do other work.  Excepts if it fails
            DynamicAbstractControl* newControl = GetDynamicControlFactory()->CreateDynamicControl(*baseProp);
            if (newControl == NULL)
            {
               LOG_ERROR("Object Factory failed to create a control for property: " + baseProp->GetDataType().GetName());
            }
            else
            {

               newControl->SetTreeView(mPropertyTree);
               newControl->SetDynamicControlFactory(GetDynamicControlFactory());

               for (size_t linkIndex = 1, linkCount = propContainers.size(); linkIndex < linkCount; ++linkIndex)
               {
                  dtCore::RefPtr<dtCore::PropertyContainer> linkedCon = propContainers[linkIndex];
                  if (linkedCon.valid())
                  {
                     dtCore::PropertyPtr linkedProp = dtQt::BasePropertyEditor::FindNestedProperty(*linkedCon, baseProp->GetName());
                     if (linkedProp.valid())
                        newControl->AddLinkedProperty(linkedCon, linkedProp.get());
                  }
               }

               // Work with the group.  Requires finding an existing group or creating one,
               // and eventually adding our new control to that group control
               const std::string& groupName = baseProp->GetGroupName();
               if (!groupName.empty())
               {
                  // find our group
                  DynamicGroupControl* groupControl = getChildGroupControl(QString(groupName.c_str()));

                  // if no group, then create one.
                  if (groupControl == nullptr)
                  {
                     groupControl = new DynamicGroupControl(groupName);
                     //groupControl->SetTreeView(mPropertyTree);
                     groupControl->InitializeData(this, newModel, pc, nullptr);
                     addChildControlSorted(groupControl, newModel);
                  }

                  // add our new control to the group.
                  newControl->InitializeData(groupControl, newModel, &cont, baseProp);
                  groupControl->addChildControl(newControl, newModel);
               }
               else
               {
                  // there's no group, so use the root.
                  newControl->InitializeData(this, newModel, &cont, baseProp);
                  addChildControl(newControl, newModel);
               }
            }
         }
         catch (dtUtil::Exception& ex)
         {
            LOG_ERROR("Failed to create a control for property: " + baseProp->GetDataType().GetName() +
               " with error: " + ex.What());
         }

            });

   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicPropertyContainerControl::getDisplayName()
   {
      QString name = DynamicGroupControl::getDisplayName();
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

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicPropertyContainerControl::PropertyAboutToChangePassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop,
            std::string oldValue, std::string newValue)
   {
      if (mPropContainer.valid())
      {
         dtCore::RefPtr<dtCore::NamedPropertyContainerParameter> val = new dtCore::NamedPropertyContainerParameter(mProperty->GetName());

         dtCore::NamedParameter* subParam =  val->AddParameter(prop.GetName(), prop.GetDataType());
         subParam->FromString(oldValue);
         // Workaround.  Because the undo/redo system doesn't (yet) support the values of nested properties, I have to change it to
         // using the containing property and adding the changed property as a value.
         oldValue = val->ToString();
         subParam->FromString(newValue);
         newValue = val->ToString();
         emit PropertyAboutToChange(*mPropContainer, *mProperty, oldValue, newValue);
      }
      else
      {
         DynamicGroupControl::PropertyAboutToChangePassThrough(pc, prop, oldValue, newValue);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicPropertyContainerControl::PropertyChangedPassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop)
   {
      if (mPropContainer.valid())
      {
         emit PropertyChanged(*mPropContainer, *mProperty);
      }
      else
      {
         DynamicGroupControl::PropertyChangedPassThrough(pc, prop);
      }
   }


}
