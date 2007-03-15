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
 * William E. Johnson II
 */
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/dynamicactorcontrol.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/propertyeditortreeview.h>
#include <dtDAL/map.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

namespace dtEditQt
{
   DynamicActorControl::DynamicActorControl():
      myProperty(NULL), 
      mTemporaryWrapper(NULL), 
      mTemporaryEditControl(NULL), 
      mTemporaryGotoButton(NULL)  
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
           mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText(myProperty->GetValue()->GetName().c_str()));
         else
            mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText("None"));
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
         QString selection = mTemporaryEditControl->currentText();
         unsigned int index = (unsigned int)(mTemporaryEditControl->currentIndex());
         std::string selectionString = selection.toStdString();
         std::string previousString = myProperty->GetValue() != NULL ? myProperty->GetValue()->GetName() : "None";

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, myProperty, previousString, selectionString);

            dtDAL::Map *curMap = EditorData::GetInstance().getCurrentMap();
            if(curMap == NULL)
               throw dtUtil::Exception(dtDAL::ExceptionEnum::MapException,
               "There is no map open, there shouldn't be any controls", __FILE__, __LINE__);
            
            std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
            GetActorProxies(proxies, myProperty->GetDesiredActorClass());

            if (index == 0)
            {
               myProperty->SetValue(NULL);
            }
            else
            {
               if(index <= proxies.size())
               {
                  dtDAL::ActorProxy *proxy = proxies[index - 1].get();
                  myProperty->SetValue(proxy);
               }
               else
               {
                  myProperty->SetValue(NULL);
               }
            }

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
   QWidget* DynamicActorControl::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index)
   {
      QWidget *wrapper = new QWidget(parent);
      wrapper->setFocusPolicy(Qt::StrongFocus);
      // set the background color to white so that it sort of blends in with the rest of the controls
      setBackgroundColor(wrapper, PropertyEditorTreeView::ROW_COLOR_ODD);

      if (!initialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      QGridLayout* grid = new QGridLayout(wrapper);
      grid->setMargin(0);
      grid->setSpacing(1);

      mTemporaryEditControl = new SubQComboBox(wrapper, this);

      mTemporaryGotoButton = new SubQPushButton(tr("Goto"), wrapper, this);

      connect(mTemporaryGotoButton, SIGNAL(clicked()), this, SLOT(onGotoClicked()));
      
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > names;
      GetActorProxies(names, myProperty->GetDesiredActorClass());

      // Insert the None option at the end of the list
      mTemporaryEditControl->addItem(QString("None"));

      for(unsigned int i = 0; i < names.size(); i++)
         mTemporaryEditControl->addItem(QString(names[i]->GetName().c_str()));

      connect(mTemporaryEditControl, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));

      updateEditorFromModel(mTemporaryEditControl);

      // set the tooltip
      mTemporaryEditControl->setToolTip(getDescription());

      grid->addWidget(mTemporaryEditControl, 0, 0, 1, 1);
      grid->addWidget(mTemporaryGotoButton, 0, 1, 1, 1);
      grid->setColumnMinimumWidth(1, mTemporaryGotoButton->sizeHint().width() / 2);
      grid->setColumnStretch(0, 2);
      
      mTemporaryWrapper = wrapper;
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getDisplayName()
   {
      return tr(myProperty->GetLabel().c_str());
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getDescription()
   {
      std::string tooltip = myProperty->GetDescription() + "  [Type: " + myProperty->GetPropertyType().GetName() + "]";
      return tr(tooltip.c_str());
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
      if (mTemporaryEditControl != NULL) 
      {
         updateModelFromEditor(mTemporaryEditControl);
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
   void DynamicActorControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
      dtCore::RefPtr<dtDAL::ActorProperty> property)
   {
      dtDAL::ActorActorProperty *changedProp = dynamic_cast<dtDAL::ActorActorProperty*>(property.get());

      if (mTemporaryEditControl != NULL && proxy == this->proxy && changedProp == myProperty) 
      {
         updateEditorFromModel(mTemporaryEditControl);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::onGotoClicked()
   {
      if(myProperty->GetValue() != NULL)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> proxy(myProperty->GetValue());
         
         EditorEvents::GetInstance().emitGotoActor(proxy);

         std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > vec;
         vec.push_back(proxy);

         EditorEvents::GetInstance().emitActorsSelected(vec);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::GetActorProxies(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill, const std::string &className)
   {
      toFill.clear();

      dtCore::RefPtr<dtDAL::Map> curMap = EditorData::GetInstance().getCurrentMap();

      if(!curMap.valid())
         throw dtUtil::Exception(dtDAL::ExceptionEnum::MapException, 
         "There is no map open, there shouldn't be any controls", __FILE__, __LINE__);
            
      curMap->FindProxies(toFill, "", "", "", className);

   }
}
