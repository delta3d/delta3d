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
* @author Curtiss Murphy
*/

#include "dtEditQt/dynamicstringcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/log.h"
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicStringControl::DynamicStringControl()
        : temporaryEditControl(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicStringControl::~DynamicStringControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicStringControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
        // all cases in Linux with gcc4.  So we replaced it with a static cast.   
        if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::STRING)
        {
            myProperty = static_cast<dtDAL::StringActorProperty *>(newProperty);
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
    void DynamicStringControl::updateEditorFromModel(QWidget *widget)
    {
        if (widget != NULL) 
        {
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);

            // set the current value from our property
            editBox->setText(tr(myProperty->GetValue().c_str()));
            editBox->selectAll();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicStringControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;

        if (widget != NULL) 
        {
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);
            bool success = false;

            // get the data from our control
            std::string result = editBox->text().toStdString();

            // set our value to our object
            if (result != myProperty->GetValue()) 
            {
                // give undo manager the ability to create undo/redo events
                EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, myProperty, 
                    myProperty->GetStringValue(), result);

                myProperty->SetValue(result);
                dataChanged = true;
            }

            // reselect all the text when we commit.  
            // Gives the user visual feedback that something happened.
            editBox->selectAll();
        }

        // notify the world (mostly the viewports) that our property changed
        if (dataChanged) 
        {
            EditorEvents::getInstance().emitActorPropertyChanged(proxy.get(), myProperty);
        }

        return dataChanged;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicStringControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // create and init the edit box
        temporaryEditControl = new SubQLineEdit (parent, this);
        if (myProperty->GetMaxLength() > 0)
            temporaryEditControl->setMaxLength(myProperty->GetMaxLength());

        if (!initialized) 
        {
            LOG_ERROR("Tried to add itself to the parent widget before being initialized");
            return temporaryEditControl;
        }

        updateEditorFromModel(temporaryEditControl);
        temporaryEditControl->setToolTip(getDescription());

        return temporaryEditControl;
    }

    const QString DynamicStringControl::getDisplayName()
    {
        return QString(tr(myProperty->GetLabel().c_str()));
    }

    const QString DynamicStringControl::getDescription() 
    {
        std::string tooltip = myProperty->GetDescription() + "  [Type: " + 
            myProperty->GetPropertyType().GetName() + "]";
        return QString(tr(tooltip.c_str()));
    }

    const QString DynamicStringControl::getValueAsString() 
    {
        return QString(tr(myProperty->GetValue().c_str()));
    }

    bool DynamicStringControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicStringControl::updateData(QWidget *widget)
    {
        // returns true if we successfully change data
        bool dataChanged = false;

        if (!initialized || widget == NULL) 
        {
            LOG_ERROR("Tried to updateData before being initialized");
            return dataChanged;
        }

        return updateModelFromEditor(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicStringControl::actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        if (temporaryEditControl != NULL && proxy == this->proxy && property == myProperty) 
        {
            updateEditorFromModel(temporaryEditControl);
        }
    }
}
