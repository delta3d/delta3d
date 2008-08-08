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

//#include <osg/Vec3>
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/dynamiccolorelementcontrol.h>
#include <dtEditQt/dynamicsubwidgets.h>
#include <dtEditQt/editorevents.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>
#include <QtGui/QWidget>


namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicColorElementControl::DynamicColorElementControl(dtDAL::ColorRgbaActorProperty *newColorRGBA, 
            int whichIndex, const std::string &newLabel)
            : label(newLabel), elementIndex(whichIndex), temporaryEditControl(NULL)
    {

        //colorRGB = NULL;
        colorRGBA = newColorRGBA;
        whichType = RGBA;
    }

    ///////////////////////////////////////////////////////////////////////////////
    //DynamicColorElementControl::DynamicColorElementControl(dtDAL::ColorRgbActorProperty *colorRGB, 
    //        int whichIndex, const std::string &newLabel)
    //        : label(newLabel), elementIndex(whichIndex)
    //{

    //    colorRGB = colorRGBA;
    //    colorRGBA = NULL;
    //    whichType = RGB;
    //}

    /////////////////////////////////////////////////////////////////////////////////
    DynamicColorElementControl::~DynamicColorElementControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorElementControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // NOTE - This object sets it's property in the contructor.
        DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorElementControl::updateEditorFromModel(QWidget *widget)
    {
        if (widget != NULL) {
            SubQSpinBox *control = static_cast<SubQSpinBox *>(widget);

            // set the current value from our property
            int value = getValue();
            control->setValue(value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicColorElementControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;

        if (widget != NULL) {
            SubQSpinBox *control = static_cast<SubQSpinBox *>(widget);
            int result = control->value();

            // set our value to our object
            if (result != getValue()) {
                setValue(result);
                dataChanged = true;
            } else {
                LOG_ERROR("updateData() failed to convert our value successfully");
            }
        }

        // notify the world (mostly the viewports) that our property changed
        if (dataChanged) {
            if (whichType == RGBA) {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, colorRGBA);
            //} else if (whichType == RGB) {
            //    EditorEvents::GetInstance().emitActorPropertyChanged(proxy, colorRGB);
            }
        }

        return dataChanged;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicColorElementControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // create and init the edit box
        temporaryEditControl = new SubQSpinBox(parent, this);
        temporaryEditControl->setMinimum(0);
        temporaryEditControl->setMaximum(255);
        //control->

        if (!initialized)  {
            LOG_ERROR("Tried to add itself to the parent widget before being initialized");
            return temporaryEditControl;
        }

        updateEditorFromModel(temporaryEditControl);
        temporaryEditControl->setToolTip(getDescription());

        return temporaryEditControl;
    }


    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicColorElementControl::getDisplayName()
    {
        return QString(tr(label.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicColorElementControl::getDescription() 
    {
        std::string tooltip;

        if (whichType == RGBA)
        {
            tooltip = colorRGBA->GetDescription();
        }// else { // == RGB
        //    tooltip = colorRGB->getDescription();
        //}

        tooltip += " [Type: " + label;
        return QString(tr(tooltip.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicColorElementControl::getValueAsString() 
    {
        int value = getValue();
        return QString::number(value);
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicColorElementControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    int DynamicColorElementControl::getValue()
    {
        int result = 0;

        if (whichType == RGBA)
        {
            osg::Vec4 vectorValue = colorRGBA->GetValue();
            double intermediate = (double) vectorValue[elementIndex];
            result = DynamicColorElementControl::convertColorFloatToInt(intermediate);
        }
        //else { // == RGB
        //    osg::Vec3 vectorValue = colorRGB->getValue();
        //    result = vectorValue[elementIndex];
        //}

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorElementControl::setValue(int value)
    {
        if (whichType == RGBA) 
        {
            std::string oldValue = colorRGBA->ToString();
            osg::Vec4 vectorValue = colorRGBA->GetValue();
            double intermediate = DynamicColorElementControl::convertColorIntToFloat(value);
            vectorValue[elementIndex] = (double) intermediate;
            colorRGBA->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, colorRGBA,
                oldValue, colorRGBA->ToString());
        }
        //else { // == RGB
        //    osg::Vec3 vectorValue = colorRGB->getValue();
        //    vectorValue[elementIndex] = value;
        //    colorRGB->setValue(vectorValue);
        //}
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicColorElementControl::updateData(QWidget *widget)
    {
        if (!initialized || widget == NULL)  
        {
            LOG_ERROR("Tried to updateData before being initialized");
            return false;
        }

        return updateModelFromEditor(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorElementControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
        dtCore::RefPtr<dtDAL::ActorProperty> property)
    {
        if (temporaryEditControl != NULL && proxy == this->proxy && property == colorRGBA) 
        {
            updateEditorFromModel(temporaryEditControl);
        }
    }

    // STATIC methods

    /////////////////////////////////////////////////////////////////////////////////
    int DynamicColorElementControl::convertColorFloatToInt(float value) 
    {
        // convert 0 to 1 to 0 to 255 with a round for display
        int result = (int) (value * 255 + 0.5);
        // clamp it to 0, 255, just in case
        // Note - I tried to use std::min and std::max, but ended up having trouble due to windows
        // and linux headers.  Settled on old school cause it compiles.
        int temp = (result > 0) ? result : 0; // std::max(result, 0);
        result = (temp < 255) ? temp : 255; // std::min(temp, 255); 
        return result;
    }

    float DynamicColorElementControl::convertColorIntToFloat(int value)
    {
        // convert 0 to 255 back to 0 to 1. 
        float result = (float) (value / 255.0);
        // clamp it to 0.0, 1.0, just in case
        // Note - I tried to use std::min and std::max, but ended up having trouble due to windows
        // and linux headers.  Settled on old school cause it compiles.
        float temp = (0.0 > result) ? 0.0 : result; // std::min(0.0, result);
        result = (temp < 1.0) ? temp : 1.0; // std::max(temp, 1.0);
        return result;
    }

}
