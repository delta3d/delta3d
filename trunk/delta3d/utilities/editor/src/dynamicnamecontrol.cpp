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

#include "dtEditQt/dynamicnamecontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/log.h"
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicNameControl::DynamicNameControl()
        : temporaryEditControl(NULL)
    {
        // listen for name changes so we can update our own edit control
        connect(&EditorEvents::getInstance(), SIGNAL(proxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy>, std::string)),
            this, SLOT(proxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy>, std::string)));
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicNameControl::~DynamicNameControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicNameControl::updateEditorFromModel(QWidget *widget)
    {
        if (widget != NULL) 
        {
            // Note, don't use the temporary variable here.  It can cause errors with QT.
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);

            // set the current value from our property
            editBox->setText(tr(proxy->GetName().c_str()));
            editBox->selectAll();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicNameControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;
        std::string oldName;

        if (widget != NULL) 
        {
            // Note, don't use the temporary variable here.  It can cause errors with QT.
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);
            bool success = false;

            // get the data from our control
            std::string result = editBox->text().toStdString();

            // set our value to our object
            if (result != proxy->GetName()) 
            {
                oldName = proxy->GetName();
                proxy->SetName(result);
                dataChanged = true;
            }

            // reselect all the text when we commit.  
            // Gives the user visual feedback that something happened.
            editBox->selectAll();
        }

        // no notification cause it's not a property 
        if (dataChanged) {
            EditorEvents::getInstance().emitProxyNameChanged(proxy.get(), oldName);
        }

        return dataChanged;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicNameControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // create and init the edit box
        temporaryEditControl = new SubQLineEdit (parent, this);

        if (!initialized)  
        {
            LOG_ERROR("Tried to add itself to the parent widget before being initialized");
            return temporaryEditControl;
        }

        updateEditorFromModel(temporaryEditControl);

        // set the tooltip
        temporaryEditControl->setToolTip(getDescription());

        return temporaryEditControl;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicNameControl::getDisplayName()
    {
        return QString("Name");
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicNameControl::getDescription() 
    {
        return QString("A descriptive name for the actor that helps searching in both the editor and in code.  It may help implementaion if this is unique");
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicNameControl::getValueAsString() 
    {
        return QString(tr(proxy->GetName().c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicNameControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicNameControl::updateData(QWidget *widget)
    {
        // returns true if we successfully change data
        bool dataChanged = false;

        if (!initialized || widget == NULL)  
        {
            LOG_ERROR("Tried to updateData before being initialized");
            return dataChanged;
        }

        return updateModelFromEditor(widget);
    }
    
    /////////////////////////////////////////////////////////////////////////////////
    void DynamicNameControl::proxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy, std::string oldName)
    {
        if (temporaryEditControl != NULL && proxy == this->proxy) 
        {
            updateEditorFromModel(temporaryEditControl);
        }
    }
}
