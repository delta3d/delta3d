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

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>
#include <osg/Vec3>
#include "dtEditQt/dynamicvec3control.h"
#include "dtEditQt/dynamicvectorelementcontrol.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtUtil/log.h>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVec3Control::DynamicVec3Control()
        : xElement(NULL), yElement(NULL), zElement(NULL)//, 
          //myVec3Property(NULL), myVec3fProperty(NULL), myVec3dProperty(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicVec3Control::~DynamicVec3Control()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVec3Control::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
        // all cases in Linux with gcc4.  So we replaced it with a static cast.   
        if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::VEC3)
        {
            myVec3Property = static_cast<dtDAL::Vec3ActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create X
            xElement = new DynamicVectorElementControl(myVec3Property.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec3Property.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec3Property.get(), 2, "Z");
            zElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(zElement);
        } 
        else if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::VEC3F)
        {
            myVec3fProperty = static_cast<dtDAL::Vec3fActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create X
            xElement = new DynamicVectorElementControl(myVec3fProperty.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec3fProperty.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec3fProperty.get(), 2, "Z");
            zElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(zElement);
        } 
        else if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::VEC3D)
        {
            myVec3dProperty = static_cast<dtDAL::Vec3dActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create X
            xElement = new DynamicVectorElementControl(myVec3dProperty.get(), 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myVec3dProperty.get(), 1, "Y");
            yElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(yElement);
            // create Z
            zElement = new DynamicVectorElementControl(myVec3dProperty.get(), 2, "Z");
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
    const QString DynamicVec3Control::getDisplayName()
    {
        if(myVec3Property.valid())
        {
            return QString(tr(myVec3Property->GetLabel().c_str()));
        }
        else if(myVec3fProperty.valid())
        {
            return QString(tr(myVec3fProperty->GetLabel().c_str()));
        }
        else if(myVec3dProperty.valid())
        {
            return QString(tr(myVec3dProperty->GetLabel().c_str()));
        }
        else
        {
           LOG_ERROR("Dynamic Vec 3 control has an invalid property type");
           return tr("");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec3Control::getDescription()
    {
        if(myVec3Property.valid())
        {
            std::string tooltip = myVec3Property->GetDescription() + "  [Type: " +
                myVec3Property->GetPropertyType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else if(myVec3fProperty.valid())
        {
            std::string tooltip = myVec3fProperty->GetDescription() + "  [Type: " +
                myVec3fProperty->GetPropertyType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else if(myVec3dProperty.valid())
        {
            std::string tooltip = myVec3dProperty->GetDescription() + "  [Type: " +
                myVec3dProperty->GetPropertyType().GetName() + "]";
            return QString(tr(tooltip.c_str()));
        }
        else
        {
           LOG_ERROR("Dynamic Vec 3 control has an invalid property type");
           return tr("");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec3Control::getValueAsString()
    {
        if(myVec3Property.valid())
        {
            const osg::Vec3 &vectorValue = myVec3Property->GetValue();

            return "(X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) +
                ", Z=" + QString::number(vectorValue.z(), 'f', NUM_DECIMAL_DIGITS) + ")";
        }
        else if(myVec3fProperty.valid())
        {
            const osg::Vec3f &vectorValue = myVec3fProperty->GetValue();

            return "(X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) +
                ", Z=" + QString::number(vectorValue.z(), 'f', NUM_DECIMAL_DIGITS) + ")";
        }
        else if(myVec3dProperty.valid())
        {
            const osg::Vec3d &vectorValue = myVec3dProperty->GetValue();

            return "(X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
                ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) +
                ", Z=" + QString::number(vectorValue.z(), 'f', NUM_DECIMAL_DIGITS) + ")";
        }
        else
        {
           LOG_ERROR("Dynamic Vec 3 control has an invalid property type");
           return tr("");
        }
    }

    bool DynamicVec3Control::isEditable()
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicVec3Control::updateData(QWidget *widget)
    {
        // this guy doesn't have any editors.  All the data is edited in child controls
        return false;
    }
}
