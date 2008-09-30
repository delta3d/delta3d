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
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/dynamicgameeventcontrol.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/gameevent.h>
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

   void DynamicGameEventControl::initializeData(DynamicAbstractControl *newParent,
      PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
   {
      // Note - Unlike the other properties, we can't static or reinterpret cast this object.
      // We need to dynamic cast it...
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::GAME_EVENT)
      {
         myProperty = dynamic_cast<dtDAL::GameEventActorProperty*>(newProperty);
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
   void DynamicGameEventControl::updateEditorFromModel(QWidget *widget)
   {
      if (widget != NULL)
      {
         //SubQComboBox *editor = static_cast<SubQComboBox*>(widget);

         // set the current value from our property
         if(myProperty->GetValue() != NULL)
            temporaryEditControl->setCurrentIndex(temporaryEditControl->findText(myProperty->GetValue()->GetName().c_str()));
         else
            temporaryEditControl->setCurrentIndex(temporaryEditControl->findText("<None>"));
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGameEventControl::updateModelFromEditor(QWidget *widget)
   {
      bool dataChanged = false;

      if (widget != NULL)
      {
         //SubQComboBox *editor = static_cast<SubQComboBox*>(widget);

         // Get the current selected string and the previously set string value
         QString selection = temporaryEditControl->currentText();
         //int index = temporaryEditControl->findText(selection);
         std::string selectionString = selection.toStdString();
         std::string previousString = myProperty->GetValue() != NULL ? myProperty->GetValue()->GetName() : "<None>";

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, myProperty, previousString, selectionString);

            dtDAL::GameEvent *eventToSet = NULL;
            //std::vector<dtDAL::GameEvent*> events;
            dtDAL::Map &curMap = *EditorData::GetInstance().getCurrentMap();
            //curMap.GetEventManager().GetAllEvents(events);
            eventToSet = curMap.GetEventManager().FindEvent(selectionString);
            //for(unsigned int i = 0; i < events.size(); i++)
            //{
            //   if(events[i]->GetName() == selectionString)
            //   {  
            //      eventToSet = events[i];
            //      break;
            //   }                  
            //}
            myProperty->SetValue(eventToSet);
            dataChanged = true;
         }
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         EditorEvents::GetInstance().emitActorPropertyChanged(proxy, myProperty);
      }

      return dataChanged;    
   }


   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicGameEventControl::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index)
   {
      temporaryEditControl = new SubQComboBox(parent, this);
      if (!initialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return temporaryEditControl;
      }

      std::vector<dtDAL::GameEvent*> events;
      dtDAL::Map &map = *EditorData::GetInstance().getCurrentMap();
      map.GetEventManager().GetAllEvents(events);

      QStringList sortedEventNames;
      for(unsigned int i = 0; i < events.size(); i++)
         sortedEventNames.append(QString(events[i]->GetName().c_str()));
      sortedEventNames.sort();
      // Insert the None option at the end of the list
      QStringList listPlusNone;
      listPlusNone.append(QString("<None>"));
      listPlusNone += sortedEventNames;
      temporaryEditControl->addItems(listPlusNone);

      connect(temporaryEditControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

      updateEditorFromModel(temporaryEditControl);

      // set the tooltip
      temporaryEditControl->setToolTip(getDescription());
      return temporaryEditControl;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGameEventControl::getDisplayName()
   {
      return QString(tr(myProperty->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGameEventControl::getDescription()
   {
      std::string tooltip = myProperty->GetDescription() + "  [Type: " + myProperty->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGameEventControl::getValueAsString()
   {
      return myProperty->GetValue() != NULL ? QString(myProperty->GetValue()->GetName().c_str()) : QString("<None>");
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
      if (temporaryEditControl != NULL) 
      {
         updateModelFromEditor(temporaryEditControl);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGameEventControl::updateData(QWidget *widget)
   {
      if (initialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGameEventControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
      dtCore::RefPtr<dtDAL::ActorProperty> property)
   {
      dtDAL::GameEventActorProperty *changedProp = dynamic_cast<dtDAL::GameEventActorProperty*>(property.get());

      if (temporaryEditControl != NULL && proxy == this->proxy && changedProp == myProperty) 
      {
         updateEditorFromModel(temporaryEditControl);
      }
   }
}
