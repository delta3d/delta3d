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
 * William E. Johnson II
 */
#include <prefix/dtqtprefix.h>
#include <dtDAL/project.h>
#include <dtQt/dynamicactorcontrol.h>
#include <dtQt/propertyeditortreeview.h>
#include <dtDAL/actoractorproperty.h>
#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/datatype.h>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>


namespace dtQt
{

   DynamicActorControl::DynamicActorControl()
      : mProperty(NULL)
      , mIdProperty(NULL)
      , mTemporaryEditControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicActorControl::~DynamicActorControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::InitializeData(dtQt::DynamicAbstractControl* newParent,
      dtQt::PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      // Note - Unlike the other properties, we can't static or reinterpret cast this object.
      // We need to dynamic cast it...
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::ACTOR)
      {
         mProperty = dynamic_cast<dtDAL::ActorActorProperty*>(newProperty);
         mIdProperty = dynamic_cast<dtDAL::ActorIDActorProperty*>(newProperty);
         dtQt::DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         dtDAL::BaseActorObject* proxy = getActorProxy();
         if (proxy)
         {
            mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText(proxy->GetName().c_str()));
         }
         else
         {
            mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText("<None>"));
         }
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicActorControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper && mTemporaryEditControl)
      {
         // Get the current selected string and the previously set string value
         std::string currentActorID = mTemporaryEditControl->itemData(mTemporaryEditControl->currentIndex()).toString().toStdString();

         dtDAL::BaseActorObject* currentProxy = getActorProxy();
         std::string previousActorID = currentProxy ? currentProxy->GetId().ToString() : "";

         // set our value to our object
         if (currentActorID != previousActorID)
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *getActorProperty(), previousActorID, currentActorID);

            mBaseProperty->FromString(currentActorID);

            dataChanged = true;
         }
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         emit PropertyChanged(*mPropContainer, *getActorProperty());
      }

      return dataChanged;
   }


   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicActorControl::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      mTemporaryEditControl = new dtQt::SubQComboBox(wrapper, this);

      std::vector< dtCore::RefPtr<dtDAL::BaseActorObject> > names;
      std::string proxyClass;
      if (mProperty)
      {
         proxyClass = mProperty->GetDesiredActorClass();
      }
      else if (mIdProperty)
      {
         proxyClass = mIdProperty->GetDesiredActorClass();
      }
      GetActorProxies(names, proxyClass);

      QStringList sortedNames;
      for (unsigned int i = 0; i < names.size(); ++i)
      {
         sortedNames.append(QString(names[i]->GetName().c_str()));
      }
      sortedNames.sort();

      mTemporaryEditControl->addItem("<None>");

      for (unsigned int i = 0; i < (unsigned int)sortedNames.size(); ++i)
      {
         QString name = sortedNames[i];

         // Find the actor that matches this name.
         for (unsigned int n = 0; n < names.size(); ++n)
         {
            if (name == names[n]->GetName().c_str())
            {
               mTemporaryEditControl->addItem(name, QVariant(names[n]->GetId().ToString().c_str()));

               names.erase(names.begin() + n);
               break;
            }
         }
      }
      mTemporaryEditControl->setToolTip(getDescription());

      connect(mTemporaryEditControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

      updateEditorFromModel(mWrapper);

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnStretch(0, 2);

      wrapper->setFocusProxy(mTemporaryEditControl);
      return wrapper;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::BaseActorObject* DynamicActorControl::getActorProxy()
   {
      if (mProperty)
      {
         return mProperty->GetValue();
      }

      if (mIdProperty)
      {
         return mIdProperty->GetActorProxy();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* DynamicActorControl::getActorProperty()
   {
      if (mProperty)
      {
         return mProperty;
      }

      return mIdProperty;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }

      return tr(getActorProperty()->GetLabel().c_str());
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getDescription()
   {
      std::string tooltip = getActorProperty()->GetDescription() + "  [Type: " + getActorProperty()->GetDataType().GetName() + "]";
      return tr(tooltip.c_str());
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();

      dtDAL::BaseActorObject* proxy = getActorProxy();
      return proxy != NULL ? QString(proxy->GetName().c_str()) : QString("<None>");
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicActorControl::isEditable()
   {
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::itemSelected(int index)
   {
      if (mTemporaryEditControl != NULL)
      {
         updateModelFromEditor(mWrapper);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicActorControl::updateData(QWidget* widget)
   {
      if (mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::actorPropertyChanged(dtDAL::PropertyContainer& propCon,
      dtDAL::ActorProperty& property)
   {
      DynamicAbstractControl::actorPropertyChanged(propCon, property);

      if (mTemporaryEditControl != NULL && &propCon == mPropContainer && &property == getActorProperty())
      {
         updateEditorFromModel(mWrapper);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
      }

      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::GetActorProxies(std::vector< dtCore::RefPtr<dtDAL::BaseActorObject> >& toFill, const std::string& className)
   {
      toFill.clear();

      std::vector<dtDAL::Map*> maps = dtDAL::Project::GetInstance().GetOpenMaps();
      for (int index = 0; index < (int)maps.size(); ++index)
      {
         dtDAL::Map* map = maps[index];
         if (map)
         {
            std::vector< dtCore::RefPtr<dtDAL::BaseActorObject> > list;
            map->FindProxies(list, "", "", "", className);

            int listCount = (int)list.size();
            for (int listIndex = 0; listIndex < listCount; ++listIndex)
            {
               toFill.push_back(list[listIndex]);
            }
         }
      }
   }

} // namespace dtEditQt
