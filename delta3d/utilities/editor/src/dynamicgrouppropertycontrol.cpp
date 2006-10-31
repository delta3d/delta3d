/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation 
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
 * David Guthrie
 */
#include <prefix/dtstageprefix-src.h>

#include <dtEditQt/dynamicgrouppropertycontrol.h>
#include <dtEditQt/propertyeditormodel.h>
#include <dtEditQt/propertyeditortreeview.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/groupuiregistry.h>
#include <dtEditQt/dynamicsubwidgets.h>

#include <dtDAL/groupactorproperty.h>
#include <dtUtil/log.h>

#include <QtGui/QWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>

namespace dtEditQt 
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicGroupPropertyControl::DynamicGroupPropertyControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicGroupPropertyControl::~DynamicGroupPropertyControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupPropertyControl::addChildControl(DynamicAbstractControl *child, PropertyEditorModel *model)
   {
      // Note - if you change the propertyeditor so that it adds and removes rows instead of destroying
      // the property editor, you need to work with the begin/endinsertrows methods of model.
      if (child != NULL) 
      {
         children.push_back(child);
      }
   }
   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicGroupPropertyControl::createEditor(QWidget *parent, 
      const QStyleOptionViewItem &option, const QModelIndex &index)
   {
      QWidget *wrapper = new QWidget(parent);
      // set the background color to white so that it sort of blends in with the rest of the controls
      setBackgroundColor(wrapper, PropertyEditorTreeView::ROW_COLOR_ODD);

      if (!initialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      QHBoxLayout *hBox = new QHBoxLayout(wrapper);
      hBox->setMargin(0);
      hBox->setSpacing(0);

      // Use Current button
      SubQPushButton* temporaryEditBtn = new SubQPushButton(tr("Edit"), wrapper, this);
      temporaryEditBtn->setMaximumHeight(18);
      connect(temporaryEditBtn, SIGNAL(clicked()), this, SLOT(EditClicked()));
      temporaryEditBtn->setToolTip(QString(tr("Edits this property with the configured editor plugin.")));
      // the button should get focus, not the wrapping widget
      wrapper->setFocusProxy(temporaryEditBtn);

      // setup the horizontal layout 
      hBox->addWidget(temporaryEditBtn);

      return wrapper;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupPropertyControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
   {
      DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);
      mGroupProperty = dynamic_cast<dtDAL::GroupActorProperty*>(newProperty);
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGroupPropertyControl::getDisplayName()
   {
      if (mGroupProperty == NULL)
         return tr("");    
      return tr(mGroupProperty->GetLabel().c_str());
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGroupPropertyControl::getDescription() 
   {
      if (mGroupProperty == NULL)
         return tr("");    

      std::string tooltip = mGroupProperty->GetDescription() + "  [Type: " + 
         mGroupProperty->GetPropertyType().GetName() + "]";
      return tr(tooltip.c_str());
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGroupPropertyControl::getValueAsString() 
   {
      if (mGroupProperty == NULL)
         return tr("");    

      return QString(tr(mGroupProperty->GetValue()->GetName().c_str()));
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGroupPropertyControl::isEditable()
   {
      if (mGroupProperty == NULL)
         return false;    

      return !mGroupProperty->IsReadOnly() &&
         //It's only editable if a plugin for the property's editor type is registered.
         EditorData::getInstance().GetGroupUIRegistry().GetPlugin(mGroupProperty->GetEditorType()) != NULL;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS                                                           //////////////
   /////////////////////////////////////////////////////////////////////////////////
   
   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGroupPropertyControl::updateData(QWidget *widget)
   {
      return true;
   }
      
   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupPropertyControl::EditClicked()
   {
      
   }
   
}
