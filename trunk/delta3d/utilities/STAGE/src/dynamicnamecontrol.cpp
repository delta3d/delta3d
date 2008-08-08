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
#include "dtEditQt/dynamicnamecontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtUtil/log.h>
#include <QtGui/QGridLayout>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicNameControl::DynamicNameControl()
        : temporaryEditControl(NULL)
    {
        // listen for name changes so we can update our own edit control
        connect(&EditorEvents::GetInstance(), SIGNAL(proxyNameChanged(ActorProxyRefPtr, std::string)),
            this, SLOT(proxyNameChanged(ActorProxyRefPtr, std::string)));
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
            //bool success = false;

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
        if(dataChanged) 
        {
            EditorEvents::GetInstance().emitProxyNameChanged(proxy.get(), oldName);
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
    void DynamicNameControl::proxyNameChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy, std::string oldName)
    {
        if (temporaryEditControl != NULL && proxy == this->proxy) 
        {
            updateEditorFromModel(temporaryEditControl);
        }
    }
}
