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

//#include <osg/Vec3>
#include "dtEditQt/dynamicvectorelementcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/log.h"
//#include <QGridLayout>
#include <QWidget>
//#include <QLabel>
//#include <QLineEdit>
#include <QDoubleValidator>


namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec2ActorProperty *newVectorProp, 
            int whichIndex, const std::string &newLabel)
            : label(newLabel), elementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop = newVectorProp;
        vec3Prop = NULL;
        vec4Prop = NULL;
        whichType = VEC2;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec3ActorProperty *newVectorProp, 
            int whichIndex, const std::string &newLabel)
            : label(newLabel), elementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop = NULL;
        vec3Prop = newVectorProp;
        vec4Prop = NULL;
        whichType = VEC3;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec4ActorProperty *newVectorProp, 
            int whichIndex, const std::string &newLabel)
            : label(newLabel), elementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop = NULL;
        vec3Prop = NULL;
        vec4Prop = newVectorProp;
        whichType = VEC4;
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::~DynamicVectorElementControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVectorElementControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

        // determine whether we have floats or doubles 
        // Doesn't matter whether this is a vec2, 3, or 4. 
        osg::Vec3 testVect;
        isVecFloat = (sizeof(testVect.x()) == sizeof(float));
        if (isVecFloat) {
            toolTipTypeLabel = "FLOAT";
        } else {
            toolTipTypeLabel = "DOUBLE";
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVectorElementControl::updateEditorFromModel(QWidget *widget)
    {
        if (widget != NULL)
        {
            // Note, don't use the temporary variable here.  It can cause errors with QT.
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);

            // set the current value from our property
            double value = getValue();
            QString strValue = QString::number(value, 'f', NUM_DECIMAL_DIGITS);
            editBox->setText(strValue);
            editBox->selectAll();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicVectorElementControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;

        if (widget != NULL) 
        {
            // Note, don't use the temporary variable here.  It can cause errors with QT.
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);
            bool success = false;
            float result = editBox->text().toFloat(&success);

            // set our value to our object
            if (success)
            {
                // Save the data if they are different.  Note, we also need to compare the QString value, 
                // else we get epsilon differences that cause the map to be marked dirty with no edits :(
                QString proxyValue = QString::number(getValue(), 'f', NUM_DECIMAL_DIGITS);
                QString newValue = editBox->text();
                if (result != getValue() && proxyValue != newValue)
                {
                    setValue(result);
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
        if (dataChanged) {
            if (whichType == VEC2) {
                EditorEvents::getInstance().emitActorPropertyChanged(proxy, vec2Prop);
            } else if (whichType == VEC3) {
                EditorEvents::getInstance().emitActorPropertyChanged(proxy, vec3Prop);
            } else { // == VEC4
                EditorEvents::getInstance().emitActorPropertyChanged(proxy, vec4Prop);
            }
        }

        return dataChanged;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicVectorElementControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // create and init the edit box
        //editBox = new QLineEdit(parent);
        temporaryEditControl = new SubQLineEdit (parent, this);
        QDoubleValidator *validator = new QDoubleValidator(temporaryEditControl);
        validator->setDecimals(NUM_DECIMAL_DIGITS);
        temporaryEditControl->setValidator(validator);

        if (!initialized)  {
            LOG_ERROR("Tried to add itself to the parent widget before being initialized");
            return temporaryEditControl;
        }

        updateEditorFromModel(temporaryEditControl);

        temporaryEditControl->setToolTip(getDescription());

        return temporaryEditControl;
    }


    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVectorElementControl::getDisplayName()
    {
        return QString(tr(label.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVectorElementControl::getDescription() 
    {
        std::string tooltip;

        if (whichType == VEC2) 
        {
            tooltip = vec2Prop->GetDescription();
        } 
        else if (whichType == VEC3) 
        {
            tooltip = vec3Prop->GetDescription();
        } 
        else 
        { // == VEC4
            tooltip = vec4Prop->GetDescription();
        }

        tooltip += " [Type: " + label;
        return QString(tr(tooltip.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVectorElementControl::getValueAsString() 
    {
        double value = getValue();
        return QString::number(value, 'f', NUM_DECIMAL_DIGITS);
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicVectorElementControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    double DynamicVectorElementControl::getValue()
    {
        double result = 0.0;

        if (whichType == VEC2) 
        {
            osg::Vec2 vectorValue = vec2Prop->GetValue();
            result = vectorValue[elementIndex];
        } 
        else if (whichType == VEC3) 
        {
            osg::Vec3 vectorValue = vec3Prop->GetValue();
            result = vectorValue[elementIndex];
        } 
        else 
        { // == VEC4
            osg::Vec4 vectorValue = vec4Prop->GetValue();
            result = vectorValue[elementIndex];
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVectorElementControl::setValue(double value)
    {
        if (whichType == VEC2) 
        {
            std::string oldValue = vec2Prop->GetStringValue();
            osg::Vec2 vectorValue = vec2Prop->GetValue();
            vectorValue[elementIndex] = value;
            vec2Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, vec2Prop, 
                oldValue, vec2Prop->GetStringValue());
        } 
        else if (whichType == VEC3) 
        {
            std::string oldValue = vec3Prop->GetStringValue();
            osg::Vec3 vectorValue = vec3Prop->GetValue();
            vectorValue[elementIndex] = value;
            vec3Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, vec3Prop, 
                oldValue, vec3Prop->GetStringValue());
        } 
        else 
        { // == VEC4
            std::string oldValue = vec4Prop->GetStringValue();
            osg::Vec4 vectorValue = vec4Prop->GetValue();
            vectorValue[elementIndex] = value;
            vec4Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, vec4Prop, 
                oldValue, vec4Prop->GetStringValue());
        }
    }


    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicVectorElementControl::updateData(QWidget *widget)
    {
        if (!initialized || widget == NULL)  
        {
            LOG_ERROR("Tried to updateData before being initialized");
            return false;
        }

        return updateModelFromEditor(widget);
    }

    void DynamicVectorElementControl::actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        if (temporaryEditControl != NULL && proxy == this->proxy) 
        {
            if (whichType == VEC2 && property == vec2Prop) 
            {
                updateEditorFromModel(temporaryEditControl);
            } 
            else if (whichType == VEC3 && property == vec3Prop) 
            {
                updateEditorFromModel(temporaryEditControl);
            } 
            else if (whichType == VEC4 && property == vec4Prop)
            {
                updateEditorFromModel(temporaryEditControl);
            }
        }
    }
}
