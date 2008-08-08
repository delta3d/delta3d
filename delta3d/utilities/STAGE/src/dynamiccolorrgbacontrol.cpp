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
#include <dtEditQt/dynamiccolorelementcontrol.h>
#include <dtEditQt/dynamiccolorrgbacontrol.h>
#include <dtEditQt/dynamicsubwidgets.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/propertyeditortreeview.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>
#include <QtGui/QColorDialog>
#include <QtGui/QColor>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPaintDevice>
#include <QtGui/QPaintEngine>
#include <QtGui/QPushButton>
#include <QtCore/QSize>
#include <QtGui/QStyleOptionViewItem>
#include <QtGui/QWidget>

namespace dtEditQt {

    ///////////////////////////////////////////////////////////////////////////////
    DynamicColorRGBAControl::DynamicColorRGBAControl()
        : mTemporaryWrapper(NULL), mTemporaryEditOnlyTextLabel(NULL), mTemporaryColorPicker(NULL)
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
        if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::RGBACOLOR)
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
            myProperty->GetDataType().GetName() + "]";
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
        wrapper->setFocusPolicy(Qt::StrongFocus);
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
        mTemporaryEditOnlyTextLabel = new SubQLabel(getValueAsString(), wrapper, this);
        // set the background color to white so that it sort of blends in with the rest of the controls
        setBackgroundColor(mTemporaryEditOnlyTextLabel, PropertyEditorTreeView::ROW_COLOR_ODD);

        // button
        mTemporaryColorPicker = new SubQPushButton(tr("Pick ..."), wrapper, this);
        //temporaryColorPicker->setMaximumHeight(18);
        connect(mTemporaryColorPicker, SIGNAL(clicked()), this, SLOT(colorPickerPressed()));
        mTemporaryColorPicker->setToolTip(getDescription());

        // setup the horizontal layout 
        hBox->addWidget(mTemporaryColorPicker);
        hBox->addSpacing(3);
        hBox->addWidget(mTemporaryEditOnlyTextLabel);
        hBox->addStretch(1);
        
        mTemporaryWrapper = wrapper;
        return wrapper;
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicColorRGBAControl::isEditable()
    {
        return !myProperty->IsReadOnly();
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicColorRGBAControl::isNeedsPersistentEditor()
    {
        // we want our color picker to always show up. Because it's less confusing that way.
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::handleSubEditDestroy(QWidget *widget, QAbstractItemDelegate::EndEditHint hint)
    {
        if (widget == mTemporaryWrapper)
        {
            mTemporaryWrapper = NULL;
            mTemporaryEditOnlyTextLabel = NULL;
            mTemporaryColorPicker = NULL;        
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::installEventFilterOnControl(QObject *filterObj)
    {
        if (mTemporaryEditOnlyTextLabel != NULL)
            mTemporaryEditOnlyTextLabel->installEventFilter(filterObj);
        if (mTemporaryColorPicker != NULL)
            mTemporaryColorPicker->installEventFilter(filterObj);
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
        QColor result = QColorDialog::getColor(startColor, EditorData::GetInstance().getMainWindow());

        // if the user pressed, OK, we set the color and assume it changed
        if (result.isValid()) 
        {
            osg::Vec4 propColor = myProperty->GetValue();  // we at least need the old alpha
            propColor[0] = DynamicColorElementControl::convertColorIntToFloat(result.red());
            propColor[1] = DynamicColorElementControl::convertColorIntToFloat(result.green());
            propColor[2] = DynamicColorElementControl::convertColorIntToFloat(result.blue());
            // plus alpha...  remains the same.

            std::string oldValue = myProperty->ToString();
            myProperty->SetValue(propColor);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, myProperty,
                oldValue, myProperty->ToString());

            // update our label
            if (mTemporaryEditOnlyTextLabel !=  NULL)
            {
                mTemporaryEditOnlyTextLabel->setText(getValueAsString());
            }

            // notify the world (mostly the viewports) that our property changed
            EditorEvents::GetInstance().emitActorPropertyChanged(proxy, myProperty);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicColorRGBAControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
        dtCore::RefPtr<dtDAL::ActorProperty> property)
    {
        if (mTemporaryEditOnlyTextLabel != NULL && proxy == this->proxy && property == myProperty) 
        {
            mTemporaryEditOnlyTextLabel->setText(getValueAsString());
        }
    }
}
