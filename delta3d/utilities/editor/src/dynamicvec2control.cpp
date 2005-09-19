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
        : xElement(NULL), yElement(NULL)
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
        if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::VEC2) 
        {
            myProperty = static_cast<dtDAL::Vec2ActorProperty *>(newProperty);
            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create X
            xElement = new DynamicVectorElementControl(myProperty, 0, "X");
            xElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(xElement);
            // create Y
            yElement = new DynamicVectorElementControl(myProperty, 1, "Y");
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
        return QString(tr(myProperty->GetLabel().c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec2Control::getDescription()
    {
        std::string tooltip = myProperty->GetDescription() + "  [Type: " +
            myProperty->GetPropertyType().GetName() + "]";
        return QString(tr(tooltip.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVec2Control::getValueAsString()
    {
        const osg::Vec2 &vectorValue = myProperty->GetValue();

        return "(X=" + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
            ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) + ")";
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
