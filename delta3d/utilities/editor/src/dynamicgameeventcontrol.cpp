/* 
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc. 
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free 
* Software Foundation; either version 2 of the License, or (at your option) 
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
* details.
*
* You should have received a copy of the GNU General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
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
      if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::GAME_EVENT)
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
            temporaryEditControl->setCurrentIndex(temporaryEditControl->findText("None"));
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
         std::string previousString = myProperty->GetValue() != NULL ? myProperty->GetValue()->GetName() : "None";

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, myProperty, previousString, selectionString);

            dtDAL::GameEvent *eventToSet = NULL;
            std::vector<dtDAL::GameEvent*> events;
            dtDAL::Map &curMap = *EditorData::getInstance().getCurrentMap();
            curMap.GetEventManager().GetAllEvents(events);
            for(unsigned int i = 0; i < events.size(); i++)
            {
               if(events[i]->GetName() == selectionString)
               {  
                  eventToSet = events[i];
                  break;
               }                  
            }
            myProperty->SetValue(eventToSet);
            dataChanged = true;
         }
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         EditorEvents::getInstance().emitActorPropertyChanged(proxy, myProperty);
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
      dtDAL::Map &map = *EditorData::getInstance().getCurrentMap();
      map.GetEventManager().GetAllEvents(events);

      // Insert the None option at the end of the list
      temporaryEditControl->addItem(QString("None"));

      for(unsigned int i = 0; i < events.size(); i++)
         temporaryEditControl->addItem(QString(events[i]->GetName().c_str()));

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
      std::string tooltip = myProperty->GetDescription() + "  [Type: " + myProperty->GetPropertyType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGameEventControl::getValueAsString()
   {
      return myProperty->GetValue() != NULL ? QString(myProperty->GetValue()->GetName().c_str()) : QString("None");
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
