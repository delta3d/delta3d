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
#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>
#include <dtQt/propertyeditordelegate.h>
#include <QtCore/qdebug.h>
#include <QtGui/QItemDelegate>
#include <QtGui/qheaderview.h>
#include <QtGui/qpainter.h>
#include <QtGui/qscrollbar.h>
#include <QtCore/QAbstractItemModel>

namespace dtQt
{

   const QColor PropertyEditorTreeView::ROW_COLOR_EVEN(237, 243, 254);
   const QColor PropertyEditorTreeView::ROW_COLOR_ODD(255, 255, 255);

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditorTreeView::PropertyEditorTreeView(PropertyEditorModel* model, QWidget* parent)
      : QTreeView(parent)
      , propertyModel(model)
      , mPreviousIndex(NULL)
   {
      setModel(model);

      delegate = new PropertyEditorDelegate(this);
      setItemDelegate(delegate);

      setAlternatingRowColors(true);
      //setEvenRowColor(ROW_COLOR_EVEN);//QColor(237, 243, 254));
      //setOddRowColor(ROW_COLOR_ODD);//QColor(255, 255, 255));

      setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
      //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditorTreeView::~PropertyEditorTreeView()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorTreeView::setRoot(DynamicGroupControl* newRoot)
   {
      propertyModel->SetRootControl(newRoot);
      reset();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
   {
      // I have to save the previous index so that I can look it up on
      // the closeEditor call so I can tell the control to
      // cleanup.
      mPreviousIndex = &previous;
      QTreeView::currentChanged(current, previous);
      mPreviousIndex = NULL;

      if (current.isValid() && current.column() == 0)
      {
         DynamicAbstractControl* property = propertyModel->GetAbstractControlFromIndex(current);
         QModelIndex newSelection = propertyModel->IndexOf(property, 1);
         setCurrentIndex(newSelection);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorTreeView::selectionChanged(const QItemSelection& selected,
      const QItemSelection& deselected)
   {
      QTreeView::selectionChanged(selected, deselected);

      QModelIndex current;
      QModelIndexList items = selected.indexes();

      foreach (current, items)
      {
         if (current.isValid() && current.column() == 0)
         {
            DynamicAbstractControl* property = propertyModel->GetAbstractControlFromIndex(current);
            QModelIndex newSelection = propertyModel->IndexOf(property, 1);
            QRect rect = visualRect(newSelection);
            setSelection(rect, QItemSelectionModel::SelectCurrent);
            break;
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorTreeView::closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
   {
      QTreeView::closeEditor(editor, hint);

      // On certain events, the delegate is not notified that the
      // editor is being closed, which doesn't allow us to clean up our pointers.
      // This work around handles that case.
      PropertyEditorModel* currentModel = dynamic_cast<PropertyEditorModel*>(model());
      if (currentModel != NULL && mPreviousIndex != NULL)
      {
         DynamicAbstractControl* control = currentModel->GetAbstractControlFromIndex(*mPreviousIndex);
         if (control != NULL)
         {
            control->handleSubEditDestroy(editor, hint);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorTreeView::reset()
   {
      QTreeView::reset();
      setSelectionMode(QTreeView::SingleSelection);
      setSelectionBehavior(QTreeView::SelectItems);
      setRootIsDecorated(true);

      setTabKeyNavigation(true);
      setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked);
      if (propertyModel->GetRootControl() != NULL)
      {
         setRootIndex(propertyModel->IndexOf(propertyModel->GetRootControl()));

         header()->setResizeMode(1, QHeaderView::Stretch);

         resizeColumnToContents(0);
         setExpanded(propertyModel->IndexOf(propertyModel->GetRootControl()), true);
      }
   }

} // namespace dtEditQt
