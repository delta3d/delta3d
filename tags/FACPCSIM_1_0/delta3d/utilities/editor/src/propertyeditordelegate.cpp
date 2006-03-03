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

#include "dtEditQt/propertyeditormodel.h"
#include "dtEditQt/propertyeditordelegate.h"
#include "dtEditQt/dynamicabstractcontrol.h"

#include <QtGui/QPainter>
#include <QtGui/QFrame>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>

#include <QtGui/qdrawutil.h>
#include <QtCore/qdebug.h>
#include <limits.h>

namespace dtEditQt 
{


    /////////////////////////////////////////////////////////////////////////////////
    PropertyEditorDelegate::PropertyEditorDelegate(QObject *parent)
        : QItemDelegate(parent)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    PropertyEditorDelegate::~PropertyEditorDelegate()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////
    bool PropertyEditorDelegate::eventFilter(QObject *object, QEvent *event)
    {
        switch (event->type()) {
            case QEvent::KeyPress:
            case QEvent::KeyRelease: {
                QKeyEvent *ke = static_cast<QKeyEvent*>(event);

                // Up and Down
                if (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down) {
                    // Check for combo box
                    QWidget *widget = static_cast<QWidget*>(object);
                    QComboBox *comboBox= qobject_cast<QComboBox*>(widget);
                    if (comboBox != NULL && !(ke->modifiers() & Qt::ControlModifier)) {
                        break;  // send the event on to the combo box.
                    } else {
                        event->ignore();
                        return true;
                    }
                }

                // Right and left
                if (ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right) {
                    // if we have a push button, ignore hte left and right buttons
                    QWidget *widget = static_cast<QWidget*>(object);
                    if (qobject_cast<QPushButton*>(widget)) {
                        event->ignore();
                        return true;
                    }
                }

                // Enter and Return
                if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
                    QWidget *widget = static_cast<QWidget*>(object);
                    if (QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget)) {
                        spinBox->interpretText();
                    } else if (qobject_cast<QComboBox*>(widget)) {
                        // pass  the event onto the combo box, then commit
                        bool result = QItemDelegate::eventFilter(object, event);
                        emit commitData(widget);
                        return result;
                    }

                    emit commitData(widget);
                    return true;
                }
            } break;
            
            default:
                break;
        }

        // send the event on to the object so that it can handle it.
        return QItemDelegate::eventFilter(object, event);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt,
                        const QModelIndex &index) const
    {
        // give the control a chance to do custom painting for this column
        const PropertyEditorModel *model = dynamic_cast<const PropertyEditorModel *>(index.model());
        if (model != NULL) {
            DynamicAbstractControl *control = model->privateData(index);
            if (control != NULL && control->isControlDoesCustomPainting(index.column())) {
                control->paintColumn(index.column(), painter, opt);
                return;
            }
        }

        QItemDelegate::paint(painter, opt, index);
    }

    ///////////////////////////////////////////////////////////////////////////////
    QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem &opt,
                            const QModelIndex &index) const
    {


        // if our control does custom painting for this column, then get the sizeHint
        const PropertyEditorModel *model = dynamic_cast<const PropertyEditorModel *>(index.model());
        if (model != NULL) {
            DynamicAbstractControl *control = model->privateData(index);
            if (control != NULL && control->isControlDoesCustomPainting(index.column())) {
                return control->sizeHint(index.column(), opt);
            }
        }

        return QItemDelegate::sizeHint(opt, index) + QSize(2,2);
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *PropertyEditorDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
    {
        QWidget *editor = NULL;

        const PropertyEditorModel *model = dynamic_cast<const PropertyEditorModel *>(index.model());
        if (model != NULL) {
            DynamicAbstractControl *control = model->privateData(index);
            if (control != NULL) {
                editor = control->createEditor(parent, option, index);//, SLOT(sync()));
                // we want the delegate to control events
                if (editor != NULL)  {
                    control->installEventFilterOnControl(const_cast<PropertyEditorDelegate *>(this));
                    editor->installEventFilter(const_cast<PropertyEditorDelegate *>(this));
                }
            }
        }


        return editor;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        const PropertyEditorModel *model = dynamic_cast<const PropertyEditorModel *>(index.model());
        if (model != NULL) {
            DynamicAbstractControl *control = model->privateData(index);
            if (control != NULL) {
                control->updateEditorFromModel(editor);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const
    {
        PropertyEditorModel *myModel = dynamic_cast<PropertyEditorModel *>(model);
        if (myModel != NULL) {
            DynamicAbstractControl *control = myModel->privateData(index);
            if (control != NULL) {
                control->updateModelFromEditor(editor);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditorDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
                                const QRect &rect, const QPixmap &pixmap) const
    {
        QItemDelegate::drawDecoration(painter, option, rect, pixmap);
    }

}
