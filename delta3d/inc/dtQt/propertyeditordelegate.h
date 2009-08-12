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

#ifndef DELTA_PROPERTYEDITORDELEGATE
#define DELTA_PROPERTYEDITORDELEGATE

#include <dtQt/export.h>
#include <QtGui/QItemDelegate>
#include <dtQt/dynamicabstractcontrol.h>

namespace dtQt
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
   class DT_QT_EXPORT PropertyEditorDelegate : public QItemDelegate
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      PropertyEditorDelegate(QObject* parent = 0);

      /**
       * Destructor
       */
      virtual ~PropertyEditorDelegate();

      virtual bool eventFilter(QObject* object, QEvent* event);

      //
      // QItemDelegate Interface
      //
      virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
         const QModelIndex& index) const;

      virtual QSize sizeHint(const QStyleOptionViewItem& option,
         const QModelIndex& index) const;

      virtual QWidget* createEditor(QWidget* parent,
         const QStyleOptionViewItem& option,
         const QModelIndex& index) const;

      virtual void setEditorData(QWidget* editor,
         const QModelIndex& index) const;

      virtual void setModelData(QWidget* editor,
         QAbstractItemModel* model,
         const QModelIndex& index) const;

   protected:
      virtual void drawDecoration(QPainter* painter, const QStyleOptionViewItem& option,
         const QRect& rect, const QPixmap& pixmap) const;
   };

} // namespace dtEditQt

#endif // DELTA_PROPERTYEDITORDELEGATE
