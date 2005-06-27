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
#include "dtEditQt/propertyeditormodel.h"
#include <QtCore/qdebug.h>

namespace dtEditQt
{

    /////////////////////////////////////////////////////////////////////////////////
    PropertyEditorModel::PropertyEditorModel(QObject *parent)
        : QAbstractItemModel(parent)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    PropertyEditorModel::~PropertyEditorModel()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    QModelIndex PropertyEditorModel::index(int row, int column, const QModelIndex &parent) const
    {
        DynamicAbstractControl *parentProp = privateData(parent);

        if (!parent.isValid() || parentProp == NULL)
            // special case for root.
            parentProp = rootControl;

        DynamicAbstractControl *childProp = parentProp->getChild(row);
        if (childProp != NULL) {
            return createIndex(row, column, childProp);
        } else {
            return QModelIndex();
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    QModelIndex PropertyEditorModel::parent(const QModelIndex &index) const
    {
        DynamicAbstractControl *prop = privateData(index);

        if (!index.isValid() || prop == NULL || prop == rootControl)
            return QModelIndex();

        return indexOf(prop->getParent());
    }

    /////////////////////////////////////////////////////////////////////////////////
    int PropertyEditorModel::rowCount(const QModelIndex &parent) const
    {
        DynamicAbstractControl *prop = privateData(parent);

        if (!parent.isValid() || prop == NULL)
            prop = rootControl;

        return prop->getChildCount();
    }

    /////////////////////////////////////////////////////////////////////////////////
    int PropertyEditorModel::columnCount(const QModelIndex &parent) const
    {
        return 2;
    }

    /////////////////////////////////////////////////////////////////////////////////
    QVariant PropertyEditorModel::data(const QModelIndex &index, int role) const
    {
        DynamicAbstractControl *property = privateData(index);

        if (!index.isValid() || property == NULL)
            return QVariant();

        switch (index.column())
        {  // ### cleanup
            case 0:
                switch (role)
                {
                    case Qt::EditRole:
                    case Qt::DisplayRole:
                        return QVariant(property->getDisplayName());
                    default:
                        break;
                }
                break;

            case 1:
            {
                switch (role)
                {
                    case Qt::EditRole:
                        // I believe the next line is irrelevant, since when we have
                        // an editor, it uses the editor.  However, it is called by QT. /shrug
                        return QVariant(property->getValueAsString());
                        break;
                    case Qt::DisplayRole:
                        return QVariant(property->getValueAsString());
                    case Qt::DecorationRole:
                        return QVariant(property->getDescription());
                    default:
                        break;
                }
                break;
            }

            default:
                break;
        }

        return QVariant();
    }


    /////////////////////////////////////////////////////////////////////////////////
    bool PropertyEditorModel::isEditable(const QModelIndex &index) const
    {
        DynamicAbstractControl *property = privateData(index);

        if (!index.isValid() || property == NULL || index.column() != 1)
            return false;
        else
            return property->isEditable();
    }

    /////////////////////////////////////////////////////////////////////////////////
    QVariant PropertyEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (orientation == Qt::Horizontal)
        {
            if (role != Qt::DisplayRole)
                return QVariant();
            if (section == 0)
                return QVariant("Property");
            else if (section == 1)
                return QVariant("Value");
        }
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    /////////////////////////////////////////////////////////////////////////////////
    Qt::ItemFlags PropertyEditorModel::flags(const QModelIndex &index) const
    {
        Qt::ItemFlags foo = QAbstractItemModel::flags(index);
        foo |= Qt::ItemIsEnabled;
        if (index.isValid())
        {
            foo |= Qt::ItemIsSelectable;
            if (isEditable(index))
                foo |= Qt::ItemIsEditable;
        }
        return foo;
    }

}
