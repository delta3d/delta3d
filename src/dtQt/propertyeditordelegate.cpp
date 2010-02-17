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
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditordelegate.h>
#include <dtQt/dynamicabstractcontrol.h>

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

#ifdef KeyPress
#undef KeyPress
#endif

#ifdef KeyRelease
#undef KeyRelease
#endif

namespace dtQt
{

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditorDelegate::PropertyEditorDelegate(QObject* parent)
      : QItemDelegate(parent)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditorDelegate::~PropertyEditorDelegate()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////
   bool PropertyEditorDelegate::eventFilter(QObject* object, QEvent* event)
   {
      switch (event->type())
      {
      case QEvent::KeyPress:
      case QEvent::KeyRelease:
         {
            QKeyEvent* ke = static_cast<QKeyEvent*>(event);

            // Enter and Return
            if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return)
            {
               QWidget* widget = static_cast<QWidget*>(object);
               if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget))
               {
                  spinBox->interpretText();
               }
               else if (qobject_cast<QComboBox*>(widget))
               {
                  // pass  the event onto the combo box, then commit
                  bool result = QItemDelegate::eventFilter(object, event);
                  emit commitData(widget);
                  return result;
               }

               emit commitData(widget);
               return true;
            }
         }
         break;
      default:
         break;
      }

      // send the event on to the object so that it can handle it.
      return QItemDelegate::eventFilter(object, event);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorDelegate::paint(QPainter* painter, const QStyleOptionViewItem& opt,
      const QModelIndex& index) const
   {
      // give the control a chance to do custom painting for this column
      const PropertyEditorModel* model = dynamic_cast<const PropertyEditorModel* >(index.model());
      if (model != NULL)
      {
         DynamicAbstractControl* control = model->GetAbstractControlFromIndex(index);
         if (control != NULL && control->isControlDoesCustomPainting(index.column()))
         {
            control->paintColumn(index.column(), painter, opt);
            return;
         }
      }

      QItemDelegate::paint(painter, opt, index);
   }

   ///////////////////////////////////////////////////////////////////////////////
   QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem& opt,
      const QModelIndex& index) const
   {
      // if our control does custom painting for this column, then get the sizeHint
      const PropertyEditorModel* model = dynamic_cast<const PropertyEditorModel*>(index.model());
      if (model != NULL)
      {
         DynamicAbstractControl* control = model->GetAbstractControlFromIndex(index);
         if (control != NULL && control->isControlDoesCustomPainting(index.column()))
         {
            return control->sizeHint(index.column(), opt);
         }
      }

      return QItemDelegate::sizeHint(opt, index) + QSize(2,2);
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* PropertyEditorDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option,
      const QModelIndex& index) const
   {
      QWidget* editor = NULL;

      const PropertyEditorModel* model = dynamic_cast<const PropertyEditorModel*>(index.model());
      if (model != NULL)
      {
         DynamicAbstractControl* control = model->GetAbstractControlFromIndex(index);
         if (control != NULL)
         {
            editor = control->createEditor(parent, option, index);//, SLOT(sync()));

            connect(this, SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)),
               control, SLOT(handleSubEditDestroy(QWidget*, QAbstractItemDelegate::EndEditHint)));

            // we want the delegate to control events
            if (editor != NULL)
            {
               control->InstallEventFilterOnControl(const_cast<PropertyEditorDelegate*>(this));

               editor->installEventFilter(const_cast<PropertyEditorDelegate*>(this));
            }
         }
      }

      return editor;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
   {
      const PropertyEditorModel* model = dynamic_cast<const PropertyEditorModel*>(index.model());
      if (model != NULL)
      {
         DynamicAbstractControl* control = model->GetAbstractControlFromIndex(index);
         if (control != NULL)
         {
            control->updateEditorFromModel(editor);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
   {
      PropertyEditorModel* myModel = dynamic_cast<PropertyEditorModel*>(model);
      if (myModel != NULL)
      {
         DynamicAbstractControl* control = myModel->GetAbstractControlFromIndex(index);
         if (control != NULL)
         {
            control->updateModelFromEditor(editor);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorDelegate::drawDecoration(QPainter* painter, const QStyleOptionViewItem& option,
      const QRect& rect, const QPixmap& pixmap) const
   {
      QItemDelegate::drawDecoration(painter, option, rect, pixmap);
   }

} // namespace dtEditQt
