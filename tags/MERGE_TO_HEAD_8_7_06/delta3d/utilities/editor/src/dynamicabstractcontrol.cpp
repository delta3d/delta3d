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

#include "dtEditQt/dynamicabstractcontrol.h"
#include "dtEditQt/propertyeditortreeview.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtEditQt/editorevents.h"
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
        : initialized(false)//, newCommitEmitter(NULL)
    {
        connect(&EditorEvents::getInstance(), SIGNAL(actorPropertyChanged(ActorProxyRefPtr,ActorPropertyRefPtr)),
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
    void DynamicAbstractControl::handleSubEditDestroy(QWidget *widget) 
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
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractControl::installEventFilterOnControl(QObject *filterObj)
    {
        QObject::installEventFilter(filterObj);
    }


}
