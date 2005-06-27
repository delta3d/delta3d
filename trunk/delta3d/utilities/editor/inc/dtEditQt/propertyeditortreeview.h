/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2005, BMH Associates, Inc. 
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
* details.
*
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* @author Curtiss Murphy
*/
#ifndef DELTA_PROPERTYEDITORTREEVIEW
#define DELTA_PROPERTYEDITORTREEVIEW

#include "dtEditQt/dynamicabstractcontrol.h"
#include "dtEditQt/propertyeditormodel.h"
#include <QAbstractItemModel>

#include <QTreeView>
#include <QColor>

namespace dtEditQt 
{

//class PropertyEditorModel;
//class Delegate;
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
    class PropertyEditorTreeView: public QTreeView
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
        PropertyEditorTreeView(PropertyEditorModel *model, QWidget *parent = 0);

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
        void setRoot(DynamicGroupControl *newRoot);

    signals:
        void propertyChanged(DynamicAbstractControl *property);

    protected slots:
        virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous ) ;
        virtual void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );

    private:
        PropertyEditorModel *propertyModel;
        PropertyEditorDelegate *delegate;
    };

};

#endif 
