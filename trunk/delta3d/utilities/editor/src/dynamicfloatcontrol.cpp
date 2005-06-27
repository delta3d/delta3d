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

#include "dtEditQt/dynamicfloatcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/log.h"
#include <QWidget>
#include <QLineEdit>
#include <QDoubleValidator>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicFloatControl::DynamicFloatControl()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicFloatControl::~DynamicFloatControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicFloatControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
        // all cases in Linux with gcc4.  So we replaced it with a static cast.   
        if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::FLOAT) 
        {
            myProperty = static_cast<dtDAL::FloatActorProperty *>(newProperty);
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
    void DynamicFloatControl::updateEditorFromModel(QWidget *widget)
    {
        if (widget != NULL) 
        {
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);

            // set the current value from our property
            float floatValue = myProperty->GetValue();
            editBox->setText(QString::number(floatValue, 'f', NUM_DECIMAL_DIGITS));
            editBox->selectAll();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicFloatControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;

        if (widget != NULL) 
        {
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);
            bool success = false;
            float result = editBox->text().toFloat(&success);

            // set our value to our object
            if (success) {
                if (result != myProperty->GetValue()) 
                {
                    // give undo manager the ability to create undo/redo events
                    EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, myProperty, 
                        myProperty->GetStringValue(), QString::number(result).toStdString());

                    myProperty->SetValue(result);
                    dataChanged = true;
                }
            } 
            else 
            {
                LOG_ERROR("updateData() failed to convert our value successfully");
            }

            // reselect all the text when we commit.  
            // Gives the user visual feedback that something happened.
            editBox->selectAll();
        }

        // notify the world (mostly the viewports) that our property changed
        if (dataChanged) 
        {
            EditorEvents::getInstance().emitActorPropertyChanged(proxy, myProperty);
        }

        return dataChanged;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicFloatControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // create and init the edit box
        //editBox = new QLineEdit(parent);
        SubQLineEdit *editBox = new SubQLineEdit (parent, this);
        QDoubleValidator *validator = new QDoubleValidator(editBox);
        validator->setDecimals(NUM_DECIMAL_DIGITS);
        editBox->setValidator(validator);

        if (!initialized)  
        {
            LOG_ERROR("Tried to add itself to the parent widget before being initialized");
            return editBox;
        }

        updateEditorFromModel(editBox);

        editBox->setToolTip(getDescription());

        return editBox;
    }

    const QString DynamicFloatControl::getDisplayName()
    {
        return QString(tr(myProperty->GetLabel().c_str()));
    }

    const QString DynamicFloatControl::getDescription() 
    {
        std::string tooltip = myProperty->GetDescription() + "  [Type: " + 
            myProperty->GetPropertyType().GetName() + "]";
        return QString(tr(tooltip.c_str()));
    }

    const QString DynamicFloatControl::getValueAsString() 
    {
        float floatValue = myProperty->GetValue();
        return QString::number(floatValue, 'f', NUM_DECIMAL_DIGITS);
    }

    bool DynamicFloatControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicFloatControl::updateData(QWidget *widget)
    {
        if (!initialized || widget == NULL)  
        {
            LOG_ERROR("Tried to updateData before being initialized");
            return false;
        }

        return updateModelFromEditor(widget);
    }
}
