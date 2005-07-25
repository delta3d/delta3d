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

#include "dtEditQt/dynamicgroupcontrol.h"
#include "dtEditQt/propertyeditormodel.h"
//#include "dtDAL/actorproxy.h"
//#include "dtDAL/ActorProperty.h"
//#include "dtDAL/EnginePropertyTypes.h"
#include "dtDAL/log.h"
#include <QWidget>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicGroupControl::DynamicGroupControl(const std::string &newName)
        : name(newName.c_str())
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicGroupControl::~DynamicGroupControl()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicGroupControl *DynamicGroupControl::getChildGroupControl(QString name)
    {
        std::vector<DynamicAbstractControl *>::iterator childIter;
        DynamicGroupControl *result = NULL;

        // walk the children to find group controls.
        for (childIter = children.begin(); childIter != children.end(); ++childIter) {
            DynamicAbstractControl *control = (*childIter);    
            if (control != NULL) {
                // for each group control, compare the name
                DynamicGroupControl *group = dynamic_cast<DynamicGroupControl *>(control);
                if (group != NULL && group->name == QString(name)) {
                    result = group;
                    break;
                }
            }
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicGroupControl::addChildControl(DynamicAbstractControl *child, PropertyEditorModel *model)
    {
        // Note - if you change the propertyeditor so that it adds and removes rows instead of destroying
        // the property editor, you need to work with the begin/endinsertrows methods of model.
        if (child != NULL) 
        {
            children.push_back(child);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicGroupControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicGroupControl::getDisplayName()
    {
        return name;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicGroupControl::updateData(QWidget *widget)
    {
        // groups don't have editors, nor do they have any data to update.
        // and they don't have to update their children, because the children 
        // get their own events if needed.

        return false;
    }

}
