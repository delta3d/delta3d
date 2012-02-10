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
 * @author William E. Johnson II
 */
#include <prefix/dtqtprefix.h>
#include <dtQt/dynamicgameeventcontrol.h>

#include <dtCore/project.h>
#include <dtCore/datatype.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/map.h>

#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

namespace dtQt
{

   DynamicGameEventControl::DynamicGameEventControl()
      : mTemporaryEditControl(NULL)
   {
   }

   DynamicGameEventControl::~DynamicGameEventControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGameEventControl::InitializeData(dtQt::DynamicAbstractControl* newParent,
      dtQt::PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - Unlike the other properties, we can't static or reinterpret cast this object.
      // We need to dynamic cast it...
      if (newProperty != NULL && newProperty->GetDataType() == dtCore::DataType::GAME_EVENT)
      {
         mProperty = dynamic_cast<dtCore::GameEventActorProperty*>(newProperty);
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
   void DynamicGameEventControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         if (mProperty->GetValue() != NULL)
         {
            mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText(mProperty->GetValue()->GetName().c_str()));
         }
         else
         {
            mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText("<None>"));
         }
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGameEventControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper && mTemporaryEditControl)
      {
         // Get the current selected string and the previously set string value
         QString selection = mTemporaryEditControl->currentText();

         std::string selectionString = selection.toStdString();
         std::string previousString = mProperty->GetValue() != NULL ? mProperty->GetValue()->GetName() : "<None>";

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mProperty, previousString, selectionString);

            mProperty->SetValue(GetGameEvent(selectionString));

            dataChanged = true;
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
   QWidget* DynamicGameEventControl::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      mTemporaryEditControl = new dtQt::SubQComboBox(wrapper, this);
      std::vector<dtCore::GameEvent*> events;

      std::vector<dtCore::Map*> maps = dtCore::Project::GetInstance().GetOpenMaps();
      for (int index = 0; index < (int)maps.size(); ++index)
      {
         dtCore::Map* map = maps[index];
         if (map)
         {
            map->GetEventManager().GetAllEvents(events);
         }
      }

      QStringList sortedEventNames;
      for (unsigned int i = 0; i < events.size(); ++i)
      {
         sortedEventNames.append(QString(events[i]->GetName().c_str()));
      }
      sortedEventNames.sort();
      // Insert the None option at the end of the list
      QStringList listPlusNone;
      listPlusNone.append(QString("<None>"));
      listPlusNone += sortedEventNames;
      mTemporaryEditControl->addItems(listPlusNone);
      mTemporaryEditControl->setToolTip(getDescription());

      connect(mTemporaryEditControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

      updateEditorFromModel(mWrapper);

      mGridLayout->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryEditControl->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      wrapper->setFocusProxy(mTemporaryEditControl);
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGameEventControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return QString(tr(mProperty->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGameEventControl::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " + mProperty->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGameEventControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      if (doPropertiesMatch())
      {
         return mProperty->GetValue() != NULL ? QString(mProperty->GetValue()->GetName().c_str()) : QString("<None>");
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
   void DynamicGameEventControl::itemSelected(int index)
   {
      if (mTemporaryEditControl != NULL)
      {
         updateModelFromEditor(mWrapper);
         CopyBaseValueToLinkedProperties();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGameEventControl::updateData(QWidget* widget)
   {
      if (mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGameEventControl::actorPropertyChanged(dtCore::PropertyContainer& propCon,
            dtCore::ActorProperty& property)
   {
      DynamicAbstractControl::actorPropertyChanged(propCon, property);

      dtCore::GameEventActorProperty* changedProp = dynamic_cast<dtCore::GameEventActorProperty*>(&property);

      if (mTemporaryEditControl != NULL && &propCon == mPropContainer && changedProp == mProperty)
      {
         updateEditorFromModel(mWrapper);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicGameEventControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      // we have to check - sometimes the destructor won't get called before the
      // next widget is created.  Then, when it is called, it sets the NEW editor to NULL!
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
      }

      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::GameEvent* DynamicGameEventControl::GetGameEvent(const std::string& eventID)
   {
      std::vector<dtCore::Map*> maps = dtCore::Project::GetInstance().GetOpenMaps();
      for (int index = 0; index < (int)maps.size(); ++index)
      {
         dtCore::Map* map = maps[index];
         if (map)
         {
            dtCore::GameEvent* event = map->GetEventManager().FindEvent(eventID);
            if (event)
            {
               return event;
            }
         }
      }

      return NULL;
   }

} // namespace dtEditQt
