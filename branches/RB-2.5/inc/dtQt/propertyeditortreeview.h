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
#ifndef DELTA_PROPERTYEDITORTREEVIEW
#define DELTA_PROPERTYEDITORTREEVIEW

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/propertyeditormodel.h>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QAbstractItemDelegate>

#include <QtGui/QTreeView>
#include <QtGui/QColor>

class QWidget;

namespace dtQt
{
   class DynamicGroupControl;
   class PropertyEditorDelegate;

   /**
    * This class is the tree control for the properties of a proxy.  It knows
    * how to draw itself based on the underlying data in the PropertyEditorModel.
    * This clas follows the model/view architecture for QT.
    * The base idea for this class was taken from qpropertyeditor.h from the QT
    * Beta2 Designer application.
    *
    * Goto: QT\tools\designer\src\components\propertyeditor\qpropertyeditor.h
    * to see the original.  The original class is distributable according to the
    * standard QT license (GPL).
    */
   class DT_QT_EXPORT PropertyEditorTreeView : public QTreeView
   {
      Q_OBJECT

   public:
      /**
       * The row colors are made public constants so that controls that are
       * embedded inside the table can determine what  color backgrounds to use.
       */
      static const QColor ROW_COLOR_EVEN;
      static const QColor ROW_COLOR_ODD;

      /**
       * Constructor
       */
      PropertyEditorTreeView(PropertyEditorModel* model, QWidget* parent = 0);

      /**
       * Destructor
       */
      ~PropertyEditorTreeView();

      /**
       * Is the tree view read only?   Yes it is.
       */
      bool isReadOnly() const
      {
         return true;
      }

      /**
       * Used to set the root item.  This causes several settings to occur.
       */
      void setRoot(DynamicGroupControl* newRoot);

   public slots:
      virtual void reset();

   protected slots:
      virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous);
      virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
      virtual void closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint);

   private:
      PropertyEditorModel*    propertyModel;
      PropertyEditorDelegate* delegate;
      const QModelIndex*      mPreviousIndex;
   };

} // namespace dtEditQt

#endif // DELTA_PROPERTYEDITORTREEVIEW
