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
 * Jeffrey Houde
 */

#include <prefix/dtqtprefix.h>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>
#include <dtQt/dynamiccontainercontrol.h>
#include <dtCore/actorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/containeractorproperty.h>
#include <dtUtil/log.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>
#include <QtGui/QLabel>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicContainerControl::DynamicContainerControl()
      : mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicContainerControl::~DynamicContainerControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicContainerControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::CONTAINER)
      {
         mProperty = static_cast<dtCore::ContainerActorProperty*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);
         resizeChildren();
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicContainerControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         mTemporaryEditControl->setText(getValueAsString());
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicContainerControl::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      // create and init the edit box
      mTemporaryEditControl = new SubQLabel(getValueAsString(), wrapper, this);
      mTemporaryEditControl->setToolTip(getDescription());

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      wrapper->setFocusProxy(mTemporaryEditControl);
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicContainerControl::getDisplayName()
   {
      resizeChildren();

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
   const QString DynamicContainerControl::getDescription()
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
   const QString DynamicContainerControl::getValueAsString()
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

   ///////////////////////////////////////////////////////////////////////////////
   void DynamicContainerControl::handleSubEditDestroy(QWidget* widget,
      QAbstractItemDelegate::EndEditHint hint /*= QAbstractItemDelegate::NoHint*/)
   {
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
      }

      DynamicAbstractParentControl::handleSubEditDestroy(widget, hint);
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   bool DynamicContainerControl::updateData(QWidget* widget)
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicContainerControl::resizeChildren()
   {
      PropertyEditorModel* model = GetModel();
      if (!model)
      {
         return false;
      }

      int size = mProperty->GetPropertyCount();

      if (size != (int)mChildren.size())
      {
         if (!mChildren.empty())
         {
            model->removeRows(0, (int)mChildren.size(), model->IndexOf(this));

            removeAllChildren(model);
         }

         for (int index = 0; index < size; index++)
         {
            dtCore::ActorProperty* propType = mProperty->GetProperty(index);
            if (propType)
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
                     dtCore::RefPtr<dtCore::ContainerActorProperty> linkedProp = dynamic_cast<dtCore::ContainerActorProperty*>(data.property);
                     if (linkedProp)
                     {
                        propertyControl->AddLinkedProperty(mPropContainer, linkedProp->GetProperty(index));
                     }
                  }

                  propertyControl->InitializeData(this, model, mPropContainer, propType);

                  connect(propertyControl, SIGNAL(PropertyAboutToChange(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                     const std::string&, const std::string&)),
                     this, SLOT(PropertyAboutToChangePassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                     const std::string&, const std::string&)));

                  connect(propertyControl, SIGNAL(PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)),
                     this, SLOT(PropertyChangedPassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&)));

                  mChildren.push_back(propertyControl);
               }
            }

            model->insertRows(0, size, model->IndexOf(this));
         }
         return true;
      }

      return false;
   }
} // namespace dtQt
