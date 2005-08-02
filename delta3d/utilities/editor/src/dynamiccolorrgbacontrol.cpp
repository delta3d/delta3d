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

#include "dtEditQt/dynamiccolorelementcontrol.h"
#include "dtEditQt/dynamiccolorrgbacontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/mainwindow.h"
#include "dtEditQt/propertyeditortreeview.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtUtil/log.h>
#include <QColorDialog>
#include <QColor>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPaintDevice>
#include <QPaintEngine>
#include <QPushButton>
#include <QSize>
#include <QStyleOptionViewItem>
#include <QWidget>

namespace dtEditQt {

    ///////////////////////////////////////////////////////////////////////////////
    DynamicColorRGBAControl::DynamicColorRGBAControl()
        : temporaryEditOnlyTextLabel(NULL), temporaryColorPicker(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicColorRGBAControl::~DynamicColorRGBAControl()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        // Note - We used to have dynamic_cast in here, but it was failing to properly cast in 
        // all cases in Linux with gcc4.  So we replaced it with a static cast.   
        if (newProperty != NULL && newProperty->GetPropertyType() == dtDAL::DataType::RGBACOLOR)
        {
            myProperty = static_cast<dtDAL::ColorRgbaActorProperty*>(newProperty);

            DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

            // create R
            rElement = new DynamicColorElementControl(myProperty, 0, "R");
            rElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(rElement);
            // create G
            gElement = new DynamicColorElementControl(myProperty, 1, "G");
            gElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(gElement);
            // create B
            bElement = new DynamicColorElementControl(myProperty, 2, "B");
            bElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(bElement);
            // create A
            aElement = new DynamicColorElementControl(myProperty, 3, "A");
            aElement->initializeData(this, newModel, newProxy, newProperty);
            children.push_back(aElement);
        } 
        else 
        {
            std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
            LOG_ERROR("Cannot create dynamic control because property [" + 
                propertyName + "] is not the correct type.");
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicColorRGBAControl::getDisplayName()
    {
        return QString(tr(myProperty->GetLabel().c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicColorRGBAControl::getDescription() 
    {
        std::string tooltip = myProperty->GetDescription() + "  [Type: " + 
            myProperty->GetPropertyType().GetName() + "]";
        return QString(tr(tooltip.c_str()));
    }


    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicColorRGBAControl::getValueAsString() 
    {
        const osg::Vec4 &vectorValue = myProperty->GetValue();

        QString display;
        display.sprintf("(R=%d, G=%d, B=%d, A=%d)", 
            DynamicColorElementControl::convertColorFloatToInt(vectorValue[0]), 
            DynamicColorElementControl::convertColorFloatToInt(vectorValue[1]), 
            DynamicColorElementControl::convertColorFloatToInt(vectorValue[2]), 
            DynamicColorElementControl::convertColorFloatToInt(vectorValue[3]));
        return display;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::updateEditorFromModel(QWidget *widget)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicColorRGBAControl::updateModelFromEditor(QWidget *widget)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicColorRGBAControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        QWidget *wrapper = new QWidget(parent);
        // set the background color to white so that it sort of blends in with the rest of the controls
        setBackgroundColor(wrapper, PropertyEditorTreeView::ROW_COLOR_ODD);

        if (!initialized)  
        {
            LOG_ERROR("Tried to add itself to the parent widget before being initialized");
            return wrapper;
        }

        QHBoxLayout *hBox = new QHBoxLayout(wrapper);
        hBox->setMargin(0);
        hBox->setSpacing(0);

        // label 
        temporaryEditOnlyTextLabel = new SubQLabel(getValueAsString(), wrapper, this);
        // set the background color to white so that it sort of blends in with the rest of the controls
        setBackgroundColor(temporaryEditOnlyTextLabel, PropertyEditorTreeView::ROW_COLOR_ODD);

        // button
        temporaryColorPicker = new SubQPushButton(tr("Pick ..."), wrapper, this);
        temporaryColorPicker->setMaximumHeight(18);
        connect(temporaryColorPicker, SIGNAL(clicked()), this, SLOT(colorPickerPressed()));
        temporaryColorPicker->setToolTip(getDescription());
        // the button should get focus, not the wrapping widget
        wrapper->setFocusProxy(temporaryColorPicker);

        // setup the horizontal layout 
        hBox->addWidget(temporaryColorPicker);
        hBox->addSpacing(3);
        hBox->addWidget(temporaryEditOnlyTextLabel);
        hBox->addStretch(1);

        return wrapper;
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicColorRGBAControl::isEditable()
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicColorRGBAControl::isNeedsPersistentEditor()
    {
        // we want our color picker to always show up. Because it's less confusing that way.
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::handleSubEditDestroy(QWidget *widget)
    {
        if (widget == temporaryEditOnlyTextLabel)
            temporaryEditOnlyTextLabel = NULL;
        if (widget == temporaryColorPicker)
            temporaryColorPicker = NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::installEventFilterOnControl(QObject *filterObj)
    {
        if (temporaryEditOnlyTextLabel != NULL)
            temporaryEditOnlyTextLabel->installEventFilter(filterObj);
        if (temporaryColorPicker != NULL)
            temporaryColorPicker->installEventFilter(filterObj);
    }


    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicColorRGBAControl::updateData(QWidget *widget)
    {
        // this guy doesn't have any editors.  All the data is edited in child controls
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::colorPickerPressed() 
    {
        // get the current value.
        osg::Vec4 vectorValue = myProperty->GetValue();
        QColor startColor(DynamicColorElementControl::convertColorFloatToInt(vectorValue[0]), 
            DynamicColorElementControl::convertColorFloatToInt(vectorValue[1]), 
            DynamicColorElementControl::convertColorFloatToInt(vectorValue[2]));

        // show the dialog.  Blocks for user input.
        QColor result = QColorDialog::getColor(startColor, EditorData::getInstance().getMainWindow());

        // if the user pressed, OK, we set the color and assume it changed
        if (result.isValid()) 
        {
            osg::Vec4 propColor = myProperty->GetValue();  // we at least need the old alpha
            propColor[0] = DynamicColorElementControl::convertColorIntToFloat(result.red());
            propColor[1] = DynamicColorElementControl::convertColorIntToFloat(result.green());
            propColor[2] = DynamicColorElementControl::convertColorIntToFloat(result.blue());
            // plus alpha...  remains the same.

            std::string oldValue = myProperty->GetStringValue();
            myProperty->SetValue(propColor);

            // give undo manager the ability to create undo/redo events
            EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, myProperty,
                oldValue, myProperty->GetStringValue());

            // update our label
            if (temporaryEditOnlyTextLabel !=  NULL)
            {
                temporaryEditOnlyTextLabel->setText(getValueAsString());
            }

            // notify the world (mostly the viewports) that our property changed
            EditorEvents::getInstance().emitActorPropertyChanged(proxy, myProperty);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        if (temporaryEditOnlyTextLabel != NULL && proxy == this->proxy && property == myProperty) 
        {
            temporaryEditOnlyTextLabel->setText(getValueAsString());
        }
    }
}
