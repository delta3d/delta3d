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

#include "dtEditQt/dynamicvec4control.h"
#include "dtEditQt/dynamicvectorelementcontrol.h"
#include <osg/Vec3>
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtUtil/log.h>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>

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
        if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::VEC4)
        {
            myVec4Property = static_cast<dtDAL::Vec4ActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create W
            wElement = new DynamicVectorElementControl(myVec4Property.get(), 0, "W");
            wElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(wElement);
            // create X
            xElement = new DynamicVectorElementControl(myVec4Property.get(), 1, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec4Property.get(), 2, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec4Property.get(), 3, "Z");
            zElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(zElement);
        }
        else if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::VEC4F)
        {
            myVec4fProperty = static_cast<dtDAL::Vec4fActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create W
            wElement = new DynamicVectorElementControl(myVec4fProperty.get(), 0, "W");
            wElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(wElement);
            // create X
            xElement = new DynamicVectorElementControl(myVec4fProperty.get(), 1, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec4fProperty.get(), 2, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec4fProperty.get(), 3, "Z");
            zElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(zElement);
        } 
        else if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::VEC4D)
        {
            myVec4dProperty = static_cast<dtDAL::Vec4dActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create W
            wElement = new DynamicVectorElementControl(myVec4dProperty.get(), 0, "W");
            wElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(wElement);
            // create X
            xElement = new DynamicVectorElementControl(myVec4dProperty.get(), 1, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec4dProperty.get(), 2, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec4dProperty.get(), 3, "Z");
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
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec4Control::getDescription()
    {
        if(myVec4Property.valid())
        {
            std::string tooltip = myVec4Property->GetDescription() + "  [Type: " +
                myVec4Property->GetPropertyType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else if(myVec4fProperty.valid())
        {
            std::string tooltip = myVec4fProperty->GetDescription() + "  [Type: " +
                myVec4fProperty->GetPropertyType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else if(myVec4dProperty.valid())
        {
            std::string tooltip = myVec4dProperty->GetDescription() + "  [Type: " +
                myVec4dProperty->GetPropertyType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec4Control::getValueAsString()
    {
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

            return "(W=" + QString::number(vectorValue.w(), 'f', NUM_DECIMAL_DIGITS) +
                ", X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) +
                ", Z=" + QString::number(vectorValue.z(), 'f', NUM_DECIMAL_DIGITS) + ")";
        }
        else if(myVec4dProperty.valid())
        {
            const osg::Vec4d &vectorValue = myVec4dProperty->GetValue();

            return "(W=" + QString::number(vectorValue.w(), 'f', NUM_DECIMAL_DIGITS) +
                ", X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) +
                ", Z=" + QString::number(vectorValue.z(), 'f', NUM_DECIMAL_DIGITS) + ")";
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
