/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2005, BMH Associates, Inc. 
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
* details.
*
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* @author Curtiss Murphy
*/

#include "dtEditQt/dynamiclabelcontrol.h"
#include "dtEditQt/dynamicresourcecontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/mainwindow.h"
#include "dtEditQt/propertyeditortreeview.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/log.h"
#include "dtDAL/resourcedescriptor.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QRect>
#include <QColor>
#include <QPalette>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicResourceControl::DynamicResourceControl()
        : temporaryEditOnlyTextLabel(NULL), temporaryUseCurrentBtn(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicResourceControl::~DynamicResourceControl()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicResourceControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
        // all cases in Linux with gcc4.  So we replaced it with a static cast.   
        if (newProperty != NULL && newProperty->GetPropertyType().IsResource()) 
        {
            myProperty = static_cast<dtDAL::ResourceActorProperty*>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);
        } 
        else 
        {
            std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
            std::string propType = (newProperty != NULL) ? newProperty->GetPropertyType().GetName() : "NULL";
            std::string isResource = (newProperty != NULL) ?
                (newProperty->GetPropertyType().IsResource() ? "IsResource" : "NotAResource") : "NULL";
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
            myProperty->GetPropertyType().GetName() + "] resource in the Resource Browser and press Use Current";
        return QString(tr(tooltip.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicResourceControl::getValueAsString() 
    {
        // if we have no current resource, show special text that indicates the type
        dtDAL::ResourceDescriptor *resource = myProperty->GetValue();
        QString resourceTag;
        if (resource == NULL) 
        {
            QString type;
            if (myProperty->GetPropertyType() == dtDAL::DataType::SOUND) 
            {
                type = dtDAL::DataType::SOUND.GetDisplayName().c_str();
            } 
            else if (myProperty->GetPropertyType() == dtDAL::DataType::STATIC_MESH) 
            {
                type = dtDAL::DataType::STATIC_MESH.GetDisplayName().c_str();
            } 
            else if (myProperty->GetPropertyType() == dtDAL::DataType::TEXTURE) 
            {
                type = dtDAL::DataType::TEXTURE.GetDisplayName().c_str();
            } 
            else if (myProperty->GetPropertyType() == dtDAL::DataType::CHARACTER) 
            {
                type = dtDAL::DataType::CHARACTER.GetDisplayName().c_str();
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
    void DynamicResourceControl::updateEditorFromModel(QWidget *widget)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicResourceControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;
        return false;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicResourceControl::createEditor(QWidget *parent, 
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

        // label 
        temporaryEditOnlyTextLabel = new SubQLabel(getValueAsString(), wrapper, this);
        // set the background color to white so that it sort of blends in with the rest of the controls
        setBackgroundColor(temporaryEditOnlyTextLabel, PropertyEditorTreeView::ROW_COLOR_ODD);

        // button
        temporaryUseCurrentBtn = new SubQPushButton(tr("Use Current"), wrapper, this);
        temporaryUseCurrentBtn->setMaximumHeight(18);
        connect(temporaryUseCurrentBtn, SIGNAL(clicked()), this, SLOT(useCurrentPressed()));
        // the button should get focus, not the wrapping widget
        wrapper->setFocusProxy(temporaryUseCurrentBtn);

        // setup the horizontal layout 
        hBox->addWidget(temporaryUseCurrentBtn);
        hBox->addSpacing(3);
        hBox->addWidget(temporaryEditOnlyTextLabel);
        hBox->addStretch(1);

        temporaryUseCurrentBtn->setToolTip(getDescription());

        return wrapper;
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicResourceControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    dtDAL::ResourceDescriptor DynamicResourceControl::getCurrentResource() 
    {
        if (myProperty->GetPropertyType() == dtDAL::DataType::SOUND) 
        {
            return EditorData::getInstance().getCurrentSoundResource();
        } 
        else if (myProperty->GetPropertyType() == dtDAL::DataType::STATIC_MESH) 
        {
            return EditorData::getInstance().getCurrentMeshResource();
        } 
        else if (myProperty->GetPropertyType() == dtDAL::DataType::TEXTURE) 
        {
            return EditorData::getInstance().getCurrentTextureResource();
        } 
        else if (myProperty->GetPropertyType() == dtDAL::DataType::CHARACTER) 
        {
            return EditorData::getInstance().getCurrentCharacterResource();
        } 
        else 
        {
            LOG_ERROR("Error setting current resource because DataType [" + 
                myProperty->GetPropertyType().GetName() + 
                "] is not supported for property [" + myProperty->GetName() + "].");
            // return something so we don't crash
            return dtDAL::ResourceDescriptor();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicResourceControl::handleSubEditDestroy(QWidget *widget)
    {
        temporaryEditOnlyTextLabel = NULL;
        temporaryUseCurrentBtn = NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicResourceControl::installEventFilterOnControl(QObject *filterObj)
    {
        if (temporaryEditOnlyTextLabel != NULL)
            temporaryEditOnlyTextLabel->installEventFilter(filterObj);
        if (temporaryUseCurrentBtn != NULL)
            temporaryUseCurrentBtn->installEventFilter(filterObj);
    }


    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicResourceControl::updateData(QWidget *widget)
    {
        // this guy doesn't have any editors.  All the data is edited in child controls
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicResourceControl::useCurrentPressed() 
    {
        // get the old and the new
        dtDAL::ResourceDescriptor *curResource = myProperty->GetValue();
        dtDAL::ResourceDescriptor newResource = getCurrentResource();
        bool isCurEmpty = (curResource == NULL || curResource->GetResourceIdentifier().empty());
        bool isNewEmpty = (newResource.GetResourceIdentifier().empty());

        // if different, than we make the change
        if (isCurEmpty != isNewEmpty || (curResource != NULL && !((*curResource) == newResource)))
        {

            std::string oldValue = myProperty->GetStringValue();
            myProperty->SetValue(&newResource);

            // give undo manager the ability to create undo/redo events
            EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, myProperty, 
                oldValue, myProperty->GetStringValue());

            // update our label
            if (temporaryEditOnlyTextLabel !=  NULL) 
            {
                temporaryEditOnlyTextLabel->setText(getValueAsString());
            }

            // notify the world (mostly the viewports) that our property changed
            EditorEvents::getInstance().emitActorPropertyChanged(proxy, myProperty);
        }
    }

}
