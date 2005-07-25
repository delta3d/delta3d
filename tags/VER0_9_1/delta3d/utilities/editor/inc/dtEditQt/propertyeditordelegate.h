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

#ifndef DELTA_PROPERTYEDITORDELEGATE
#define DELTA_PROPERTYEDITORDELEGATE

#include <QItemDelegate>
//#include <QAbstractItemDelegate>
#include "dtEditQt/dynamicabstractcontrol.h"

namespace dtEditQt 
{

    /**
     * This class is a delegate for editing commands between the model and the
     * dynamic control.  Essentially, this is the 'control' part of the MVC
     * architecture, but QT intentionally doesn't call it that.
     *
     * The base idea for this class was taken from qpropertyeditor_delegate.h from the QT
     * Beta2 Designer application.
     *
     * Goto: QT\tools\designer\src\components\propertyeditor\qpropertyeditor_delegate.h
     * to see the original.  The original class is distributable according to the
     * standard QT license (GPL).
     */
    class PropertyEditorDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        PropertyEditorDelegate(QObject *parent = 0);

        /**
         * Destructor
         */
        virtual ~PropertyEditorDelegate();

        virtual bool eventFilter(QObject *object, QEvent *event);

        //
        // QItemDelegate Interface
        //
        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

        virtual QSize sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const;

        virtual QWidget *createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const;

        virtual void setEditorData(QWidget *editor,
                                const QModelIndex &index) const;

        virtual void setModelData(QWidget *editor,
                                QAbstractItemModel *model,
                                const QModelIndex &index) const;


    protected:
        virtual void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QRect &rect, const QPixmap &pixmap) const;

    };

}

#endif
