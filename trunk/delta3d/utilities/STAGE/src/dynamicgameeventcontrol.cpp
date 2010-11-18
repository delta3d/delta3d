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
#include <prefix/stageprefix.h>

#include <dtDAL/datatype.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventactorproperty.h>
#include <dtDAL/map.h>

#include <dtEditQt/dynamicgameeventcontrol.h>
#include <dtEditQt/editordata.h>

#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

namespace dtEditQt
{

   DynamicGameEventControl::DynamicGameEventControl()
   {
   }

   DynamicGameEventControl::~DynamicGameEventControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGameEventControl::InitializeData(dtQt::DynamicAbstractControl* newParent,
      dtQt::PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      // Note - Unlike the other properties, we can't static or reinterpret cast this object.
      // We need to dynamic cast it...
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::GAME_EVENT)
      {
         mProperty = dynamic_cast<dtDAL::GameEventActorProperty*>(newProperty);
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
      if (widget != NULL)
      {
         //SubQComboBox *editor = static_cast<SubQComboBox*>(widget);

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
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGameEventControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget != NULL)
      {
         //SubQComboBox *editor = static_cast<SubQComboBox*>(widget);

         // Get the current selected string and the previously set string value
         QString selection = mTemporaryEditControl->currentText();
         //int index = temporaryEditControl->findText(selection);
         std::string selectionString = selection.toStdString();
         std::string previousString = mProperty->GetValue() != NULL ? mProperty->GetValue()->GetName() : "<None>";

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mProperty, previousString, selectionString);

            dtDAL::GameEvent* eventToSet = NULL;
            //std::vector<dtDAL::GameEvent*> events;
            dtDAL::Map& curMap = *EditorData::GetInstance().getCurrentMap();
            //curMap.GetEventManager().GetAllEvents(events);
            //for (unsigned int i = 0; i < events.size(); i++)
            eventToSet = curMap.GetEventManager().FindEvent(selectionString);
            //{
            //   if (events[i]->GetName() == selectionString)
            //   {
            //      eventToSet = events[i];
            //      break;
            //   }
            //}
            mProperty->SetValue(eventToSet);
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
      mTemporaryEditControl = new dtQt::SubQComboBox(parent, this);
      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return mTemporaryEditControl;
      }

      std::vector<dtDAL::GameEvent*> events;
      dtDAL::Map& map = *EditorData::GetInstance().getCurrentMap();
      map.GetEventManager().GetAllEvents(events);

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

      connect(mTemporaryEditControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

      updateEditorFromModel(mTemporaryEditControl);

      // set the tooltip
      mTemporaryEditControl->setToolTip(getDescription());
      return mTemporaryEditControl;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGameEventControl::getDisplayName()
   {
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
      return mProperty->GetValue() != NULL ? QString(mProperty->GetValue()->GetName().c_str()) : QString("<None>");
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGameEventControl::isEditable()
   {
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGameEventControl::itemSelected(int index)
   {
      if (mTemporaryEditControl != NULL)
      {
         updateModelFromEditor(mTemporaryEditControl);
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
   void DynamicGameEventControl::actorPropertyChanged(dtDAL::PropertyContainer& propCon,
            dtDAL::ActorProperty& property)
   {
      DynamicAbstractControl::actorPropertyChanged(propCon, property);

      dtDAL::GameEventActorProperty* changedProp = dynamic_cast<dtDAL::GameEventActorProperty*>(&property);

      if (mTemporaryEditControl != NULL && &propCon == mPropContainer && changedProp == mProperty)
      {
         updateEditorFromModel(mTemporaryEditControl);
      }
   }

} // namespace dtEditQt
