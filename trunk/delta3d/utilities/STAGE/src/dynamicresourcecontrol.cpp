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
#include <prefix/dtstageprefix-src.h>

#include <dtEditQt/dynamicresourcecontrol.h>
#include <dtEditQt/dynamiclabelcontrol.h>
#include <dtEditQt/dynamicsubwidgets.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/propertyeditortreeview.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/resourcedescriptor.h>

#include <dtUtil/log.h>

#include <QtCore/QSize>
#include <QtCore/QRect>

#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include <QtGui/QColor>
#include <QtGui/QPalette>
#include <QtGui/QFocusFrame>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicResourceControl::DynamicResourceControl()
      : mTemporaryWrapper(NULL)
      , mTemporaryEditOnlyTextLabel(NULL)
      , mTemporaryUseCurrentBtn(NULL)
      , mTemporaryClearBtn(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicResourceControl::~DynamicResourceControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::initializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::ActorProxy* newProxy, dtDAL::ActorProperty* newProperty)
   {
      // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
      // all cases in Linux with gcc4.  So we replaced it with a static cast.   
      if (newProperty != NULL && newProperty->GetDataType().IsResource()) 
      {
         myProperty = static_cast<dtDAL::ResourceActorProperty*>(newProperty);
         DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);
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
      return QString(tr(myProperty->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicResourceControl::getDescription() 
   {
      std::string tooltip = myProperty->GetDescription() + " - To assign a resource, select a [" + 
         myProperty->GetDataType().GetName() + "] resource in the Resource Browser and press Use Current";
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicResourceControl::getValueAsString() 
   {
      DynamicAbstractControl::getValueAsString();

      // if we have no current resource, show special text that indicates the type
      dtDAL::ResourceDescriptor* resource = myProperty->GetValue();
      QString resourceTag;
      if (resource == NULL) 
      {
         QString type;
         if (myProperty->GetDataType() == dtDAL::DataType::SOUND) 
         {
            type = dtDAL::DataType::SOUND.GetDisplayName().c_str();
         } 
         else if (myProperty->GetDataType() == dtDAL::DataType::STATIC_MESH) 
         {
            type = dtDAL::DataType::STATIC_MESH.GetDisplayName().c_str();
         } 
         else if (myProperty->GetDataType() == dtDAL::DataType::SKELETAL_MESH) 
         {
            type = dtDAL::DataType::SKELETAL_MESH.GetDisplayName().c_str();
         } 
         else if (myProperty->GetDataType() == dtDAL::DataType::TEXTURE) 
         {
            type = dtDAL::DataType::TEXTURE.GetDisplayName().c_str();
         } 
         else if (myProperty->GetDataType() == dtDAL::DataType::TERRAIN) 
         {
            type = dtDAL::DataType::TERRAIN.GetDisplayName().c_str();
         } 
         else if (myProperty->GetDataType() == dtDAL::DataType::PARTICLE_SYSTEM) 
         {
            type = dtDAL::DataType::PARTICLE_SYSTEM.GetDisplayName().c_str();
         } 
         else 
         {
            type = "Unknown Type";
         }

         resourceTag = "(NONE) - [" + type + "]";
      } 
      else 
      {
         resourceTag = QString(tr(resource->GetDisplayName().c_str()));
      }

      return resourceTag;
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::updateEditorFromModel(QWidget* widget)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicResourceControl::updateModelFromEditor(QWidget* widget)
   {
      return DynamicAbstractControl::updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicResourceControl::createEditor(QWidget* parent, 
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = new QWidget(parent);
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

      // label 
      mTemporaryEditOnlyTextLabel = new SubQLabel(getValueAsString(), wrapper, this);
      // set the background color to white so that it sort of blends in with the rest of the controls
      setBackgroundColor(mTemporaryEditOnlyTextLabel, PropertyEditorTreeView::ROW_COLOR_ODD);

      // Use Current button
      mTemporaryUseCurrentBtn = new SubQPushButton(tr("Use Current"), wrapper, this);
      // make sure it hold's it's min width.  This is a work around for a wierd QT behavior that 
      // allowed the button to get really tiny and stupid looking (had 'U' instead of 'Use Current')
      QSize size = mTemporaryUseCurrentBtn->sizeHint();
      mTemporaryUseCurrentBtn->setMaximumWidth(size.width());
      connect(mTemporaryUseCurrentBtn, SIGNAL(clicked()), this, SLOT(useCurrentPressed()));

      // Clear button
      mTemporaryClearBtn = new SubQPushButton(tr("Clear"), wrapper, this);
      size = mTemporaryClearBtn->sizeHint();
      mTemporaryClearBtn->setMaximumWidth(size.width());
      connect(mTemporaryClearBtn, SIGNAL(clicked()), this, SLOT(clearPressed()));
      std::string tooltip = myProperty->GetDescription() + " - Clears the current resource";
      mTemporaryClearBtn->setToolTip(QString(tr(tooltip.c_str())));

      grid->addWidget(mTemporaryEditOnlyTextLabel, 0, 0, 1, 1);
      grid->addWidget(mTemporaryUseCurrentBtn,     0, 1, 1, 1);
      grid->addWidget(mTemporaryClearBtn,          0, 2, 1, 1);
      grid->setColumnMinimumWidth(1, mTemporaryUseCurrentBtn->sizeHint().width());
      grid->setColumnMinimumWidth(2, mTemporaryClearBtn->sizeHint().width());
      grid->setColumnStretch(0, 2);
      grid->setColumnStretch(1, 1);
      grid->setColumnStretch(2, 0);

      mTemporaryUseCurrentBtn->setToolTip(getDescription());
      mTemporaryWrapper = wrapper;

      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicResourceControl::isEditable()
   {
      return !myProperty->IsReadOnly();
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor DynamicResourceControl::getCurrentResource() 
   {
      if (myProperty->GetDataType() == dtDAL::DataType::SOUND) 
      {
         return EditorData::GetInstance().getCurrentSoundResource();
      } 
      else if (myProperty->GetDataType() == dtDAL::DataType::STATIC_MESH) 
      {
         return EditorData::GetInstance().getCurrentMeshResource();
      } 
      else if (myProperty->GetDataType() == dtDAL::DataType::TEXTURE) 
      {
         return EditorData::GetInstance().getCurrentTextureResource();
      } 
      else if (myProperty->GetDataType() == dtDAL::DataType::TERRAIN) 
      {
         return EditorData::GetInstance().getCurrentTerrainResource();
      } 
      else if (myProperty->GetDataType() == dtDAL::DataType::PARTICLE_SYSTEM) 
      {
         return EditorData::GetInstance().getCurrentParticleResource();
      } 
      else if (myProperty->GetDataType() == dtDAL::DataType::SKELETAL_MESH) 
      {
         return EditorData::GetInstance().getCurrentSkeletalModelResource();
      } 
      else 
      {
         LOG_ERROR("Error setting current resource because DataType [" + 
            myProperty->GetDataType().GetName() + 
            "] is not supported for property [" + myProperty->GetName() + "].");
         // return something so we don't crash
         return dtDAL::ResourceDescriptor();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mTemporaryWrapper)
      {
         mTemporaryWrapper = NULL;
         mTemporaryEditOnlyTextLabel = NULL;
         mTemporaryUseCurrentBtn = NULL;
         mTemporaryClearBtn = NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::installEventFilterOnControl(QObject* filterObj)
   {
      if (mTemporaryEditOnlyTextLabel != NULL)
      {
         mTemporaryEditOnlyTextLabel->installEventFilter(filterObj);
      }
      if (mTemporaryUseCurrentBtn != NULL)
      {
         mTemporaryUseCurrentBtn->installEventFilter(filterObj);
      }
      if (mTemporaryClearBtn != NULL)
      {
         mTemporaryClearBtn->installEventFilter(filterObj);
      }
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
   void DynamicResourceControl::useCurrentPressed() 
   {
      // get the old and the new
      dtDAL::ResourceDescriptor* curResource = myProperty->GetValue();
      dtDAL::ResourceDescriptor newResource = getCurrentResource();
      bool isCurEmpty = (curResource == NULL || curResource->GetResourceIdentifier().empty());
      bool isNewEmpty = (newResource.GetResourceIdentifier().empty());

      // if different, than we make the change
      if (isCurEmpty != isNewEmpty || (curResource != NULL && !((*curResource) == newResource)))
      {
         std::string oldValue = myProperty->ToString();
         myProperty->SetValue(&newResource);

         // give undo manager the ability to create undo/redo events
         // technically, we're sending the about to change event AFTER we already 
         // changed it, but it doesn't matter.  It's the easiest way to get the string value.
         EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, myProperty, 
            oldValue, myProperty->ToString());

         // update our label
         if (mTemporaryEditOnlyTextLabel !=  NULL) 
         {
            mTemporaryEditOnlyTextLabel->setText(getValueAsString());
         }

         // notify the world (mostly the viewports) that our property changed
         EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, myProperty);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::clearPressed()
   {
      dtDAL::ResourceDescriptor* curResource = myProperty->GetValue();
      bool isCurEmpty = (curResource == NULL || curResource->GetResourceIdentifier().empty());

      if (!isCurEmpty) 
      {
         std::string oldValue = myProperty->ToString();
         myProperty->SetValue(NULL);

         // give undo manager the ability to create undo/redo events
         // technically, we're sending the about to change event AFTER we already 
         // changed it, but it doesn't matter.  It's the easiest way to get the string value.
         EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, myProperty, 
            oldValue, myProperty->ToString());

         // update our label
         if (mTemporaryEditOnlyTextLabel !=  NULL) 
         {
            mTemporaryEditOnlyTextLabel->setText(getValueAsString());
         }

         // notify the world (mostly the viewports) that our property changed
         EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, myProperty);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicResourceControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
      dtCore::RefPtr<dtDAL::ActorProperty> property)
   {
      DynamicAbstractControl::actorPropertyChanged(proxy, property);

      // update our label
      if (mTemporaryEditOnlyTextLabel != NULL && proxy == mProxy && property == myProperty) 
      {
         mTemporaryEditOnlyTextLabel->setText(getValueAsString());
      }
   }

} // namespace dtEditQt
