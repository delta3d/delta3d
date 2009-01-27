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
#include <dtEditQt/dynamiclabelcontrol.h>
#include <dtEditQt/dynamicsubwidgets.h>
#include <dtEditQt/editorevents.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/enginepropertytypes.h>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtCore/QString>
#include <QtGui/QWidget>

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
                newProperty->GetDataType().GetName() + "]";
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
