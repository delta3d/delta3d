/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2012, MOVES Institute
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
 * Jeff P. Houde
 */
#include <prefix/dtqtprefix.h>
#include <dtQt/dynamiccontainerselectorcontrol.h>

#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>
#include <dtCore/containerselectoractorproperty.h>

#include <dtQt/dynamicsubwidgets.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtUtil/log.h>

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicContainerSelectorControl::DynamicContainerSelectorControl()
      : mProperty(NULL)
      , mTemporaryComboControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicContainerSelectorControl::~DynamicContainerSelectorControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicContainerSelectorControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::CONTAINER_SELECTOR)
      {
         mProperty = static_cast<dtCore::ContainerSelectorActorProperty*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);
         refreshChildren();
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicContainerSelectorControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper)
      {
         if (mTemporaryComboControl)
         {
            mTemporaryComboControl->setCurrentIndex(mTemporaryComboControl->findText(tr(mProperty->GetValue().c_str())));
            mTemporaryComboControl->setEditText(tr(mProperty->GetValue().c_str()));
         }
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicContainerSelectorControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper)
      {
         std::string result;
         if (mTemporaryComboControl)
         {
            result = mTemporaryComboControl->currentText().toStdString();
         }

         // set our value to our object
         if (result != mProperty->GetValue())
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mProperty,
               mProperty->ToString(), result);

            mProperty->SetValue(result);
            dataChanged = true;

            refreshChildren();
         }
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         emit PropertyChanged(*mPropContainer, *mProperty);
      }

      return dataChanged;
   }


   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicContainerSelectorControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      mTemporaryComboControl = new SubQComboBox(wrapper, this);
      mTemporaryComboControl->setToolTip(getDescription());
      std::vector<std::string> stringList = mProperty->GetList();
      mTemporaryComboControl->addItem("<None>");
      int count = (int)stringList.size();
      for (int index = 0; index < count; ++index)
      {
         mTemporaryComboControl->addItem(stringList[index].c_str());
      }

      mTemporaryComboControl->setEditable(false);

      updateEditorFromModel(mWrapper);

      mGridLayout->addWidget(mTemporaryComboControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryComboControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      connect(mTemporaryComboControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

      wrapper->setFocusProxy(mTemporaryComboControl);

      return wrapper;
   }

   const QString DynamicContainerSelectorControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return QString(tr(mProperty->GetLabel().c_str()));
   }

   const QString DynamicContainerSelectorControl::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
         mProperty->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   const QString DynamicContainerSelectorControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      return QString(tr(mProperty->GetValue().c_str()));
   }

   bool DynamicContainerSelectorControl::isEditable()
   {
      return !mProperty->IsReadOnly();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicContainerSelectorControl::refreshChildren()
   {
      // Clear all the current child controls.
      int childCount = getChildCount();
      PropertyEditorModel* model = GetModel();
      if (model && childCount > 0)
      {
         model->removeRows(0, childCount, model->IndexOf(this));

         removeAllChildren(model);
      }

      // Now retrieve the container and update all the property children.
      dtCore::PropertyContainer* propCon = mProperty->GetContainer();
      if (propCon)
      {
         std::vector<dtCore::ActorProperty*> propList;
         propCon->GetPropertyList(propList);
         int count = (int)propList.size();
         for (int index = 0; index < count; ++index)
         {
            dtCore::ActorProperty* prop = propList[index];
            if (prop)
            {
               DynamicAbstractControl* element = GetDynamicControlFactory()->CreateDynamicControl(*prop);
               element->SetTreeView(mPropertyTree);
               element->SetDynamicControlFactory(GetDynamicControlFactory());
               element->InitializeData(this, GetModel(), propCon, prop);
               connect(element, SIGNAL(PropertyAboutToChange(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                  const std::string&, const std::string&)),
                  this, SLOT(PropertyAboutToChangePassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&,
                  const std::string&, const std::string&)));

               connect(element, SIGNAL(PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty&)),
                  this, SLOT(PropertyChangedPassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty&)));
               mChildren.push_back(element);
            }
         }

         model->insertRows(0, count, model->IndexOf(this));
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   bool DynamicContainerSelectorControl::updateData(QWidget* widget)
   {
      // returns true if we successfully change data
      bool dataChanged = false;

      if (!mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return dataChanged;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicContainerSelectorControl::itemSelected(int index)
   {
      if (mTemporaryComboControl != NULL)
      {
         updateModelFromEditor(mWrapper);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicContainerSelectorControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTemporaryComboControl = NULL;
      }
      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }

} // namespace dtQt
