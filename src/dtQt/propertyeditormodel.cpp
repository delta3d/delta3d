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
#include <QtCore/qdebug.h>

namespace dtQt
{

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditorModel::PropertyEditorModel(QObject* parent)
      : QAbstractItemModel(parent), rootControl(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditorModel::~PropertyEditorModel()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicGroupControl* PropertyEditorModel::GetRootControl() const
   {
      return rootControl;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::SetRootControl(DynamicGroupControl* newRoot)
   {
      rootControl = newRoot;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QModelIndex PropertyEditorModel::IndexOf(DynamicAbstractControl* property, int column) const
   {
      if (property == rootControl)
      {
         return createIndex(0, column, rootControl);
      }

      if (property == NULL || property->getParent() == NULL)
      {
         return QModelIndex(); // INVALID
      }

      DynamicAbstractControl* parent = property->getParent();
      int row = parent->getChildIndex(property);
      return createIndex(row, column, property);
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* PropertyEditorModel::GetAbstractControlFromIndex(const QModelIndex& index) const
   {
      if (!index.isValid())
      {
         return NULL;
      }
      else
      {
         return static_cast<DynamicAbstractControl*>(index.internalPointer());
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   QModelIndex PropertyEditorModel::index(int row, int column, const QModelIndex& parent) const
   {
      DynamicAbstractControl* parentProp = GetAbstractControlFromIndex(parent);
      DynamicAbstractControl* childProp = NULL;

      if (rootControl == NULL)
      {
         return QModelIndex();
      }
      // createIndex(row, column, rootControl);

      if (!parent.isValid() || parentProp == NULL)
      {
         // special case for root.
         parentProp = rootControl;
      }

      if (parentProp != NULL)
      {
         childProp = parentProp->getChild(row);
      }

      if (childProp != NULL)
      {
         return createIndex(row, column, childProp);
      }
      else
      {
         return QModelIndex();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   QModelIndex PropertyEditorModel::parent(const QModelIndex& index) const
   {
      DynamicAbstractControl* prop = GetAbstractControlFromIndex(index);

      if (!index.isValid() || prop == NULL || prop == rootControl)
      {
         return QModelIndex();
      }

      return IndexOf(prop->getParent());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorModel::setDescription(DynamicAbstractControl* control)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   int PropertyEditorModel::rowCount(const QModelIndex& parent) const
   {
      DynamicAbstractControl* prop = GetAbstractControlFromIndex(parent);

      if (!parent.isValid() || prop == NULL)
      {
         prop = rootControl;
      }

      return prop->getChildCount();
   }

   /////////////////////////////////////////////////////////////////////////////////
   int PropertyEditorModel::columnCount(const QModelIndex& parent) const
   {
      return 2;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QVariant PropertyEditorModel::data(const QModelIndex& index, int role) const
   {
      DynamicAbstractControl* property = GetAbstractControlFromIndex(index);

      if (!index.isValid() || property == NULL)
      {
         return QVariant();
      }

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
   bool PropertyEditorModel::isEditable(const QModelIndex& index) const
   {
      DynamicAbstractControl* property = GetAbstractControlFromIndex(index);

      if (!index.isValid() || property == NULL || index.column() != 1)
      {
         return false;
      }
      else
      {
         return property->isEditable();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   QVariant PropertyEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
   {
      if (orientation == Qt::Horizontal)
      {
         if (role != Qt::DisplayRole)
         {
            return QVariant();
         }
         if (section == 0)
         {
            return QVariant("Property");
         }
         else if (section == 1)
         {
            return QVariant("Value");
         }
      }
      return QAbstractItemModel::headerData(section, orientation, role);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyEditorModel::insertRows(int position, int rows, const QModelIndex& parent)
   {
      beginInsertRows(parent, position, position + rows - 1);
      endInsertRows();
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyEditorModel::removeRows(int position, int rows, const QModelIndex& parent)
   {
      DynamicAbstractControl* property = GetAbstractControlFromIndex(parent);
      int childCount = property->getChildCount();
      for (int childIndex = 0; childIndex < childCount; ++childIndex)
      {
         DynamicAbstractControl* child = property->getChild(childIndex);

         if (child->getChildCount() > 0)
         {
            removeRows(0, child->getChildCount(), IndexOf(child));
         }
      }

      beginRemoveRows(parent, position, position + rows - 1);
      endRemoveRows();
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   Qt::ItemFlags PropertyEditorModel::flags(const QModelIndex& index) const
   {
      Qt::ItemFlags foo = QAbstractItemModel::flags(index);
      foo |= Qt::ItemIsEnabled;
      if (index.isValid())
      {
         foo |= Qt::ItemIsSelectable;
         if (isEditable(index))
         {
            foo |= Qt::ItemIsEditable;
         }
      }
      return foo;
   }

} // namespace dtEditQt
