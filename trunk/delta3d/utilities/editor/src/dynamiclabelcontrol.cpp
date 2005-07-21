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

#include "dtEditQt/dynamiclabelcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/log.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QWidget>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicLabelControl::DynamicLabelControl()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicLabelControl::~DynamicLabelControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicLabelControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        //myProperty = dynamic_cast<dtDAL::StringActorProperty *>(newProperty);
        DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

        // It's ok to do a NULL property.  It just means that someone is going to call the set 
        // display values soon.
        if (newProperty != NULL) {
            valueAsString = QString(tr("Unknown")); //newProperty->getValue().c_str()));

            std::string tooltip = newProperty->GetDescription() + "  [Type: " + 
                newProperty->GetPropertyType().GetName() + "]";
            description = QString(tr(tooltip.c_str()));

            name = QString(tr(newProperty->GetLabel().c_str()));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicLabelControl::getDisplayName()
    {
        return name;
        //return QString(tr(myProperty->getLabel().c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicLabelControl::getDescription() 
    {
        return description;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicLabelControl::getValueAsString() 
    {
        return valueAsString;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicLabelControl::setDisplayValues(QString newName, QString newDescription, QString newValueAsString)
    {
        name = newName;
        description = newDescription;
        valueAsString = newValueAsString;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicLabelControl::updateData(QWidget *widget)
    {
        // Labels don't have editors, nor do they have any data to update.
        return false;
    }

}
