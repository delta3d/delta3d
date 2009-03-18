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
#include <dtDAL/project.h>
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
   DynamicActorControl::DynamicActorControl()
      : myProperty(NULL)
      , myIdProperty(NULL)
      , mTemporaryWrapper(NULL)
      , mTemporaryEditControl(NULL)
      , mTemporaryGotoButton(NULL)
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
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::ACTOR)
      {
         myProperty = dynamic_cast<dtDAL::ActorActorProperty*>(newProperty);
         myIdProperty = dynamic_cast<dtDAL::ActorIDActorProperty*>(newProperty);
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
         dtDAL::ActorProxy* proxy = getActorProxy();
         if (proxy)
         {
            mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText(proxy->GetName().c_str()));
            return;
         }
         mTemporaryEditControl->setCurrentIndex(mTemporaryEditControl->findText("<None>"));
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicActorControl::updateModelFromEditor(QWidget *widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget != NULL)
      {
         //SubQComboBox *editor = static_cast<SubQComboBox*>(widget);

         // Get the current selected string and the previously set string value
         QString selection = mTemporaryEditControl->currentText();
         //unsigned int index = (unsigned int)(mTemporaryEditControl->currentIndex());
         std::string selectionString = selection.toStdString();
         
         dtDAL::ActorProxy* proxy = getActorProxy();
         std::string previousString = proxy? proxy->GetName() : "<None>";

         // set our value to our object
         if (previousString != selectionString)
         {
            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, getActorProperty(), previousString, selectionString);

            dtDAL::Map *curMap = EditorData::GetInstance().getCurrentMap();
            if(curMap == NULL)
               throw dtUtil::Exception(dtDAL::ExceptionEnum::MapException,
               "There is no map open, there shouldn't be any controls", __FILE__, __LINE__);
            

            // Find our matching proxy with this name - "<None>" ends up as NULl cause no match 
            std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
            std::string proxyClass;
            if (myProperty)
            {
               proxyClass = myProperty->GetDesiredActorClass();
            }
            else if (myIdProperty)
            {
               proxyClass = myIdProperty->GetDesiredActorClass();
            }
            GetActorProxies(proxies, proxyClass);
            dtDAL::ActorProxy *proxy = NULL;
            for(unsigned int i = 0; i < proxies.size(); i++)
            {
               if (proxies[i]->GetName().c_str() == selectionString)
               {
                  proxy = proxies[i].get();
                  break;
               }
            }

            if (myProperty)
            {
               myProperty->SetValue(proxy);
            }
            else if (myIdProperty)
            {
               myIdProperty->SetValue(proxy->GetId());
            }

            dataChanged = true;
         }
      }

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         EditorEvents::GetInstance().emitActorPropertyChanged(proxy, getActorProperty());
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
      std::string proxyClass;
      if (myProperty)
      {
         proxyClass = myProperty->GetDesiredActorClass();
      }
      else if (myIdProperty)
      {
         proxyClass = myIdProperty->GetDesiredActorClass();
      }
      GetActorProxies(names, proxyClass);

      // Insert the None option at the end of the list
      QStringList sortedNames;
      for(unsigned int i = 0; i < names.size(); i++)
         sortedNames.append(QString(names[i]->GetName().c_str()));
      sortedNames.sort();
      // Insert the None option at the end of the list
      QStringList listPlusNone;
      listPlusNone.append(QString("<None>"));
      listPlusNone += sortedNames;
      mTemporaryEditControl->addItems(listPlusNone);
      //mTemporaryEditControl->addItem(QString("<None>"));


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

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxy* DynamicActorControl::getActorProxy()
   {
      if (myProperty)
      {
         return myProperty->GetValue();
      }

      if (myIdProperty)
      {
         return myIdProperty->GetActorProxy();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* DynamicActorControl::getActorProperty()
   {
      if (myProperty)
      {
         return myProperty;
      }
      
      return myIdProperty;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicActorControl::getDisplayName()
   {
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

      dtDAL::ActorProxy* proxy = getActorProxy();
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
      DynamicAbstractControl::actorPropertyChanged(proxy, property);

      dtDAL::ActorActorProperty *changedProp = dynamic_cast<dtDAL::ActorActorProperty*>(property.get());

      if (mTemporaryEditControl != NULL && proxy == this->proxy && changedProp == getActorProperty()) 
      {
         updateEditorFromModel(mTemporaryEditControl);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicActorControl::onGotoClicked()
   {
      dtDAL::ActorProxy* proxy = getActorProxy();
      if(proxy != NULL)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> refProxy(proxy);
         
         EditorEvents::GetInstance().emitGotoActor(refProxy);

         std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > vec;
         vec.push_back(refProxy);

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
