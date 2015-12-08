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
#include <dtCore/project.h>
#include <dtQt/dynamicactorcontrol.h>
#include <dtQt/propertyeditortreeview.h>
#include <dtCore/actoractorproperty.h>
#include <dtCore/actoridactorproperty.h>
#include <dtCore/map.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/datatype.h>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>


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
      dtQt::PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - Unlike the other properties, we can't static or reinterpret cast this object.
      // We need to dynamic cast it...
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::ACTOR)
      {
         mProperty = dynamic_cast<dtCore::ActorActorProperty*>(newProperty);
         mIdProperty = dynamic_cast<dtCore::ActorIDActorProperty*>(newProperty);
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
      if (widget != NULL && widget == mWrapper && mTemporaryEditControl)
      {
         int index = mTemporaryEditControl->findText(getValueAsString());
         mTemporaryEditControl->setCurrentIndex(index);
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
         if (mTemporaryEditControl->currentIndex() > -1)
         {
            // Get the current selected string and the previously set string value
            std::string currentActorID = mTemporaryEditControl->itemData(mTemporaryEditControl->currentIndex()).toString().toStdString();
            std::string previousActorID = getActorProperty()->ToString();

            // set our value to our object
            if (currentActorID != previousActorID)
            {
               // give undo manager the ability to create undo/redo events
               emit PropertyAboutToChange(*mPropContainer, *getActorProperty(), previousActorID, currentActorID);

               mBaseProperty->FromString(currentActorID);

               dataChanged = true;
            }
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

      if (!IsInitialized())
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      mTemporaryEditControl = new dtQt::SubQComboBox(wrapper, this);

      std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > names;
      std::string proxyClass;
      if (mProperty)
      {
         proxyClass = mProperty->GetDesiredActorClass();
      }
      else if (mIdProperty)
      {
         proxyClass = mIdProperty->GetDesiredActorClass();
      }
      GetActors(names, proxyClass);

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
   dtCore::BaseActorObject* DynamicActorControl::getActor()
   {
      if (mProperty)
      {
         return mProperty->GetValue();
      }

      if (mIdProperty)
      {
         return mIdProperty->GetActor();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* DynamicActorControl::getActorProperty()
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

      if (doPropertiesMatch())
      {
         dtCore::BaseActorObject* actor = getActor();
         if (!actor)
         {
            dtCore::ActorProperty* prop = getActorProperty();
            if (prop && !prop->ToString().empty())
            {
               return "<Unknown>";
            }

            return "<None>";
         }

         return actor->GetName().c_str();
      }
      else
      {
         return "<Multiple Values...>";
      }
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
         CopyBaseValueToLinkedProperties();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::actorPropertyChanged(dtCore::PropertyContainer& propCon,
      dtCore::ActorProperty& property)
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
   void DynamicActorControl::GetActors(std::vector< dtCore::RefPtr<dtCore::BaseActorObject> >& toFill, const std::string& className)
   {
      toFill.clear();

      bool showPrototypes = false;
      if (mProperty)
      {
         showPrototypes = mProperty->GetShowPrototypes();
      }

      if (mIdProperty)
      {
         showPrototypes = mIdProperty->GetShowPrototypes();
      }

      int count = dtCore::BaseActorObject::GetInstanceCount();
      for (int index = 0; index < count; ++index)
      {
         dtCore::BaseActorObject* object = dtCore::BaseActorObject::GetInstance(index);
         if (object)
         {
            if (!className.empty() && !object->IsInstanceOf(className))
            {
               continue;
            }

            dtCore::ActorProperty* ghostProp = object->GetProperty("Is Ghost");
            if (!ghostProp || ghostProp->ToString() == "false")
            {
               dtCore::ActorProperty* prototypeProp = object->GetProperty("Initial Ownership");
               bool isPrototype = false;
               if (prototypeProp && prototypeProp->ToString() == "PROTOTYPE")
               {
                  isPrototype = true;
               }

               if (!isPrototype || showPrototypes)
               {
                  toFill.push_back(object);
               }
            }
         }
      }
   }

} // namespace dtEditQt
