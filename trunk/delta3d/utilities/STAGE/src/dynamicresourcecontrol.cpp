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
 * Curtiss Murphy
 */
#include <prefix/stageprefix.h>

#include <dtEditQt/dynamicresourcecontrol.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/groupuiregistry.h>
#include <dtEditQt/resourceuiplugin.h>

#include <dtQt/dynamicsubwidgets.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtDAL/datatype.h>
#include <dtDAL/resourceactorproperty.h>

#include <QtGui/QMessageBox>
#include <QtGui/QGridLayout>

#include <dtUtil/log.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicResourceControl::DynamicResourceControl()
   : dtQt::DynamicResourceControlBase()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicResourceControl::~DynamicResourceControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicResourceControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicResourceControlBase::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      // Edit button
      ResourceUIPlugin* plugin = GetPlugin();
      if (plugin)
      {
         mTemporaryEditBtn = new dtQt::SubQPushButton(tr("Edit"), wrapper, this);
         mTemporaryEditBtn->setToolTip("Open the custom editor for this resource.");
         connect(mTemporaryEditBtn, SIGNAL(clicked()), this, SLOT(editPressed()));

         mGridLayout->addWidget(mTemporaryEditBtn, 0, 0, 1, 1);
         mGridLayout->setColumnMinimumWidth(0, mTemporaryEditBtn->sizeHint().width());
         mGridLayout->setColumnStretch(0, 0);
      }

      return wrapper;
   }

   //////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::editPressed()
   {
      NotifyParentOfPreUpdate();

      if (GetProperty().GetDataType() == dtDAL::DataType::UNKNOWN)
      {
         QMessageBox::critical(mPropertyTree,
            tr("Error"),tr("No Resource Property is associated with this control.  An internal error has occurred."), QMessageBox::Ok, QMessageBox::Ok);
         return;
      }

      ResourceUIPlugin* plugin = GetPlugin();
      plugin->SetResource(GetProperty().GetValue());
      QWidget* pluginWidget = plugin->CreateWidget(mPropertyTree);
      if (pluginWidget == NULL)
      {
         QMessageBox::critical(mPropertyTree,
            tr("Plugin Error"),tr("The plugin registered for this resource actor property returned a NULL editor window."),
            QMessageBox::Ok, QMessageBox::Ok);
         return;
      }

      connect(plugin, SIGNAL(ScriptChanged(const std::string&)), this, SLOT(onScriptChanged(const std::string&)));

      //QDialog* dialog = dynamic_cast<QDialog*>(pluginWidget);
      //if (dialog != NULL)
      //{
      //   dialog->setModal(true);
      //   dialog->exec();
      //   if (dialog->result() == QDialog::Accepted)
      //   {
      //      dtCore::RefPtr<dtDAL::NamedGroupParameter> param = new dtDAL::NamedGroupParameter(mGroupProperty->GetName());
      //      plugin->UpdateModelFromWidget(*pluginWidget, *param);
      //      // give undo manager the ability to create undo/redo events
      //      emit PropertyAboutToChange(*mPropContainer, *mGroupProperty,
      //         mGroupProperty->ToString(), param->ToString());
      //      mGroupProperty->SetValue(*param);
      //      // notify the world (mostly the viewports) that our property changed
      //      emit PropertyChanged(*mPropContainer, *mGroupProperty);
      //   }
      //}
      //else
      //{
      //   QMessageBox::critical(mPropertyTree,
      //      tr("Plugin Error"),tr("Non-QDialog group property plugin widgets are not yet supported."), QMessageBox::Ok, QMessageBox::Ok);
      //}
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTemporaryEditBtn = NULL;
      }

      DynamicResourceControlBase::handleSubEditDestroy(widget, hint);
   }

   /////////////////////////////////////////////////////////////////////////////////
   ResourceUIPlugin* DynamicResourceControl::GetPlugin()
   {
      return dynamic_cast<ResourceUIPlugin*>(EditorData::GetInstance().GetGroupUIRegistry().GetPlugin(GetProperty().GetEditorType()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor DynamicResourceControl::getCurrentResource()
   {
      if (GetProperty().GetDataType() == dtDAL::DataType::UNKNOWN)
      {
         LOG_ERROR("Error setting current resource because DataType [" +
                  GetProperty().GetDataType().GetName() +
                  "] is not supported for property [" + GetProperty().GetName() + "].");
      }

      return EditorData::GetInstance().getCurrentResource(GetProperty().GetDataType());
   }
} // namespace dtEditQt
