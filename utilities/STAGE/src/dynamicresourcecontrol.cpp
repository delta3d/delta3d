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

#include <dtQt/dynamicsubwidgets.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtEditQt/groupuiregistry.h>
#include <dtEditQt/resourceuiplugin.h>
#include <dtEditQt/editordata.h>

#include <dtCore/project.h>
#include <dtCore/datatype.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/map.h>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>

#include <dtUtil/log.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   STAGEDynamicResourceControl::STAGEDynamicResourceControl()
      : dtQt::DynamicResourceControl()
      , mTemporaryEditBtn(NULL)
   {
      mNonResourceOptions.push_back("<Use Current>");
   }

   /////////////////////////////////////////////////////////////////////////////////
   STAGEDynamicResourceControl::~STAGEDynamicResourceControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget *STAGEDynamicResourceControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = dtQt::DynamicResourceControl::createEditor(parent, option, index);

      if (!IsInitialized())
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      // Edit button
      ResourceUIPlugin* plugin = GetPlugin();
      if (plugin && doPropertiesMatch())
      {
         mTemporaryEditBtn = new dtQt::SubQToolButton(wrapper, this);
         mTemporaryEditBtn->setText("Edit");
         mTemporaryEditBtn->setToolTip("Open the custom editor for this resource.");
         connect(mTemporaryEditBtn, SIGNAL(clicked()), this, SLOT(editPressed()));

         mGridLayout->addWidget(mTemporaryEditBtn, 0, 1, 1, 1);
         mGridLayout->setColumnMinimumWidth(1, mTemporaryEditBtn->sizeHint().width());
         mGridLayout->setColumnStretch(1, 0);
      }

      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void STAGEDynamicResourceControl::itemSelected(QAction* action)
   {
      // If we are using the currently selected resource from STAGE,
      // Set the data of the action to the current resource.
      if (action && action->text() == "<Use Current>")
      {
         dtCore::ResourceDescriptor current = EditorData::GetInstance().getCurrentResource(GetProperty().GetDataType());
         action->setData(QVariant(current.GetResourceIdentifier().c_str()));
      }

      DynamicResourceControl::itemSelected(action);
   }

   //////////////////////////////////////////////////////////////////////////
   void STAGEDynamicResourceControl::editPressed()
   {
      NotifyParentOfPreUpdate();

      if (GetProperty().GetDataType() == dtCore::DataType::UNKNOWN)
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
   }

   /////////////////////////////////////////////////////////////////////////////////
   void STAGEDynamicResourceControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTemporaryEditBtn = NULL;
      }

      dtQt::DynamicResourceControl::handleSubEditDestroy(widget, hint);
   }

   /////////////////////////////////////////////////////////////////////////////////
   ResourceUIPlugin* STAGEDynamicResourceControl::GetPlugin()
   {
      return dynamic_cast<ResourceUIPlugin*>(EditorData::GetInstance().GetGroupUIRegistry().GetPlugin(GetProperty().GetEditorType()));
   }
} // namespace dtEditQt
