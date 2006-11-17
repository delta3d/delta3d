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
* Curtiss Murphy
*/
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/dynamicabstractcontrol.h>
#include <dtEditQt/propertyeditormodel.h>
#include <dtEditQt/propertyeditortreeview.h>
#include <dtEditQt/propertyeditordelegate.h>
#include <QtCore/qdebug.h>
#include <QtGui/QItemDelegate>
#include <QtGui/qheaderview.h>
#include <QtGui/qpainter.h>
#include <QtGui/qscrollbar.h>
#include <QtCore/QAbstractItemModel>

namespace dtEditQt 
{

    const QColor PropertyEditorTreeView::ROW_COLOR_EVEN(237, 243, 254);
    const QColor PropertyEditorTreeView::ROW_COLOR_ODD(255, 255, 255);

    /////////////////////////////////////////////////////////////////////////////////
    PropertyEditorTreeView::PropertyEditorTreeView(PropertyEditorModel *model, QWidget *parent)
        : QTreeView(parent), propertyModel(model), mPreviousIndex(NULL)
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
    void PropertyEditorTreeView::setRoot(DynamicGroupControl *newRoot)
    {
        propertyModel->setRootControl(newRoot);
        reset();
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditorTreeView::currentChanged ( const QModelIndex & current, const QModelIndex & previous ) 
    {
      //I have to save the previous index so that I can look it up on
      //the closeEditor call so I can tell the control to 
      //cleanup.
      mPreviousIndex = &previous;
      QTreeView::currentChanged(current, previous);
      mPreviousIndex = NULL;
      
      if (current.isValid() && current.column() == 0) {
         DynamicAbstractControl *property = propertyModel->privateData(current);
         QModelIndex newSelection = propertyModel->indexOf(property, 1);
         setCurrentIndex(newSelection);
      }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditorTreeView::selectionChanged ( const QItemSelection & selected, 
                                                const QItemSelection & deselected )
    {
        QTreeView::selectionChanged(selected, deselected);

        QModelIndex current;
        QModelIndexList items = selected.indexes();

        foreach (current, items) {
            if (current.isValid() && current.column() == 0) {
                DynamicAbstractControl *property = propertyModel->privateData(current);
                QModelIndex newSelection = propertyModel->indexOf(property, 1);
                QRect rect = visualRect(newSelection);
                setSelection(rect, QItemSelectionModel::SelectCurrent);
                break;
            }
        }
    }
   
   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorTreeView::closeEditor(QWidget * editor, QAbstractItemDelegate::EndEditHint hint)
   {
      QTreeView::closeEditor(editor, hint);
      
      // On certain events, the delegate is not notified that the 
      // editor is being closed, which doesn't allow us to clean up our pointers.
      // This work around handles that case.
      PropertyEditorModel *currentModel = dynamic_cast<PropertyEditorModel *>(model());
      if (currentModel != NULL && mPreviousIndex != NULL) 
      {
         DynamicAbstractControl *control = currentModel->privateData(*mPreviousIndex);
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
      if (propertyModel->getRootControl() != NULL)
      {
         setRootIndex(propertyModel->indexOf(propertyModel->getRootControl()));
   
         header()->setResizeMode(1, QHeaderView::Stretch);
     
         resizeColumnToContents(0);
         setExpanded(propertyModel->indexOf(propertyModel->getRootControl()), true);
      }
   }
}
