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
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>
#include "dtEditQt/dynamicvec2control.h"
#include "dtEditQt/dynamicvectorelementcontrol.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtUtil/log.h>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVec2Control::DynamicVec2Control()
        : xElement(NULL), yElement(NULL)//, 
          //myVec2Property(NULL), myVec2fProperty(NULL), myVec2dProperty(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicVec2Control::~DynamicVec2Control()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVec2Control::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
        // all cases in Linux with gcc4.  So we replaced it with a static cast.   
        if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::VEC2) 
        {
            myVec2Property = static_cast<dtDAL::Vec2ActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create X
            xElement = new DynamicVectorElementControl(myVec2Property.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec2Property.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
        } 
        else if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::VEC2F) 
        {
            myVec2fProperty = static_cast<dtDAL::Vec2fActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create X
            xElement = new DynamicVectorElementControl(myVec2fProperty.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec2fProperty.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
        } 
        else if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::VEC2D) 
        {
            myVec2dProperty = static_cast<dtDAL::Vec2dActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create X
            xElement = new DynamicVectorElementControl(myVec2dProperty.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec2dProperty.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
        } 
        else 
        {
            std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
            LOG_ERROR("Cannot create dynamic control because property [" + 
                propertyName + "] is not the correct type.");
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec2Control::getDisplayName()
    {
        if(myVec2Property.valid())
            return QString(tr(myVec2Property->GetLabel().c_str()));
        else if(myVec2fProperty.valid())
            return QString(tr(myVec2fProperty->GetLabel().c_str()));
        else if(myVec2dProperty.valid())
            return QString(tr(myVec2dProperty->GetLabel().c_str()));
        else
        {
           LOG_ERROR("Dynamic Vec 2 control has an invalid property type");
           return tr("");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec2Control::getDescription()
    {
        if(myVec2Property.valid())
        {
            std::string tooltip = myVec2Property->GetDescription() + "  [Type: " +
                myVec2Property->GetDataType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else if(myVec2fProperty.valid())
        {
            std::string tooltip = myVec2fProperty->GetDescription() + "  [Type: " +
                myVec2fProperty->GetDataType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else if(myVec2dProperty.valid())
        {
            std::string tooltip = myVec2dProperty->GetDescription() + "  [Type: " +
                myVec2dProperty->GetDataType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else
        {
           LOG_ERROR("Dynamic Vec 2 control has an invalid property type");
           return tr("");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec2Control::getValueAsString()
    {
       osg::Vec3 testVect;
       bool isVecFloat = (sizeof(testVect.x()) == sizeof(float));
       const unsigned int NUM_DECIMAL_DIGITS = isVecFloat ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE;

        if(myVec2Property.valid())
        {
            const osg::Vec2 &vectorValue = myVec2Property->GetValue();

            return "(X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) + ")";
        }
        else if(myVec2fProperty.valid())
        {
            const osg::Vec2f &vectorValue = myVec2fProperty->GetValue();

            return "(X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS_FLOAT) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS_FLOAT) + ")";
        }
        else if(myVec2dProperty.valid())
        {
            const osg::Vec2d &vectorValue = myVec2dProperty->GetValue();

            return "(X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS_DOUBLE) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS_DOUBLE) + ")";
        }
        else
        {
           LOG_ERROR("Dynamic Vec 2 control has an invalid property type");
           return tr("");
        }
    }

    bool DynamicVec2Control::isEditable()
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicVec2Control::updateData(QWidget *widget)
    {
        // this guy doesn't have any editors.  All the data is edited in child controls
        return false;
    }
}
