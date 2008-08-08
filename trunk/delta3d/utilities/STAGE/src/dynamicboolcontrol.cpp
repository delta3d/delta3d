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
#include <dtEditQt/dynamicboolcontrol.h>
#include <dtEditQt/dynamicsubwidgets.h>
#include <dtEditQt/editorevents.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

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
        if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::BOOLEAN) 
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
                std::string oldValue = myProperty->ToString();
                myProperty->SetValue(result);
                // give undo manager the ability to create undo/redo events
                EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, myProperty,
                    oldValue, myProperty->ToString());
                dataChanged = true;
            }
        }

        // notify the world (mostly the viewports) that our property changed
        if (dataChanged) 
        {
            EditorEvents::GetInstance().emitActorPropertyChanged(proxy, myProperty);
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

        connect(temporaryEditControl, SIGNAL(activated (int)), this, SLOT(itemSelected(int)));

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
        return !myProperty->IsReadOnly();
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicBoolControl::itemSelected(int index) 
    {
        if (temporaryEditControl != NULL) 
        {
            updateModelFromEditor(temporaryEditControl);
        }
    }

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
    void DynamicBoolControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
        dtCore::RefPtr<dtDAL::ActorProperty> property)
    {
        if (temporaryEditControl != NULL && proxy == this->proxy && property == myProperty) 
        {
            updateEditorFromModel(temporaryEditControl);
        }
    }
}
