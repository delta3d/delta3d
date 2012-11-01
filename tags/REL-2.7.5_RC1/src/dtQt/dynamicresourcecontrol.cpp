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
#include <prefix/dtqtprefix.h>
#include <dtQt/dynamicresourcecontrol.h>

#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>
#include <dtCore/project.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/resourceactorproperty.h>

#include <dtQt/dynamiclabelcontrol.h>
#include <dtQt/propertyeditortreeview.h>
#include <dtQt/resourceselectorwidget.h>

#include <dtUtil/log.h>

#include <QtCore/QRect>
#include <QtCore/QSize>

#include <QtGui/QColor>
#include <QtGui/QFocusFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPalette>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include <QtGui/QMenu>
#include <QtGui/QAction>


namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicResourceControl::DynamicResourceControl()
      : mTemporaryButton(NULL)
   {
      mNonResourceOptions.push_back("<None>");
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicResourceControl::~DynamicResourceControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in
      // all cases in Linux with gcc4.  So we replaced it with a static cast.
      if (newProperty != NULL && newProperty->GetDataType().IsResource())
      {
         mProperty = static_cast<dtCore::ResourceActorProperty*>(newProperty);
         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         std::string propType     = (newProperty != NULL) ? newProperty->GetDataType().GetName() : "NULL";
         std::string isResource   = (newProperty != NULL) ?
            (newProperty->GetDataType().IsResource() ? "IsResource" : "NotAResource") : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type[" + propType + "], [" +
            isResource + "].");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicResourceControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return QString(tr(mProperty->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicResourceControl::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + " - To assign a resource, select a [" +
         mProperty->GetDataType().GetName() + "] resource in the Resource Browser and press Use Current";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicResourceControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      if (doPropertiesMatch())
      {
         // if we have no current resource, show special text that indicates the type
         dtCore::ResourceDescriptor resource = mProperty->GetValue();
         QString resourceTag;
         if (resource.IsEmpty())
         {
            resourceTag = QString(tr("<None>"));
         }
         else
         {
            resourceTag = QString(tr(resource.GetDisplayName().c_str()));
         }

         return resourceTag;
      }
      else
      {
         return "<Multiple Values...>";
      }
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::updateEditorFromModel(QWidget* widget)
   {
      // update our label
      if (widget == mWrapper && mTemporaryButton)
      {
         mTemporaryButton->SetResource(getValueAsString());
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicResourceControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicResourceControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = DynamicAbstractControl::createEditor(parent, option, index);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      mTemporaryButton = new ResourceSelectorWidget(wrapper);
      mTemporaryButton->SetResourceType(mNonResourceOptions, mProperty->GetDataType());
      mTemporaryButton->setToolTip(getDescription());
      mTemporaryButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      connect(mTemporaryButton, SIGNAL(ResourceSelected(QAction*)), this, SLOT(itemSelected(QAction*)));

      updateEditorFromModel(mWrapper);

      mGridLayout->addWidget(mTemporaryButton, 0, 0, 1, 1);
      mGridLayout->setColumnMinimumWidth(0, mTemporaryButton->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 1);

      wrapper->setFocusProxy(mTemporaryButton);
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceActorProperty& DynamicResourceControl::GetProperty()
   {
      return *mProperty;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicResourceControl::updateData(QWidget* widget)
   {
      // this guy doesn't have any editors.  All the data is edited in child controls
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::itemSelected(QAction* action)
   {
      NotifyParentOfPreUpdate();
      if (action)
      {
         QString qResult = action->data().toString();
         std::string result = qResult.toStdString();

         if (result != mProperty->GetValue().GetResourceIdentifier())
         {
            // give undo manager the ability to create undo/redo events
            emit PropertyAboutToChange(*mPropContainer, *mProperty,
               mProperty->ToString(), result);

            mProperty->FromString(result);

            emit PropertyChanged(*mPropContainer, *mProperty);
         }

         CopyBaseValueToLinkedProperties();

         // Tell our resource selector what resource was selected
         if (qResult.isEmpty())
         {
            qResult = "<None>";
         }
         mTemporaryButton->SetResource(qResult);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTemporaryButton = NULL;
      }

      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }
} // namespace dtEditQt
