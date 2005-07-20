/* 
* Delta3D Open Source Game and Simulation Engine Level Editor 
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
#ifndef DELTA_PROPERTYEDITORMODEL
#define DELTA_PROPERTYEDITORMODEL

//#include "qpropertyeditor_items_p.h"
#include "dtEditQt/dynamicabstractcontrol.h"
#include "dtEditQt/dynamicabstractparentcontrol.h"
#include "dtEditQt/dynamicgroupcontrol.h"
#include <QAbstractItemModel>

namespace dtEditQt 
{
    /**
     * This class is the model used to represent the properties for a selected object.  
     * This clas is fairly complex. It follows the model/view architecture for QT. 
     * The base idea for this class was taken from qpropertyeditor_model.h from the QT
     * Beta2 Designer application.  
     * 
     * Goto: QT\tools\designer\src\components\propertyeditor\qpropertyeditor_model.h 
     * to see the original.  The original class is distributable according to the
     * standard QT license (GPL).
     */
    class PropertyEditorModel: public QAbstractItemModel
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        PropertyEditorModel(QObject *parent = 0);

        /**
         * Destructor
         */
        ~PropertyEditorModel();

        /**
         * Remove the children of the root group object.  This is necessary because the 
         * model needs to tell the underlying QAbstractItemModel to update itself. 
         * If we don't remove items safely, then the base class will eventually
         * try to access an object that doesn't exist.
         */
        //void clearRootChildren();

        inline DynamicGroupControl *getRootControl() const
        { return rootControl; }

        inline void setRootControl(DynamicGroupControl *newRoot) 
        { rootControl = newRoot; }

        /**
         * This class returns the QModelIndex for the property.  The index comes 
         * from getting the parent of the property and then finding this properties 
         * row in the parent.
         */
        inline QModelIndex indexOf(DynamicAbstractControl *property, int column = 0) const
        {
            if (property == rootControl) 
                return createIndex(0, column, rootControl);

            if (property == NULL || property->getParent() == NULL) 
            {
                return QModelIndex(); // INVALID
            }

            DynamicAbstractControl *parent = property->getParent();
            int row = parent->getChildIndex(property);
            return createIndex(row, column, property);
        }

        /**
         * Convenience method - This returns the DynamicAbstractControl from the 
         * QModelIndex.  It's kinda wonky, but QT allows you to contruct a QModelIndex
         * with a void * pointer but doesn't allow you to access the data as a void * 
         * later.  Instead, we have to static cast it out from the QVariant in data().  
         */
        inline DynamicAbstractControl *privateData(const QModelIndex &index) const
        { 
            if (!index.isValid()) 
            {
                return NULL;
            } 
            else 
            {
                return static_cast<DynamicAbstractControl *>(index.internalPointer()); 
            }
        }

        Qt::ItemFlags flags(const QModelIndex &index) const;

    signals:
        void propertyChanged(DynamicAbstractControl *property);
        void resetProperty(const QString &name);

    public slots:
        //void setInitialInput(IProperty *initialInput);
        //void propertyAdded(DynamicAbstractControl *property);
        //void propertyRemoved(const QModelIndex &index);
        //void refresh(DynamicAbstractControl *property);
        //void refresh();

    public:
        //
        // QAbstractItemModel interface
        //
        virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
        virtual QModelIndex parent(const QModelIndex &index) const;

        virtual int rowCount(const QModelIndex &parent) const;
        virtual int columnCount(const QModelIndex &parent) const;
        virtual bool hasChildren(const QModelIndex &parent) const
        { return rowCount(parent) > 0; }

        virtual QVariant data(const QModelIndex &index, int role) const;
        //virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

        virtual bool isEditable(const QModelIndex &index) const;
        //virtual QModelIndex buddy(const QModelIndex &index) const;

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        //virtual bool removeRows(int position, int rows, const QModelIndex &parent);

    protected:
        //inline IProperty *childAt(IProperty *parent, int pos) const
        // {
        //    if (parent && parent->kind() == IProperty::Property_Group)
        //        return static_cast<IPropertyGroup*>(parent)->propertyAt(pos);
        //    return 0;
        //}

    private:
        //void refreshHelper(DynamicAbstractControl *property);
        DynamicGroupControl *rootControl;//IProperty *m_initialInput;

        // the parent control class needs to be a friend so that it can call a stupid beginremoverow method.
        friend class DynamicAbstractParentControl;
        friend class PropertyEditor;
        friend class DynamicGroupControl;
    };

} 

#endif 
