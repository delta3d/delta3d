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
#include "dtEditQt/dynamicactorcontrol.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"

#include <osg/ref_ptr>
#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <QPushButton>
#include <QHBoxLayout>

namespace dtEditQt
{
   DynamicActorControl::DynamicActorControl() 
   {
       
   }

   DynamicActorControl::~DynamicActorControl()
   {
      
   }

   void DynamicActorControl::initializeData(DynamicAbstractControl *newParent,
      PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
   {
      // Note - Unlike the other properties, we can't static or reinterpret cast this object.
      // We need to dynamic cast it...
      if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::ACTOR)
      {
         myProperty = dynamic_cast<dtDAL::ActorActorProperty*>(newProperty);
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
   void DynamicActorControl::updateEditorFromModel(QWidget *widget)
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
   bool DynamicActorControl::updateModelFromEditor(QWidget *widget)
   {
      bool dataChanged = false;

      if (widget != NULL)
      {
         //SubQComboBox *editor = static_cast<SubQComboBox*>(widget);

         // Get the current selected string and the previously set string value
         QString selection = temporaryEditControl->currentText();
         int index = temporaryEditControl->findText(selection);
         std::string selectionString = selection.toStdString();
         std::string previousString = myProperty->GetValue() != NULL ? myProperty->GetValue()->GetName() : "None";

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, myProperty, previousString, selectionString);

            osg::ref_ptr<dtDAL::Map> curMap = EditorData::getInstance().getCurrentMap();
            if(!curMap.valid())
               EXCEPT(dtDAL::ExceptionEnum::MapException, "There is no map open, there shouldn't be any controls");
            
            std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxies = GetActorProxies(myProperty->GetDesiredActorClass());

            myProperty->SetValue((unsigned int)index < proxies.size() ? proxies[index].get() : NULL);
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
   QWidget* DynamicActorControl::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index)
   {
      // create and init the combo box
      QWidget *wrapper = new QWidget(parent);

      if (!initialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      QHBoxLayout *hLayout = new QHBoxLayout(wrapper);
      hLayout->setMargin(0);
      hLayout->setSpacing(0);

      temporaryEditControl = new SubQComboBox(wrapper, this);

      temporaryGotoButton = new SubQPushButton(tr("Goto"), wrapper, this);
      temporaryGotoButton->setMaximumHeight(18);

      connect(temporaryGotoButton, SIGNAL(clicked()), this, SLOT(onGotoClicked()));
      
      wrapper->setFocusProxy(temporaryGotoButton);

      std::vector<osg::ref_ptr<dtDAL::ActorProxy> > names = GetActorProxies(myProperty->GetDesiredActorClass());

      for(unsigned int i = 0; i < names.size(); i++)
         temporaryEditControl->addItem(QString(names[i]->GetName().c_str()));

      // Insert the None option at the end of the list
      temporaryEditControl->addItem(QString("None"));

      connect(temporaryEditControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

      updateEditorFromModel(temporaryEditControl);

      // set the tooltip
      temporaryEditControl->setToolTip(getDescription());

      hLayout->addWidget(temporaryEditControl);
      hLayout->setSpacing(1);
      hLayout->addWidget(temporaryGotoButton);
      
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getDisplayName()
   {
      return QString(tr(myProperty->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getDescription()
   {
      std::string tooltip = myProperty->GetDescription() + "  [Type: " + myProperty->GetPropertyType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getValueAsString()
   {
      return myProperty->GetValue() != NULL ? QString(myProperty->GetValue()->GetName().c_str()) : QString("None");
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
      if (temporaryEditControl != NULL) 
      {
         updateModelFromEditor(temporaryEditControl);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicActorControl::updateData(QWidget *widget)
   {
      if (initialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
      osg::ref_ptr<dtDAL::ActorProperty> property)
   {
      dtDAL::ActorActorProperty *changedProp = dynamic_cast<dtDAL::ActorActorProperty*>(property.get());

      if (temporaryEditControl != NULL && proxy == this->proxy && changedProp == myProperty) 
      {
         updateEditorFromModel(temporaryEditControl);
      }
   }

   void DynamicActorControl::onGotoClicked()
   {
      if(myProperty->GetValue() != NULL)
      {
         osg::ref_ptr<dtDAL::ActorProxy> proxy(myProperty->GetValue());
         
         EditorEvents::getInstance().emitGotoActor(proxy);

         std::vector<osg::ref_ptr<dtDAL::ActorProxy> > vec;
         vec.push_back(proxy);

         EditorEvents::getInstance().emitActorsSelected(vec);
      }
   }

   std::vector<osg::ref_ptr<dtDAL::ActorProxy> > DynamicActorControl::GetActorProxies(const std::string &className)
   {
      std::vector<osg::ref_ptr<dtDAL::ActorProxy> > toFill; 

      osg::ref_ptr<dtDAL::Map> curMap = EditorData::getInstance().getCurrentMap();

      if(!curMap.valid())
         EXCEPT(dtDAL::ExceptionEnum::MapException, "There is no map open, there shouldn't be any controls");
            
      curMap->FindProxies(toFill, "", "", "", className);

      return toFill;  
   }
}