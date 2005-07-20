/*
* Delta3D Open Source Game and Simulation Engine Level Editor
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

#include "dtEditQt/dynamicboolcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/log.h"
#include <QComboBox>
#include <QGridLayout>
#include <QIntValidator>
#include <QLabel>
#include <QWidget>

namespace dtEditQt 
{

    const QString DynamicBoolControl::TRUE_LABEL("True");
    const QString DynamicBoolControl::FALSE_LABEL("False");

    ///////////////////////////////////////////////////////////////////////////////
    DynamicBoolControl::DynamicBoolControl()
        : temporaryEditControl(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicBoolControl::~DynamicBoolControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicBoolControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
        // all cases in Linux with gcc4.  So we replaced it with a static cast.   
        if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::BOOLEAN) 
        {
            myProperty = static_cast<dtDAL::BooleanActorProperty *>(newProperty);
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
    void DynamicBoolControl::updateEditorFromModel(QWidget *widget)
    {
        if (widget != NULL)
        {
            SubQComboBox *editor = static_cast<SubQComboBox *>(widget);

            // set the current value from our property
            bool value = myProperty->GetValue();
            editor->setCurrentIndex(editor->findText((value) ? TRUE_LABEL : FALSE_LABEL));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicBoolControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;

        if (widget != NULL) 
        {
            SubQComboBox *editor = static_cast<SubQComboBox *>(widget);

            // set the current value from our property
            QString selection = editor->currentText();
            bool result = (selection == TRUE_LABEL);

            // set our value to our object
            if (result != myProperty->GetValue()) 
            {
                std::string oldValue = myProperty->GetStringValue();
                myProperty->SetValue(result);
                // give undo manager the ability to create undo/redo events
                EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, myProperty,
                    oldValue, myProperty->GetStringValue());
                dataChanged = true;
            }
        }

        // notify the world (mostly the viewports) that our property changed
        if (dataChanged) 
        {
            EditorEvents::getInstance().emitActorPropertyChanged(proxy, myProperty);
        }

        return dataChanged;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicBoolControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // create and init the combo box
        temporaryEditControl = new SubQComboBox(parent, this);
        temporaryEditControl->addItem(TRUE_LABEL);
        temporaryEditControl->addItem(FALSE_LABEL);

        if (!initialized)  
        {
            LOG_ERROR("Tried to add itself to the parent widget before being initialized");
            return temporaryEditControl;
        }

        updateEditorFromModel(temporaryEditControl);

        // set the tooltip
        temporaryEditControl->setToolTip(getDescription());

        return temporaryEditControl;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicBoolControl::getDisplayName()
    {
        return QString(tr(myProperty->GetLabel().c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicBoolControl::getDescription() 
    {
        std::string tooltip = myProperty->GetDescription() + "  [Type: " + 
            myProperty->GetPropertyType().GetDisplayName() + "]";
        return QString(tr(tooltip.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicBoolControl::getValueAsString() 
    {
        bool value = myProperty->GetValue();
        return (value) ? TRUE_LABEL : FALSE_LABEL;
    }

    bool DynamicBoolControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicBoolControl::updateData(QWidget *widget)
    {
        if (!initialized || widget == NULL) 
        {
            LOG_ERROR("Tried to updateData before being initialized");
            return false;
        }

        return updateModelFromEditor(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicBoolControl::actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        if (temporaryEditControl != NULL && proxy == this->proxy && property == myProperty) 
        {
            updateEditorFromModel(temporaryEditControl);
        }
    }
}
