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
#include <dtEditQt/dynamicvec4control.h>
#include <dtEditQt/dynamicvectorelementcontrol.h>
#include <osg/Vec3>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVec4Control::DynamicVec4Control()
        : wElement(NULL), xElement(NULL), yElement(NULL), zElement(NULL)//, 
          //myVec4Property(NULL), myVec4fProperty(NULL), myVec4dProperty(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicVec4Control::~DynamicVec4Control()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVec4Control::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
        // all cases in Linux with gcc4.  So we replaced it with a static cast.   
        if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::VEC4)
        {
            myVec4Property = static_cast<dtDAL::Vec4ActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create W
            wElement = new DynamicVectorElementControl(myVec4Property.get(), 3, "W");
            wElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(wElement);
            // create X
            xElement = new DynamicVectorElementControl(myVec4Property.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec4Property.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec4Property.get(), 2, "Z");
            zElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(zElement);
        }
        else if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::VEC4F)
        {
            myVec4fProperty = static_cast<dtDAL::Vec4fActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create W
            wElement = new DynamicVectorElementControl(myVec4fProperty.get(), 3, "W");
            wElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(wElement);
            // create X
            xElement = new DynamicVectorElementControl(myVec4fProperty.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec4fProperty.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec4fProperty.get(), 2, "Z");
            zElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(zElement);
        } 
        else if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::VEC4D)
        {
            myVec4dProperty = static_cast<dtDAL::Vec4dActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create W
            wElement = new DynamicVectorElementControl(myVec4dProperty.get(), 3, "W");
            wElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(wElement);
            // create X
            xElement = new DynamicVectorElementControl(myVec4dProperty.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec4dProperty.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec4dProperty.get(), 2, "Z");
            zElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(zElement);
        } 
        else 
        {
            std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
            LOG_ERROR("Cannot create dynamic control because property [" + 
                propertyName + "] is not the correct type.");
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec4Control::getDisplayName()
    {
        if(myVec4Property.valid())
            return QString(tr(myVec4Property->GetLabel().c_str()));
        else if(myVec4fProperty.valid())
            return QString(tr(myVec4fProperty->GetLabel().c_str()));
        else if(myVec4dProperty.valid())
            return QString(tr(myVec4dProperty->GetLabel().c_str()));
        else
        {
           LOG_ERROR("Dynamic Vec 4 control has an invalid property type");
           return tr("");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec4Control::getDescription()
    {
        if(myVec4Property.valid())
        {
            std::string tooltip = myVec4Property->GetDescription() + "  [Type: " +
                myVec4Property->GetDataType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else if(myVec4fProperty.valid())
        {
            std::string tooltip = myVec4fProperty->GetDescription() + "  [Type: " +
                myVec4fProperty->GetDataType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else if(myVec4dProperty.valid())
        {
            std::string tooltip = myVec4dProperty->GetDescription() + "  [Type: " +
                myVec4dProperty->GetDataType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else
        {
           LOG_ERROR("Dynamic Vec 4 control has an invalid property type");
           return tr("");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec4Control::getValueAsString()
    {
       DynamicAbstractControl::getValueAsString();

       osg::Vec3 testVect;
       bool isVecFloat = (sizeof(testVect.x()) == sizeof(float));

       const unsigned int NUM_DECIMAL_DIGITS = isVecFloat ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE;

        if(myVec4Property.valid())
        {
            const osg::Vec4 &vectorValue = myVec4Property->GetValue();

            return "(W=" + QString::number(vectorValue.w(), 'f', NUM_DECIMAL_DIGITS) +
                ", X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) +
                ", Z=" + QString::number(vectorValue.z(), 'f', NUM_DECIMAL_DIGITS) + ")";
        }
        else if(myVec4fProperty.valid())
        {
            const osg::Vec4f &vectorValue = myVec4fProperty->GetValue();

            return "(W=" + QString::number(vectorValue.w(), 'f', NUM_DECIMAL_DIGITS_FLOAT) +
                ", X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS_FLOAT) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS_FLOAT) +
                ", Z=" + QString::number(vectorValue.z(), 'f', NUM_DECIMAL_DIGITS_FLOAT) + ")";
        }
        else if(myVec4dProperty.valid())
        {
            const osg::Vec4d &vectorValue = myVec4dProperty->GetValue();

            return "(W=" + QString::number(vectorValue.w(), 'f', NUM_DECIMAL_DIGITS_DOUBLE) +
                ", X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS_DOUBLE) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS_DOUBLE) +
                ", Z=" + QString::number(vectorValue.z(), 'f', NUM_DECIMAL_DIGITS_DOUBLE) + ")";
        }
        else
        {
           LOG_ERROR("Dynamic Vec 4 control has an invalid property type");
           return tr("");
        }
    }

    bool DynamicVec4Control::isEditable()
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicVec4Control::updateData(QWidget *widget)
    {
        // this guy doesn't have any editors.  All the data is edited in child controls
        return false;
    }
}
