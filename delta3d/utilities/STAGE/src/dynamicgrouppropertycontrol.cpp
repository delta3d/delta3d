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
 * David Guthrie
 */
#include <prefix/stageprefix.h>

#include <dtEditQt/dynamicgrouppropertycontrol.h>

#include <dtDAL/datatype.h>
#include <dtDAL/groupactorproperty.h>

#include <dtEditQt/editordata.h>
#include <dtEditQt/groupuiregistry.h>
#include <dtEditQt/groupuiplugin.h>
#include <dtEditQt/mainwindow.h>

#include <dtQt/dynamicsubwidgets.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtUtil/log.h>

#include <QtGui/QHBoxLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicGroupPropertyControl::DynamicGroupPropertyControl()
      : mGroupProperty(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicGroupPropertyControl::~DynamicGroupPropertyControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupPropertyControl::addChildControl(dtQt::DynamicAbstractControl* child, dtQt::PropertyEditorModel* model)
   {
      // Note - if you change the propertyeditor so that it adds and removes rows instead of destroying
      // the property editor, you need to work with the begin/endinsertrows methods of model.
      if (child != NULL)
      {
         mChildren.push_back(child);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicGroupPropertyControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = new QWidget(parent);
      wrapper->setFocusPolicy(Qt::StrongFocus);
      // set the background color to white so that it sort of blends in with the rest of the controls
      SetBackgroundColor(wrapper, dtQt::PropertyEditorTreeView::ROW_COLOR_ODD);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      QGridLayout* grid = new QGridLayout(wrapper);
      grid->setMargin(0);
      grid->setSpacing(1);

      // Edit button
      dtQt::SubQPushButton* temporaryEditBtn = new dtQt::SubQPushButton(tr("Edit"), wrapper, this);
      connect(temporaryEditBtn, SIGNAL(clicked()), this, SLOT(EditClicked()));
      temporaryEditBtn->setToolTip(QString(tr("Edits this property with the configured editor plugin.")));

      grid->addWidget(temporaryEditBtn, 0, 0, 1, 1);

      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupPropertyControl::InitializeData(dtQt::DynamicAbstractControl* newParent,
      dtQt::PropertyEditorModel* newModel, dtDAL::PropertyContainer* propCon, dtDAL::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::GROUP)
      {
         mGroupProperty = dynamic_cast<dtDAL::GroupActorProperty*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, propCon, newProperty);
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property \"" +
            propertyName + "\" is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGroupPropertyControl::getDisplayName()
   {
      if (mGroupProperty == NULL)
      {
         return tr("");
      }
      return tr(mGroupProperty->GetLabel().c_str());
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGroupPropertyControl::getDescription()
   {
      if (mGroupProperty == NULL)
      {
         return tr("");
      }

      std::string tooltip = mGroupProperty->GetDescription() + "  [Type: " +
         mGroupProperty->GetDataType().GetName() + "]";
      return tr(tooltip.c_str());
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicGroupPropertyControl::getValueAsString()
   {
      if (mGroupProperty == NULL)
      {
         return tr("");
      }

      DynamicAbstractControl::getValueAsString();
      return QString(tr(mGroupProperty->GetValue()->GetName().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGroupPropertyControl::isEditable()
   {
      if (mGroupProperty == NULL)
      {
         return false;
      }

      return !mGroupProperty->IsReadOnly() &&
         //It's only editable if a plugin for the property's editor type is registered.
         GetPlugin() != NULL;
   }

   /////////////////////////////////////////////////////////////////////////////////
   GroupUIPlugin* DynamicGroupPropertyControl::GetPlugin()
   {
      if (mGroupProperty == NULL)
      {
         return NULL;
      }

      //TODO rip this out of editor data.
      return dynamic_cast<GroupUIPlugin*>(EditorData::GetInstance().GetGroupUIRegistry().GetPlugin(mGroupProperty->GetEditorType()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS                                                           //////////////
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicGroupPropertyControl::updateData(QWidget* widget)
   {
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicGroupPropertyControl::EditClicked()
   {
      if (mGroupProperty == NULL)
      {
         QMessageBox::critical(mPropertyTree,
            tr("Error"),tr("No Group Property is associated with this control.  An internal error has occurred."), QMessageBox::Ok, QMessageBox::Ok);
         return;
      }

      GroupUIPlugin* plugin = GetPlugin();
      QWidget* pluginWidget = plugin->CreateWidget(mPropertyTree);
      if (pluginWidget == NULL)
      {
         QMessageBox::critical(mPropertyTree,
            tr("Plugin Error"),tr("The plugin registered for this group actor property returned a NULL editor window."),
            QMessageBox::Ok, QMessageBox::Ok);
         return;
      }

      plugin->UpdateWidgetFromModel(*pluginWidget, *mGroupProperty->GetValue());

      QDialog* dialog = dynamic_cast<QDialog*>(pluginWidget);
      if (dialog != NULL)
      {
         dialog->setModal(true);
         dialog->exec();
         if (dialog->result() == QDialog::Accepted)
         {
            dtCore::RefPtr<dtDAL::NamedGroupParameter> param = new dtDAL::NamedGroupParameter(mGroupProperty->GetName());
            plugin->UpdateModelFromWidget(*pluginWidget, *param);
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mGroupProperty,
               mGroupProperty->ToString(), param->ToString());
            mGroupProperty->SetValue(*param);
            // notify the world (mostly the viewports) that our property changed
            emit PropertyChanged(*mPropContainer, *mGroupProperty);
         }
      }
      else
      {
         QMessageBox::critical(mPropertyTree,
            tr("Plugin Error"),tr("Non-QDialog group property plugin widgets are not yet supported."), QMessageBox::Ok, QMessageBox::Ok);
      }
   }

} // namespace dtEditQt
