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
#include <dtEditQt/dynamicabstractcontrol.h>
#include <dtEditQt/propertyeditortreeview.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtEditQt/editorevents.h>
#include <QtGui/QColor> 
#include <QtGui/QPalette>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QSize>
#include <QtGui/QStyleOptionViewItem>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicAbstractControl::DynamicAbstractControl()
        : initialized(false), parent(NULL), model(NULL), propertyTree(NULL)//, newCommitEmitter(NULL)
    {
        connect(&EditorEvents::GetInstance(), SIGNAL(actorPropertyChanged(ActorProxyRefPtr,ActorPropertyRefPtr)),
            this, SLOT(actorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)));
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicAbstractControl::~DynamicAbstractControl()
    {}

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractControl::updateEditorFromModel(QWidget *widget)
    {
        // do nothing here, but allows controls without an editor to not have to override it
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicAbstractControl::updateModelFromEditor(QWidget *widget)
    {
        // do nothing here, but allows controls without an editor to not have to override it
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractControl::initializeData(DynamicAbstractControl *newParent, 
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        initialized = true;

        parent = newParent;
        proxy = newProxy;
        model = newModel;
    }

    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicAbstractControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // do nothing.  This method allows controls with no editor to not have to override it.
        return NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicAbstractControl *DynamicAbstractControl::getParent()
    {
        return parent;
    }

    /////////////////////////////////////////////////////////////////////////////////
    int DynamicAbstractControl::getChildIndex(DynamicAbstractControl *child) 
    {
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicAbstractControl *DynamicAbstractControl::getChild(int index) 
    {
        return NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////
    int DynamicAbstractControl::getChildCount() 
    {
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicAbstractControl::getDisplayName()
    {
        return QString("");
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicAbstractControl::getDescription() 
    {
        return QString("");
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicAbstractControl::getValueAsString() 
    {
        return QString("");
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicAbstractControl::isEditable()
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicAbstractControl::isControlDoesCustomPainting(int column)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractControl::paintColumn(int column, QPainter *painter, const QStyleOptionViewItem &opt)
    {
        // do nothing
    }

    /////////////////////////////////////////////////////////////////////////////////
    QSize DynamicAbstractControl::sizeHint(int column, const QStyleOptionViewItem &opt)
    {
        // do nothing
        return QSize();
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractControl::setTreeView(PropertyEditorTreeView *newPropertyTree)
    {
        propertyTree = newPropertyTree;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractControl::handleSubEditDestroy(QWidget *widget, QAbstractItemDelegate::EndEditHint hint) 
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicAbstractControl::isNeedsPersistentEditor()
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractControl::setBackgroundColor(QWidget *widget, QColor color)
    {
        QPalette palette(widget->palette());
        palette.setColor(QPalette::Active, QPalette::Background, color);
        palette.setColor(QPalette::Inactive, QPalette::Background, color);
        palette.setColor(QPalette::Disabled, QPalette::Background, color) ;
        widget->setPalette(palette);
        //4.1 only
        widget->setAutoFillBackground(true);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractControl::installEventFilterOnControl(QObject *filterObj)
    {
        QObject::installEventFilter(filterObj);
    }


}
