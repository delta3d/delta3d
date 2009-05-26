/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Curtiss Murphy
 */
#include <prefix/dtstageprefix-src.h>
#include <dtUtil/enumeration.h>
#include <dtEditQt/dynamicenumcontrol.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

namespace dtEditQt
{

   //const QString DynamicBoolControl::TRUE_LABEL("True");
   //const QString DynamicBoolControl::FALSE_LABEL("False");

   ///////////////////////////////////////////////////////////////////////////////
   DynamicEnumControl::DynamicEnumControl()
      : mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicEnumControl::~DynamicEnumControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicEnumControl::initializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::ActorProxy* newProxy, dtDAL::ActorProperty* newProperty)
   {
      // Note - Unlike the other properties, we can't static or reinterpret cast this object.
      // We need to dynamic cast it...
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::ENUMERATION)
      {
         mProperty = dynamic_cast<dtDAL::AbstractEnumActorProperty*>(newProperty);
         DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicEnumControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget != NULL)
      {
         SubQComboBox* editor = static_cast<SubQComboBox*>(widget);

         // set the current value from our property
         dtUtil::Enumeration& value = mProperty->GetEnumValue();
         editor->setCurrentIndex(editor->findText(QString(value.GetName().c_str())));
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicEnumControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget != NULL)
      {
         SubQComboBox* editor = static_cast<SubQComboBox*>(widget);

         // Get the current selected string and the previously set string value
         QString selection = editor->currentText();
         std::string selectionString = selection.toStdString();
         dtUtil::Enumeration& previousValue = mProperty->GetEnumValue();
         std::string previousString = previousValue.GetName();

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mProxy,
               *mProperty->AsActorProperty(), previousString, selectionString);

            mProperty->SetValueFromString(selectionString);
            dataChanged = true;
         }
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         emit PropertyChanged(*mProxy, *mProperty->AsActorProperty());
      }

      return dataChanged;
   }


   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicEnumControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      // create and init the combo box
      mTemporaryEditControl = new SubQComboBox(parent, this);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return mTemporaryEditControl;
      }

      const std::vector<dtUtil::Enumeration*>& options = mProperty->GetList();
      std::vector<dtUtil::Enumeration*>::const_iterator iter;

      for (iter = options.begin(); iter != options.end(); ++iter)
      {
         dtUtil::Enumeration* enumValue = (*iter);
         mTemporaryEditControl->addItem(QString(enumValue->GetName().c_str()));
      }

      connect(mTemporaryEditControl, SIGNAL(activated (int)), this, SLOT(itemSelected(int)));

      updateEditorFromModel(mTemporaryEditControl);

      // set the tooltip
      mTemporaryEditControl->setToolTip(getDescription());

      return mTemporaryEditControl;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicEnumControl::getDisplayName()
   {
      return QString(tr(mProperty->AsActorProperty()->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicEnumControl::getDescription()
   {
      std::string tooltip = mProperty->AsActorProperty()->GetDescription() + "  [Type: " +
         mProperty->AsActorProperty()->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicEnumControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      dtUtil::Enumeration &value = mProperty->GetEnumValue();
      return QString(value.GetName().c_str());
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicEnumControl::isEditable()
   {
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicEnumControl::itemSelected(int index)
   {
      if (mTemporaryEditControl != NULL)
      {
         updateModelFromEditor(mTemporaryEditControl);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicEnumControl::updateData(QWidget* widget)
   {
      if (!mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicEnumControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
      dtCore::RefPtr<dtDAL::ActorProperty> property)
   {
      DynamicAbstractControl::actorPropertyChanged(proxy, property);

      dtDAL::AbstractEnumActorProperty* changedProp =
         dynamic_cast<dtDAL::AbstractEnumActorProperty *>(property.get());

      if (mTemporaryEditControl != NULL && proxy == mProxy && changedProp == mProperty)
      {
         updateEditorFromModel(mTemporaryEditControl);
      }
   }

} // namespace dtEditQt
