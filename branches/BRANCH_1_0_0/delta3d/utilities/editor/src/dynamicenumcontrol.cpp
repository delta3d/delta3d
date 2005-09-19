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

#include <dtUtil/enumeration.h>
#include "dtEditQt/dynamicenumcontrol.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtUtil/log.h>
#include <QComboBox>
#include <QGridLayout>
#include <QIntValidator>
#include <QLabel>
#include <QWidget>

namespace dtEditQt
{

    //const QString DynamicBoolControl::TRUE_LABEL("True");
    //const QString DynamicBoolControl::FALSE_LABEL("False");

    ///////////////////////////////////////////////////////////////////////////////
    DynamicEnumControl::DynamicEnumControl()
        : temporaryEditControl(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicEnumControl::~DynamicEnumControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicEnumControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - Unlike the other properties, we can't static or reinterpret cast this object.
        // We need to dynamic cast it...
        if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::ENUMERATION)
        {
            myProperty = dynamic_cast<dtDAL::AbstractEnumActorProperty *>(newProperty);
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
    void DynamicEnumControl::updateEditorFromModel(QWidget *widget)
    {
        if (widget != NULL)
        {
            SubQComboBox *editor = static_cast<SubQComboBox *>(widget);

            // set the current value from our property
            dtUtil::Enumeration &value = myProperty->GetEnumValue();
            editor->setCurrentIndex(editor->findText(QString(value.GetName().c_str())));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicEnumControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;

        if (widget != NULL)
        {
            SubQComboBox *editor = static_cast<SubQComboBox *>(widget);

            // Get the current selected string and the previously set string value
            QString selection = editor->currentText();
            std::string selectionString = selection.toStdString();
            dtUtil::Enumeration &previousValue = myProperty->GetEnumValue();
            std::string previousString = previousValue.GetName();

            // set our value to our object
            if (previousString != selectionString)
            {
                // give undo manager the ability to create undo/redo events
                EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy,
                    myProperty->AsActorProperty(), previousString, selectionString);

                myProperty->SetValueFromString(selectionString);
                dataChanged = true;
            }
        }

        // notify the world (mostly the viewports) that our property changed
        if (dataChanged)
        {
            EditorEvents::getInstance().emitActorPropertyChanged(proxy,
                myProperty->AsActorProperty());
        }

        return dataChanged;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicEnumControl::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // create and init the combo box
        temporaryEditControl = new SubQComboBox(parent, this);

        if (!initialized)
        {
            LOG_ERROR("Tried to add itself to the parent widget before being initialized");
            return temporaryEditControl;
        }

        const std::vector<dtUtil::Enumeration*> &options = myProperty->GetList();
        std::vector<dtUtil::Enumeration*>::const_iterator iter;

        for (iter = options.begin(); iter != options.end(); iter++)
        {
            dtUtil::Enumeration *enumValue = (*iter);
            temporaryEditControl->addItem(QString(enumValue->GetName().c_str()));
        }

        connect(temporaryEditControl, SIGNAL(activated (int)), this, SLOT(itemSelected(int)));

        updateEditorFromModel(temporaryEditControl);

        // set the tooltip
        temporaryEditControl->setToolTip(getDescription());

        return temporaryEditControl;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicEnumControl::getDisplayName()
    {
        return QString(tr(myProperty->AsActorProperty()->GetLabel().c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicEnumControl::getDescription()
    {
        std::string tooltip = myProperty->AsActorProperty()->GetDescription() + "  [Type: " +
            myProperty->AsActorProperty()->GetPropertyType().GetName() + "]";
        return QString(tr(tooltip.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicEnumControl::getValueAsString()
    {
        dtUtil::Enumeration &value = myProperty->GetEnumValue();
        return QString(value.GetName().c_str());
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicEnumControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicEnumControl::itemSelected(int index) 
    {
        if (temporaryEditControl != NULL) 
        {
            updateModelFromEditor(temporaryEditControl);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicEnumControl::updateData(QWidget *widget)
    {
        if (!initialized || widget == NULL)
        {
            LOG_ERROR("Tried to updateData before being initialized");
            return false;
        }

        return updateModelFromEditor(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicEnumControl::actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        dtDAL::AbstractEnumActorProperty *changedProp = 
            dynamic_cast<dtDAL::AbstractEnumActorProperty *>(property.get());

        if (temporaryEditControl != NULL && proxy == this->proxy && changedProp == myProperty) 
        {
            updateEditorFromModel(temporaryEditControl);
        }
    }
}
